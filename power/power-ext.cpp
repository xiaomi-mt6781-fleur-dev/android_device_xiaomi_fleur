/*
 * Copyright (C) 2023 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "android.hardware.power-ext"

#include <linux/input.h>
#include <android-base/logging.h>
#include <android-base/file.h>
#include <aidl/android/hardware/power/BnPower.h>

constexpr int kWakeupModeOff = 4;
constexpr int kWakeupModeOn = 5;

namespace aidl {
namespace android {
namespace hardware {
namespace power {
namespace impl {
namespace mediatek {
    static int open_ts_input() {
        int fd = -1;
        DIR *dir = opendir("/dev/input");

        if (dir != NULL) {
            struct dirent *ent;

            while ((ent = readdir(dir)) != NULL) {
                if (ent->d_type == DT_CHR) {
                    char absolute_path[PATH_MAX] = {0};
                    char name[80] = {0};

                    strcpy(absolute_path, "/dev/input/");
                    strcat(absolute_path, ent->d_name);

                    fd = open(absolute_path, O_RDWR);
                    if (ioctl(fd, EVIOCGNAME(sizeof(name) - 1), &name) > 0) {
                        if (strcmp(name, "fts_ts") == 0 ||
                                strcmp(name, "NVTCapacitiveTouchScreen") == 0 ||
                                strcmp(name, "goodix_ts") == 0)
                            break;
                    }

                    close(fd);
                    fd = -1;
                }
            }

            closedir(dir);
        }

        return fd;
    }

    bool isDeviceSpecificModeSupported (Mode type, bool* _aidl_return) {
        switch (type) {
            case Mode::DOUBLE_TAP_TO_WAKE:
            {
                int fd = open_ts_input();
                if (fd != -1) {
                    *_aidl_return = true;
                    close(fd);
                    return true;
                } else {
                    *_aidl_return = false;
                    return false;
                }
            }
            default:
                return false;
        }
    }
    bool setDeviceSpecificMode(Mode type, bool enabled) {
        switch(type) {
            case Mode::DOUBLE_TAP_TO_WAKE:
            {
                int fd = open_ts_input();
                if (fd == -1)
                    return false;
                
                struct input_event ev = {
                    .type = EV_SYN,
                    .code = SYN_CONFIG,
                    .value = (enabled ? kWakeupModeOn : kWakeupModeOff),
                };
                write(fd, &ev, sizeof(ev));
                close(fd);
                return true;
            }
            default:
                return false;
        }
    }
} // namespace mediatek
} // namespace impl
} // namespace power
} // namespace hardware
} // namespace android
} // namespace aidl
