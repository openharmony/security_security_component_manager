/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef SERVICE_TEST_COMMON_H
#define SERVICE_TEST_COMMON_H
#include "location_button.h"
#include "paste_button.h"
#include "save_button.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
class ServiceTestCommon {
public:
    static constexpr int32_t TEST_UID_1 = 1;
    static constexpr int32_t TEST_UID_2 = 2;
    static constexpr int32_t TEST_PID_1 = 1;
    static constexpr int32_t TEST_PID_2 = 2;
    static constexpr int32_t TEST_PID_3 = 3;
    static constexpr int32_t TEST_SC_ID_1 = 1;
    static constexpr int32_t TEST_SC_ID_2 = 2;
    static constexpr int32_t TEST_INVALID_SC_ID = -1;
    static constexpr int32_t SC_ID_START = 1000;
    static constexpr int32_t SA_ID = 3506;

    static constexpr float TEST_SIZE = 100.0;
    static constexpr double TEST_COORDINATE = 100.0;
    static constexpr double TEST_DIFF_COORDINATE = 200.0;
    static constexpr double TEST_DIMENSION = 100.0;
    static constexpr double TEST_INVALID_DIMENSION = -100.0;
    static constexpr double ZERO_OFFSET = 0.0F;
    static constexpr uint32_t TEST_INVALID_SIZE = 0;
    static constexpr uint32_t QUARTER = 4;

    static constexpr uint32_t TEST_COLOR_INVALID = 0x66000000;
    static constexpr uint32_t TEST_COLOR_BLACK = 0xff000000;
    static constexpr uint32_t TEST_COLOR_WHITE = 0xffffffff;
    static constexpr uint32_t TEST_COLOR_YELLOW = 0xff7fff00;
    static constexpr uint32_t TEST_COLOR_RED = 0xffff0000;
    static constexpr uint32_t TEST_COLOR_BLUE = 0xff0000ff;

    static constexpr uint64_t TIME_CONVERSION_UNIT = 1000;
    static constexpr uint32_t MAX_INT_NUM = 0x7fffffff;
    static constexpr uint32_t HAP_TOKEN_ID = 537715419;
    static constexpr uint32_t TEST_TOKEN_ID = 1;

    static void BuildLocationComponentJson(nlohmann::json& jsonComponent);
    static void BuildSaveComponentJson(nlohmann::json& jsonComponent);
    static void BuildPasteComponentJson(nlohmann::json& jsonComponent);
};
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
#endif  // SERVICE_TEST_COMMON_H
