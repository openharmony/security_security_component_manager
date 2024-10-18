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
#ifndef I_SECURITY_COMPONENT_SERVICE_H
#define I_SECURITY_COMPONENT_SERVICE_H

#include <string>
#include "access_token.h"
#include "iremote_broker.h"
#include "sec_comp_info.h"
#include "security_component_service_ipc_interface_code.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
constexpr int32_t SA_ID_SECURITY_COMPONENT_SERVICE = 3506;

class ISecCompService : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.security.ISecCompService");

    virtual int32_t RegisterSecurityComponent(SecCompType type,
        const std::string& componentInfo, int32_t& scId) = 0;
    virtual int32_t UpdateSecurityComponent(int32_t scId, const std::string& componentInfo) = 0;
    virtual int32_t UnregisterSecurityComponent(int32_t scId) = 0;
    virtual int32_t ReportSecurityComponentClickEvent(int32_t scId, const std::string& componentInfo,
        const SecCompClickEvent& clickInfo, sptr<IRemoteObject> callerToken, sptr<IRemoteObject> dialogCallback) = 0;
    virtual bool VerifySavePermission(AccessToken::AccessTokenID tokenId) = 0;
    virtual sptr<IRemoteObject> GetEnhanceRemoteObject() = 0;
    virtual int32_t PreRegisterSecCompProcess() = 0;
};
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
#endif  // I_SECURITY_COMPONENT_SERVICE_H
