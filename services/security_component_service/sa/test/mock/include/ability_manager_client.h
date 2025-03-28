/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef OHOS_ABILITY_RUNTIME_ABILITY_MANAGER_CLIENT_H
#define OHOS_ABILITY_RUNTIME_ABILITY_MANAGER_CLIENT_H

#include <mutex>

#include "iremote_object.h"
#include "want.h"

namespace OHOS {
namespace AAFwk {
/**
 * @class AbilityManagerClient
 * AbilityManagerClient is used to access ability manager services.
 */
class AbilityManagerClient {
public:
    AbilityManagerClient() = default;
    virtual ~AbilityManagerClient() = default;
    static std::shared_ptr<AbilityManagerClient> GetInstance()
    {
        static std::shared_ptr<AbilityManagerClient> instance_ = std::make_shared<AbilityManagerClient>();
        return instance_;
    }

    ErrCode StartExtensionAbility(const Want &want, sptr<IRemoteObject> callerToken)
    {
        return 0;
    }
};
}  // namespace AAFwk
}  // namespace OHOS
#endif  // OHOS_ABILITY_RUNTIME_ABILITY_MANAGER_CLIENT_H