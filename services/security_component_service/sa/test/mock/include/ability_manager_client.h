/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef OHOS_ABILITY_RUNTIME_ABILITY_MANAGER_CLIENT_MOCK_H
#define OHOS_ABILITY_RUNTIME_ABILITY_MANAGER_CLIENT_MOCK_H

#include <memory>
#include "iremote_object.h"
#include "want.h"

namespace OHOS {
namespace AAFwk {
/*
 * Minimal ability manager client mock for unit tests: records the last
 * started extension ability so tests can assert what the ui adapter impl
 * passes to the grant dialog or toast ability.
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

    int32_t StartExtensionAbility(const Want& want, sptr<IRemoteObject> callerToken, int32_t userId = -1)
    {
        lastUserId_ = userId;
        lastCallerToken_ = callerToken;
        return 0;
    }

    int32_t StartExtensionAbility(const Want& want, sptr<IRemoteObject> callerToken,
        int32_t userId, int32_t extensionAbilityType)
    {
        return StartExtensionAbility(want, callerToken, userId);
    }

    void ResetRecord()
    {
        lastUserId_ = -1;
        lastCallerToken_ = nullptr;
    }

    int32_t GetLastUserId() const
    {
        return lastUserId_;
    }

    sptr<IRemoteObject> GetLastCallerToken() const
    {
        return lastCallerToken_;
    }

private:
    int32_t lastUserId_ = -1;
    sptr<IRemoteObject> lastCallerToken_ = nullptr;
};
}  // namespace AAFwk
}  // namespace OHOS
#endif  // OHOS_ABILITY_RUNTIME_ABILITY_MANAGER_CLIENT_MOCK_H
