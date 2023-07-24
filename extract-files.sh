#!/bin/bash
#
# Copyright (C) 2016 The CyanogenMod Project
# Copyright (C) 2017-2020 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

set -e

DEVICE=fleur
VENDOR=xiaomi

# Load extract_utils and do some sanity checks
MY_DIR="${BASH_SOURCE%/*}"
if [[ ! -d "${MY_DIR}" ]]; then MY_DIR="${PWD}"; fi

ANDROID_ROOT="${MY_DIR}/../../.."

HELPER="${ANDROID_ROOT}/tools/extract-utils/extract_utils.sh"
if [ ! -f "${HELPER}" ]; then
    echo "Unable to find helper script at ${HELPER}"
    exit 1
fi
source "${HELPER}"

# Default to sanitizing the vendor folder before extraction
CLEAN_VENDOR=true

KANG=
SECTION=

while [ "${#}" -gt 0 ]; do
    case "${1}" in
        -n | --no-cleanup )
                CLEAN_VENDOR=false
                ;;
        -k | --kang )
                KANG="--kang"
                ;;
        -s | --section )
                SECTION="${2}"; shift
                CLEAN_VENDOR=false
                ;;
        * )
                SRC="${1}"
                ;;
    esac
    shift
done

if [ -z "${SRC}" ]; then
    SRC="adb"
fi

function blob_fixup {
    case "$1" in
    system/lib*/libsink.so)
		grep -q "libshim_sink.so" "${2}" || \
		"${PATCHELF}" --add-needed "libshim_sink.so" "${2}"
		;;
    vendor/bin/hw/android.hardware.gnss-service.mediatek |\
    vendor/lib64/hw/android.hardware.gnss-impl-mediatek.so)
        "$PATCHELF" --replace-needed "android.hardware.gnss-V1-ndk_platform.so" "android.hardware.gnss-V1-ndk.so" "$2"
        ;;
    vendor/lib/hw/vendor.mediatek.hardware.pq@2.13-impl.so)
        ;&
    vendor/lib64/hw/vendor.mediatek.hardware.pq@2.13-impl.so)
        "${PATCHELF}" --replace-needed "libutils.so" "libutils-v32.so" "${2}"
        ;;
    vendor/bin/hw/vendor.mediatek.hardware.mtkpower@1.0-service)
        "$PATCHELF" --replace-needed "android.hardware.power-V2-ndk_platform.so" "android.hardware.power-V2-ndk.so" "${2}"
        ;;
    vendor/etc/init/vendor.mediatek.hardware.mtkpower@1.0-service.rc)
        echo "$(cat ${2}) input" > "${2}"
        ;;
    vendor/bin/hw/android.hardware.media.c2@1.2-mediatek)
        ;&
    vendor/bin/hw/android.hardware.media.c2@1.2-mediatek-64b)
       "$PATCHELF" --replace-needed "libavservices_minijail_vendor.so" "libavservices_minijail.so" "$2"
	   	grep -q "libstagefright_foundation-v33.so" "${2}" || \
		"${PATCHELF}" --add-needed "libstagefright_foundation-v33.so" "${2}"
        ;;
    vendor/lib*/libwvhidl.so | vendor/lib*/mediadrm/libwvdrmengine.so)
        grep -q "libprotobuf-cpp-lite-3.9.1.so" "${2}" && \
        "${PATCHELF}" --replace-needed "libprotobuf-cpp-lite-3.9.1.so" "libprotobuf-cpp-full-3.9.1.so" "${2}"
        ;;
	vendor/bin/mnld | vendor/lib*/libaalservice.so | vendor/lib*/libcam.utils.sensorprovider.so)
		grep -q "libshim_sensors.so" "${2}" || \
		"${PATCHELF}" --add-needed "libshim_sensors.so" "${2}"
		;;
	esac
}

# Initialize the helper
setup_vendor "${DEVICE}" "${VENDOR}" "${ANDROID_ROOT}" false "${CLEAN_VENDOR}"

extract "${MY_DIR}/proprietary-files.txt" "${SRC}" "${KANG}" --section "${SECTION}"

"${MY_DIR}/setup-makefiles.sh"
