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
#ifndef SECURITY_COMPONENT_SERVICE_H
#define SECURITY_COMPONENT_SERVICE_H

#include <string>
#include <vector>
#include "app_state_observer.h"
#include "iremote_object.h"
#include "nlohmann/json.hpp"
#include "nocopyable.h"
#include "sec_comp_manager.h"
#include "sec_comp_stub.h"
#include "singleton.h"
#include "system_ability.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
enum class ServiceRunningState { STATE_NOT_START, STATE_RUNNING };
class __attribute__((visibility("default"))) SecCompService final : public SystemAbility, public SecCompStub {
    DECLARE_DELAYED_SINGLETON(SecCompService);
    DECLEAR_SYSTEM_ABILITY(SecCompService);

public:
    SecCompService(int32_t saId, bool runOnCreate);

    void OnStart() override;
    void OnStop() override;

    int32_t RegisterSecurityComponent(SecCompType type, const std::string& componentInfo, int32_t& scId) override;
    int32_t UpdateSecurityComponent(int32_t scId, const std::string& componentInfo) override;
    int32_t UnregisterSecurityComponent(int32_t scId) override;
    int32_t ReportSecurityComponentClickEvent(int32_t scId,
        const std::string& componentInfo, const SecCompClickEvent& clickInfo,
        sptr<IRemoteObject> callerToken, sptr<IRemoteObject> dialogCallback) override;
    bool VerifySavePermission(AccessToken::AccessTokenID tokenId) override;
    sptr<IRemoteObject> GetEnhanceRemoteObject() override;
    int32_t PreRegisterSecCompProcess() override;

    int Dump(int fd, const std::vector<std::u16string>& args) override;

private:
    int32_t ParseParams(const std::string& componentInfo, SecCompCallerInfo& caller, nlohmann::json& jsonRes);
    bool Initialize() const;
    bool RegisterAppStateObserver();
    void UnregisterAppStateObserver();
    bool GetCallerInfo(SecCompCallerInfo& caller);

    ServiceRunningState state_;
    sptr<AppExecFwk::IAppMgr> iAppMgr_;
    sptr<AppStateObserver> appStateObserver_;
};
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
#endif  // SECURITY_COMPONENT_SERVICE_H
