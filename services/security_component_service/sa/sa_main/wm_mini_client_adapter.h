/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#ifndef I_SECURITY_COMPONENT_WM_MINI_CLIENT_ADAPTER_H
#define I_SECURITY_COMPONENT_WM_MINI_CLIENT_ADAPTER_H

#include "wm_mini_client.h"

namespace OHOS {
namespace Rosen {
class WmMiniClientAdapter {
public:
    static Security::SecurityComponent::MiniWMError GetWindowInfo(int32_t userId,
        std::vector<sptr<Security::SecurityComponent::MiniAccessibilityWindowInfo>>& infos);
    static Security::SecurityComponent::MiniWMError GetUnreliableWindowInfo(int32_t windowId, int32_t userId,
        std::vector<sptr<Security::SecurityComponent::MiniUnreliableWindowInfo>>& infos);
    static Security::SecurityComponent::MiniAccessibilityWindowInfo* CreateWindowInfo();
    static Security::SecurityComponent::MiniUnreliableWindowInfo* CreateUnreliableWindowInfo();
};
}  // namespace Rosen
}  // namespace OHOS

#endif  // I_SECURITY_COMPONENT_WM_MINI_CLIENT_ADAPTER_H
