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
#ifndef SECURITY_COMPONENT_MOCK_SYSTEM_ABILITY_MANAGER_PROXY
#define SECURITY_COMPONENT_MOCK_SYSTEM_ABILITY_MANAGER_PROXY
#include <string>
#ifndef FUZZ_ENABLE
#include <gmock/gmock.h>
#endif // FUZZ_ENABLE
#include "if_system_ability_manager.h"

namespace OHOS {
class SystemAbilityManagerProxy : public IRemoteProxy<ISystemAbilityManager> {
public:
    explicit SystemAbilityManagerProxy(const sptr<IRemoteObject>& impl)
        : IRemoteProxy<ISystemAbilityManager>(impl) {}
    ~SystemAbilityManagerProxy() = default;
#ifndef FUZZ_ENABLE
    MOCK_METHOD1(GetSystemAbility, sptr<MockIRemoteObject>(int32_t));
    MOCK_METHOD1(UnloadSystemAbility, int32_t(int32_t));
#else
    sptr<MockIRemoteObject> GetSystemAbility(int32_t saId)
    {
        sptr<MockIRemoteObject> object = new MockIRemoteObject();
        return object;
    }

    int32_t UnloadSystemAbility(int32_t saId)
    {
        return 0;
    }
#endif // FUZZ_ENABLE
};
} // namespace OHOS
#endif
