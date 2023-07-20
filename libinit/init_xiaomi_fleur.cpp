/*
 * Copyright (C) 2021-2022 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <libinit_dalvik_heap.h>
#include <libinit_variant.h>
#include <libinit_utils.h>

#include "vendor_init.h"

#define FINGERPRINT_FL "Redmi/fleur_global/fleur:12/SP1A.210812.016/V13.0.8.0.SKEMIXM:user/test-keys"
#define FINGERPRINT_FLP "POCO/fleur_p_global/fleur:12/SP1A.210812.016/V13.0.8.0.SKEMIXM:user/release-keys"
#define FINGERPRINT_ML "Redmi/miel_global/miel:12/SP1A.210812.016/V13.0.8.0.SKEMIXM:user/release-keys"
#define FINGERPRINT_MLP_IN "POCO/miel_p_in/miel:12/SP1A.210812.016/V13.0.5.0.SKEINXM:user/release-keys"
#define FINGERPRINT_ML_IN "Redmi/miel_in/miel:12/SP1A.210812.016/V13.0.5.0.SKEINXM:user/release-keys"

static const variant_info_t fleur_info = {
    .hwc_value = "Global",
    .sku_value = "fleur",
    .brand = "Redmi",
    .device = "fleur",
    .marketname = "Redmi Note 11s",
    .model = "2201117SY",
    .mod_device = "fleur_global",
    .build_fingerprint = FINGERPRINT_FL,
};

static const variant_info_t fleurp_info = {
    .hwc_value = "Global",
    .sku_value = "fleurp",
    .brand = "POCO",
    .device = "fleur",
    .marketname = "POCO M4 Pro",
    .model = "2201117PG",
    .mod_device = "fleur_p_global",
    .build_fingerprint = FINGERPRINT_FLP,
};

static const variant_info_t miel_info = {
    .hwc_value = "Global",
    .sku_value = "miel",
    .brand = "Redmi",
    .device = "miel",
    .marketname = "Redmi Note 11s",
    .model = "2201117SG",
    .mod_device = "miel_global",
    .build_fingerprint = FINGERPRINT_ML,
};

static const variant_info_t miel_in_info = {
    .hwc_value = "India",
    .sku_value = "miel",
    .brand = "Redmi",
    .device = "miel",
    .marketname = "Redmi Note 11s",
    .model = "2201117SI",
    .mod_device = "miel_in",
    .build_fingerprint = FINGERPRINT_ML_IN,
};

static const variant_info_t mielp_info = {
    .hwc_value = "India",
    .sku_value = "mielp",
    .brand = "POCO",
    .device = "miel",
    .marketname = "POCO M4 Pro",
    .model = "2201117PI",
    .mod_device = "miel_p_in",
    .build_fingerprint = FINGERPRINT_MLP_IN,
};

static const std::vector<variant_info_t> variants = {
    fleur_info,
    fleurp_info,
    miel_info,
    mielp_info,
    miel_in_info,
};

void vendor_load_properties() {
    set_dalvik_heap();
    search_variant(variants);

    // SafetyNet workaround
    property_override("ro.boot.verifiedbootstate", "green");
}
