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
#ifndef SECURITY_COMPONENT_PERMISSION_MANAGER_H
#define SECURITY_COMPONENT_PERMISSION_MANAGER_H

#include <map>
#include <deque>
#include "accesstoken_kit.h"
#include "rwlock.h"
#include "sec_comp_base.h"
#include "sec_event_handler.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
class SecCompPermManager {
public:
    SecCompPermManager() = default;
    virtual ~SecCompPermManager() = default;
    static SecCompPermManager& GetInstance();

    int32_t GrantLocationPermission(AccessToken::AccessTokenID tokenId,
        const std::string& permissionName, int flag);
    int32_t RevokeLocationPermission(AccessToken::AccessTokenID tokenId,
        const std::string& permissionName, int flag);

    int32_t GrantTempSavePermission(AccessToken::AccessTokenID tokenId);
    int32_t RevokeTempSavePermission(AccessToken::AccessTokenID tokenId);

    bool InitEventHandler(const std::shared_ptr<SecEventHandler>& secHandler);
    std::shared_ptr<SecEventHandler> GetSecEventHandler() const;

private:
    bool DelayRevokePermission(AccessToken::AccessTokenID tokenId, const std::string& taskName);
    bool RevokeSavePermissionTask(const std::string& taskName);

    std::unordered_map<AccessToken::AccessTokenID, int32_t> applySaveCountMap_;
    std::deque<std::string> taskQue_;
    std::mutex mutex_;
    std::shared_ptr<SecEventHandler> secHandler_;
};
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
#endif  // SECURITY_COMPONENT_PERMISSION_MANAGER_H
