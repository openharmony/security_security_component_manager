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

#include "accesstoken_kit.h"

#include "sec_comp_log.h"

namespace OHOS {
namespace Security {
namespace AccessToken {
int32_t AccessTokenKit::getHapTokenInfoRes = 0;
std::mutex AccessTokenKit::mutex_;
std::map<AccessTokenID, std::set<std::string>> AccessTokenKit::permMap_;

int AccessTokenKit::RevokePermission(AccessTokenID tokenID, const std::string& permissionName, int flag)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = permMap_.find(tokenID);
    if (iter == permMap_.end()) {
        return 0;
    }

    permMap_[tokenID].erase(permissionName);
    if (permMap_[tokenID].size() == 0) {
        permMap_.erase(tokenID);
    }
    return 0;
};

int AccessTokenKit::GrantPermission(AccessTokenID tokenID, const std::string& permissionName, int flag)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = permMap_.find(tokenID);
    if (iter != permMap_.end()) {
        iter->second.insert(permissionName);
        return 0;
    }
    std::set<std::string> permSet;
    permSet.insert(permissionName);
    permMap_[tokenID] = permSet;
    return 0;
};

int AccessTokenKit::VerifyAccessToken(AccessTokenID tokenID, const std::string& permissionName)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = permMap_.find(tokenID);
    if (iter == permMap_.end() || permMap_[tokenID].count(permissionName) < 1) {
        return -1;
    }
    return 0;
};
} // namespace SECURITY_COMPONENT_INTERFACES_INNER_KITS_ACCESSTOKEN_KIT_H
} // namespace Security
} // namespace OHOS
