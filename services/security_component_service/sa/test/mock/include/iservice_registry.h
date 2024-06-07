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
#ifndef SEC_COMP_MOCK_SYSTEM_ABILITY_MANAGER_CLINET_INCLUDE_H
#define SEC_COMP_MOCK_SYSTEM_ABILITY_MANAGER_CLINET_INCLUDE_H
#ifndef FUZZ_ENABLE
#include <gmock/gmock.h>
#else
#include "mock_system_ability_proxy.h"
#endif // FUZZ_ENABLE
#include "if_system_ability_manager.h"

namespace OHOS {
#ifdef FUZZ_ENABLE
namespace {
static sptr<SystemAbilityManagerProxy> GLOBAL_SAM_PROXY =
    new SystemAbilityManagerProxy(nullptr);
}
#endif
class SystemAbilityManagerClient {
public:
    static SystemAbilityManagerClient& GetInstance();
    SystemAbilityManagerClient() = default;
    ~SystemAbilityManagerClient() = default;

    static SystemAbilityManagerClient* clientInstance;
    static SystemAbilityManagerClient defaultInstance;
    static std::mutex instanceMtx;
#ifndef FUZZ_ENABLE
    MOCK_METHOD0(GetSystemAbilityManager, sptr<ISystemAbilityManager>());
#else
    sptr<ISystemAbilityManager> GetSystemAbilityManager()
    {
        return GLOBAL_SAM_PROXY;
    }
#endif // FUZZ_ENABLE
};
}  // namespace OHOS
#endif  // SEC_COMP_MOCK_SYSTEM_ABILITY_MANAGER_CLINET_INCLUDE_H