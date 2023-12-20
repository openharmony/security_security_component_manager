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
#ifndef I_SECURITY_COMPONENT_WINDOW_INFO_HELPER_H
#define I_SECURITY_COMPONENT_WINDOW_INFO_HELPER_H

#include <cstdint>
#include "sec_comp_info.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
class WindowInfoHelper {
public:
    static float GetWindowScale(int32_t windowId);
    static bool CheckOtherWindowCoverComp(int32_t compWinId, const SecCompRect& secRect);
public:
    static constexpr float FULL_SCREEN_SCALE = 1.0F;
};
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
#endif  // I_SECURITY_COMPONENT_WINDOW_INFO_HELPER_H
