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
#ifndef TEST_COMMON_H
#define TEST_COMMON_H
#include "location_button.h"
#include "paste_button.h"
#include "save_button.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
class TestCommon {
public:
    static constexpr float TEST_SIZE = 100.0;
    static constexpr double TEST_COORDINATE = 100.0;
    static constexpr double TEST_DIMENSION = 100.0;
    static constexpr uint32_t TEST_COLOR = 0xffffffff;
    static constexpr uint32_t TEST_COLOR_YELLOW = 0xffffff00;
    static constexpr uint32_t TEST_COLOR_RED = 0xffff0000;
    static constexpr uint32_t TEST_COLOR_BLUE = 0xff0000ff;
    static constexpr uint32_t TEST_DIFF_COLOR = 0;
    static constexpr uint64_t TIME_CONVERSION_UNIT = 1000;
    static constexpr uint32_t HAP_TOKEN_ID = 537715419;
    static constexpr uint32_t MAX_HMAC_SIZE = 64;
    static constexpr size_t MAX_CALLER_SIZE = 10;

    static void BuildLocationComponentInfo(nlohmann::json& jsonComponent);
    static void BuildSaveComponentInfo(nlohmann::json& jsonComponent);
    static void BuildPasteComponentInfo(nlohmann::json& jsonComponent);
};
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
#endif  // TEST_COMMON_H
