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
#ifndef SECURITY_COMPONENT_PROXY_H
#define SECURITY_COMPONENT_PROXY_H

#include <string>
#include "i_sec_comp_service.h"
#include "iremote_proxy.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
class SecCompProxy : public IRemoteProxy<ISecCompService> {
public:
    explicit SecCompProxy(const sptr<IRemoteObject>& impl);
    ~SecCompProxy() override;
    int32_t RegisterSecurityComponent(SecCompType type, const std::string& componentInfo, int32_t& scId) override;
    int32_t UpdateSecurityComponent(int32_t scId, const std::string& componentInfo) override;
    int32_t UnregisterSecurityComponent(int32_t scId) override;
    int32_t ReportSecurityComponentClickEvent(int32_t scId,
        const std::string& componentInfo, const SecCompClickEvent& clickInfo,
        sptr<IRemoteObject> callerToken, sptr<IRemoteObject> dialogCallback) override;
    bool VerifySavePermission(AccessToken::AccessTokenID tokenId) override;
    sptr<IRemoteObject> GetEnhanceRemoteObject() override;
    int32_t PreRegisterSecCompProcess() override;

private:
    int32_t SendReportClickEventRequest(MessageParcel& data);
    static inline BrokerDelegator<SecCompProxy> delegator_;
};
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
#endif  // SECURITY_COMPONENT_PROXY_H
