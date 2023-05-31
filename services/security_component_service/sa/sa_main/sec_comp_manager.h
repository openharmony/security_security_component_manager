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

#ifndef SECURITY_COMPONENT_MANAGER_H
#define SECURITY_COMPONENT_MANAGER_H

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "accesstoken_kit.h"
#include "app_state_observer.h"
#include "nocopyable.h"
#include "rwlock.h"
#include "sec_comp_base.h"
#include "sec_comp_entity.h"
#include "sec_comp_info.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
struct SecCompCallerInfo {
    AccessToken::AccessTokenID tokenId;
    int32_t uid;
    int32_t pid;
};

class SecCompManager {
public:
    static SecCompManager& GetInstance();
    virtual ~SecCompManager() = default;

    int32_t RegisterSecurityComponent(SecCompType type, const nlohmann::json& jsonComponent,
        const SecCompCallerInfo& caller, int32_t& scId);
    int32_t UpdateSecurityComponent(int32_t scId, const nlohmann::json& jsonComponent,
        const SecCompCallerInfo& caller);
    int32_t UnregisterSecurityComponent(int32_t scId, const SecCompCallerInfo& caller);
    int32_t ReportSecurityComponentClickEvent(int32_t scId, const nlohmann::json& jsonComponent,
        const SecCompCallerInfo& caller, const SecCompClickEvent& touchInfo);
    bool ReduceAfterVerifySavePermission(AccessToken::AccessTokenID tokenId);
    void NotifyProcessForeground(int32_t uid);
    void NotifyProcessBackground(int32_t uid);
    void NotifyProcessDied(int32_t uid);
    void DumpSecComp(std::string& dumpStr) const;
    bool Initialize();

private:
    SecCompManager();
    int32_t AddProcessComponent(std::vector<SecCompEntity>& componentList,
        const SecCompEntity& newEntity);
    int32_t AddSecurityComponentToList(int32_t uid, const SecCompEntity& newEntity);
    int32_t DeleteSecurityComponentFromList(int32_t uid, int32_t scId);
    SecCompEntity* GetSecurityComponentFromList(int32_t uid, int32_t scId);
    void ExitSaAfterAllProcessDie();

    int32_t CreateScId();

    OHOS::Utils::RWLock componentInfoLock_;
    std::mutex scIdMtx_;
    std::unordered_map<int32_t, std::vector<SecCompEntity>> componentMap_;
    int32_t scValidCount_;
    int32_t scIdStart_;
    DISALLOW_COPY_AND_MOVE(SecCompManager);
};
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
#endif  // SECURITY_COMPONENT_MANAGER_H
