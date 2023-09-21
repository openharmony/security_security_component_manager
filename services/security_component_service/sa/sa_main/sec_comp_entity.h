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
#ifndef SECURITY_COMPONENT_ENTITY_H
#define SECURITY_COMPONENT_ENTITY_H

#include <memory>
#include "accesstoken_kit.h"
#include "sec_comp_base.h"
#include "sec_comp_info.h"
#include "sec_comp_perm_manager.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
class SecCompEntity {
public:
    SecCompEntity(std::shared_ptr<SecCompBase> component,
        AccessToken::AccessTokenID token, int32_t scId) : componentInfo_(component),
        tokenId_(token), scId_(scId) {};
    ~SecCompEntity() = default;
    int32_t RevokeTempPermission();
    int32_t GrantTempPermission();
    int32_t GetScId() const
    {
        return scId_;
    };

    SecCompType GetType() const
    {
        if (componentInfo_ == nullptr) {
            return UNKNOWN_SC_TYPE;
        }
        return componentInfo_->type_;
    };

    bool IsGrant() const
    {
        return isGrant_;
    }

    std::shared_ptr<SecCompBase> GetComponentInfo() const
    {
        return componentInfo_;
    };

    void SetComponentInfo(const std::shared_ptr<SecCompBase>& com)
    {
        componentInfo_ = com;
    };

    std::shared_ptr<SecCompBase> GetComponentInfo()
    {
        return componentInfo_;
    };

    bool CompareComponentBasicInfo(SecCompBase* other, bool isRectCheck) const;
    int32_t CheckTouchInfo(const SecCompClickEvent& touchInfo) const;

private:
    std::shared_ptr<SecCompBase> componentInfo_;
    bool isGrant_ = false;
    AccessToken::AccessTokenID tokenId_;
    int32_t scId_;
};
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
#endif // SECURITY_COMPONENT_ENTITY_H
