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
#include "access_token.h"
#include "app_state_observer.h"
#include "iremote_object.h"
#include "nlohmann/json.hpp"
#include "nocopyable.h"
#include "sec_comp_base.h"
#include "sec_comp_click_event_parcel.h"
#include "sec_comp_info.h"
#include "sec_comp_manager.h"
#include "sec_comp_service_stub.h"
#include "security_component_service_ipc_interface_code.h"
#include "singleton.h"
#include "system_ability.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
enum class ServiceRunningState { STATE_NOT_START, STATE_RUNNING };
class __attribute__((visibility("default"))) SecCompService final : public SystemAbility, public SecCompServiceStub {
    DECLARE_DELAYED_SINGLETON(SecCompService);
    DECLEAR_SYSTEM_ABILITY(SecCompService);

public:
    SecCompService(int32_t saId, bool runOnCreate);

    void OnStart() override;
    void OnStop() override;

    int32_t RegisterSecurityComponent(const SecCompRawdata& rawData, SecCompRawdata& rawReply) override;
    int32_t UpdateSecurityComponent(const SecCompRawdata& rawData, SecCompRawdata& rawReply) override;
    int32_t UnregisterSecurityComponent(const SecCompRawdata& rawData, SecCompRawdata& rawReply) override;
    int32_t ReportSecurityComponentClickEvent(const sptr<IRemoteObject>& callerToken,
        const sptr<IRemoteObject>& dialogCallback, const SecCompRawdata& rawData, SecCompRawdata& rawReply) override;
    int32_t VerifySavePermission(AccessToken::AccessTokenID tokenId, bool& isGranted) override;
    int32_t PreRegisterSecCompProcess(const SecCompRawdata& rawData, SecCompRawdata& rawReply) override;

    int Dump(int fd, const std::vector<std::u16string>& args) override;

private:
    int32_t WriteError(int32_t res, SecCompRawdata& rawReply);
    int32_t RegisterReadFromRawdata(SecCompRawdata& rawData, SecCompType& type, std::string& componentInfo);
    int32_t RegisterSecurityComponentBody(SecCompType type, const std::string& componentInfo, int32_t& scId);
    int32_t RegisterWriteToRawdata(int32_t res, int32_t scId, SecCompRawdata& rawReply);
    int32_t UpdateReadFromRawdata(SecCompRawdata& rawData, int32_t& scId, std::string& componentInfo);
    int32_t UpdateSecurityComponentBody(int32_t scId, const std::string& componentInfo);
    int32_t UpdateWriteToRawdata(int32_t res, SecCompRawdata& rawReply);
    int32_t UnregisterReadFromRawdata(SecCompRawdata& rawData, int32_t& scId);
    int32_t UnregisterSecurityComponentBody(int32_t scId);
    int32_t UnregisterWriteToRawdata(int32_t res, SecCompRawdata& rawReply);
    int32_t ReportSecurityComponentClickEventBody(SecCompInfo& secCompInfo,
        sptr<IRemoteObject> callerToken, sptr<IRemoteObject> dialogCallback, std::string& message);
    int32_t ReportWriteToRawdata(int32_t res, std::string message, SecCompRawdata& rawReply);
    int32_t PreRegisterReadFromRawdata(SecCompRawdata& rawData);
    int32_t PreRegisterSecCompProcessBody();
    int32_t PreRegisterWriteToRawdata(int32_t res, SecCompRawdata& rawReply);
    int32_t ParseParams(const std::string& componentInfo, SecCompCallerInfo& caller, nlohmann::json& jsonRes);
    bool Initialize() const;
    bool RegisterAppStateObserver();
    void UnregisterAppStateObserver();
    bool GetCallerInfo(SecCompCallerInfo& caller);
    bool IsMediaLibraryCalling();

    ServiceRunningState state_;
    sptr<AppExecFwk::IAppMgr> iAppMgr_;
    sptr<AppStateObserver> appStateObserver_;
    AccessToken::AccessTokenID mediaLibraryTokenId_ = 0;
};
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
#endif  // SECURITY_COMPONENT_SERVICE_H
