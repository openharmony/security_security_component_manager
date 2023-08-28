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
#ifndef SECURITY_COMPONENT_MOCK_ABILITY_RUNTIME_APP_MGR_INTERFACE_H
#define SECURITY_COMPONENT_MOCK_ABILITY_RUNTIME_APP_MGR_INTERFACE_H

#include "iremote_broker.h"
#include "iremote_object.h"
#include "application_state_observer_stub.h"

namespace OHOS {
namespace AppExecFwk {
class IAppMgr : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.appexecfwk.mock.AppMgr");

    virtual int32_t RegisterApplicationStateObserver(const sptr<IApplicationStateObserver> &observer,
        const std::vector<std::string> &bundleNameList = {}) = 0;

    virtual int32_t UnregisterApplicationStateObserver(const sptr<IApplicationStateObserver> &observer) = 0;

    virtual int32_t GetForegroundApplications(std::vector<AppStateData> &list) = 0;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // SECURITY_COMPONENT_MOCK_ABILITY_RUNTIME_APP_MGR_INTERFACE_H
