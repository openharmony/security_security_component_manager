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
#ifndef MOCK_SECURITY_COMPONENT_SYSTEM_ABILITY_H
#define MOCK_SECURITY_COMPONENT_SYSTEM_ABILITY_H
#ifndef FUZZ_ENABLE
#include <gmock/gmock.h>
#endif // FUZZ_ENABLE
#include "if_system_ability_manager.h"
#include "iremote_object.h"

namespace OHOS {
#define DECLEAR_SYSTEM_ABILITY(className)
#define SA_ID_SECURITY_COMPONENT_SERVICE 1000
#define REGISTER_SYSTEM_ABILITY_BY_ID(a, b, c)

class SystemAbility {
public:
    static bool MakeAndRegisterAbility(SystemAbility*)
    {
        return true;
    }

    virtual void OnStart() {};

    virtual void OnStop() {};
#ifndef FUZZ_ENABLE
    MOCK_METHOD1(Publish, bool(sptr<IRemoteObject>));
#else
    bool Publish(sptr<IRemoteObject> obj)
    {
        return true;
    }
#endif // FUZZ_ENABLE

    SystemAbility(bool runOnCreate = false) {};

    SystemAbility(const int32_t serviceId, bool runOnCreate = false) {};

    virtual ~SystemAbility() = default;
};
}  // namespace OHOS
#endif  // MOCK_SECURITY_COMPONENT_SYSTEM_ABILITY_H
