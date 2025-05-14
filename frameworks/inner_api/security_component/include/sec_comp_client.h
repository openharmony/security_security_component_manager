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

#ifndef SECURITY_COMPONENT_CLIENT_H
#define SECURITY_COMPONENT_CLIENT_H

#include <condition_variable>
#include <mutex>
#include <string>
#include "access_token.h"
#include "isec_comp_service.h"
#include "sec_comp_death_recipient.h"
#include "sec_comp_enhance_adapter.h"
#include "sec_comp_err.h"
#include "sec_comp_info.h"
#include "security_component_service_ipc_interface_code.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
class SecCompClient final {
public:
    static SecCompClient& GetInstance();

    int32_t RegisterWriteToRawdata(SecCompType type, const std::string& componentInfo, SecCompRawdata& rawData);
    int32_t UpdateWriteToRawdata(int32_t scId, const std::string& componentInfo, SecCompRawdata& rawData);
    int32_t UnregisterWriteToRawdata(int32_t scId, SecCompRawdata& rawData);
    int32_t ReportWriteToRawdata(SecCompInfo& secCompInfo, SecCompRawdata& rawData, std::string& message);
    int32_t PreRegisterWriteToRawdata(SecCompRawdata& rawData);
    int32_t RegisterSecurityComponent(SecCompType type, const std::string& componentInfo, int32_t& scId);
    int32_t UpdateSecurityComponent(int32_t scId, const std::string& componentInfo);
    int32_t UnregisterSecurityComponent(int32_t scId);
    int32_t ReportSecurityComponentClickEvent(SecCompInfo& secCompInfo,
        sptr<IRemoteObject> callerToken, sptr<IRemoteObject> dialogCallback, std::string& message);
    bool VerifySavePermission(AccessToken::AccessTokenID tokenId);
    int32_t PreRegisterSecCompProcess();
    bool IsServiceExist();
    bool LoadService();
    bool IsSystemAppCalling();
    bool HasCustomPermissionForSecComp();

    void FinishStartSASuccess(const sptr<IRemoteObject>& remoteObject);
    void FinishStartSAFail();
    void OnRemoteDiedHandle();
    std::mutex useIPCMutex_;

private:
    SecCompClient();
    virtual ~SecCompClient();
    DISALLOW_COPY_AND_MOVE(SecCompClient);

    bool TryToGetSecCompSa();
    bool StartLoadSecCompSa();
    void WaitForSecCompSa();
    void GetSecCompSa();
    void LoadSecCompSa();
    sptr<ISecCompService> GetProxy(bool doLoadSa);
    void GetProxyFromRemoteObject(const sptr<IRemoteObject>& remoteObject);

    std::mutex cvLock_;
    bool readyFlag_ = false;
    std::condition_variable secComCon_;
    std::mutex proxyMutex_;
    sptr<ISecCompService> proxy_ = nullptr;
    sptr<SecCompDeathRecipient> serviceDeathObserver_ = nullptr;
};
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
#endif  // SECURITY_COMPONENT_CLIENT_H
