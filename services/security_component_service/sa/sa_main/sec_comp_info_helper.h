/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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
#ifndef I_SECURITY_COMPONENT_INFO_HELPER_H
#define I_SECURITY_COMPONENT_INFO_HELPER_H

#include "accesstoken_kit.h"
#include "nlohmann/json.hpp"
#include "sec_comp_base.h"
#include "sec_comp_info.h"
#include "sec_comp_perm_manager.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
template<typename T>
T* ConstructComponent(const nlohmann::json& jsonComponent)
{
    T *componentPtr = new (std::nothrow)T();
    if ((componentPtr != nullptr) && !componentPtr->FromJson(jsonComponent)) {
        delete componentPtr;
        return nullptr;
    }
    return componentPtr;
}

class __attribute__((visibility("default"))) SecCompInfoHelper {
public:
    static SecCompBase* ParseComponent(SecCompType type, const nlohmann::json& jsonComponent);
    static int32_t GrantTempPermission(AccessToken::AccessTokenID tokenId,
        const std::shared_ptr<SecCompBase>& componentInfo);
    static bool CheckComponentValid(SecCompBase* comp);
    static bool CheckRectValid(const SecCompRect& rect, const SecCompRect& windowRect);

private:
    static float GetWindowScale(int32_t windowId);
    static void AdjustSecCompRect(SecCompBase* comp, float scale);
};
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
#endif  // I_SECURITY_COMPONENT_INFO_HELPER_H
