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
#include "sec_comp_perm_manager.h"

#include "sec_comp_err.h"
#include "sec_comp_log.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "SecCompPermManager"};
static const int32_t WAIT_MILLISECONDS = 5 * 1000;
}

SecCompPermManager& SecCompPermManager::GetInstance()
{
    static SecCompPermManager instance;
    return instance;
}

bool SecCompPermManager::DelayRevokePermission(AccessToken::AccessTokenID tokenId, const std::string& taskName)
{
    if (secHandler_ == nullptr) {
        SC_LOG_ERROR(LABEL, "fail to get EventHandler");
        return false;
    }

    std::function<void()> delayed = ([tokenId]() {
        SC_LOG_DEBUG(LABEL, "delay revoke save permission");
        SecCompPermManager::GetInstance().RevokeTempSavePermission(tokenId);
    });

    SC_LOG_DEBUG(LABEL, "revoke save permission after %{public}d ms", WAIT_MILLISECONDS);
    secHandler_->ProxyPostTask(delayed, taskName, WAIT_MILLISECONDS);
    return true;
}

bool SecCompPermManager::RevokeSavePermissionTask(const std::string& taskName)
{
    if (secHandler_ == nullptr) {
        SC_LOG_ERROR(LABEL, "fail to get EventHandler");
        return false;
    }

    SC_LOG_DEBUG(LABEL, "revoke save permission task name:%{public}s", taskName.c_str());
    secHandler_->ProxyRemoveTask(taskName);
    return true;
}

int32_t SecCompPermManager::GrantLocationPermission(AccessToken::AccessTokenID tokenId,
    const std::string& permissionName, int flag)
{
    int32_t res = AccessToken::AccessTokenKit::GrantPermission(tokenId, permissionName,
        AccessToken::PermissionFlag::PERMISSION_COMPONENT_SET);
    SC_LOG_INFO(LABEL, "grant permission res: %{public}d, permission: %{public}s, tokenId:%{public}d",
        res, permissionName.c_str(), tokenId);
    return res;
}

int32_t SecCompPermManager::RevokeLocationPermission(AccessToken::AccessTokenID tokenId,
    const std::string& permissionName, int flag)
{
    int32_t res =  AccessToken::AccessTokenKit::RevokePermission(tokenId, permissionName,
        AccessToken::PermissionFlag::PERMISSION_COMPONENT_SET);
    SC_LOG_INFO(LABEL, "revoke permission res: %{public}d, permission: %{public}s, tokenId:%{public}d",
        res, permissionName.c_str(), tokenId);
    return res;
}

int32_t SecCompPermManager::GrantTempSavePermission(AccessToken::AccessTokenID tokenId)
{
    auto current = static_cast<uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::string taskName = std::to_string(tokenId) + std::to_string(current);
    if (!DelayRevokePermission(tokenId, taskName)) {
        return SC_SERVICE_ERROR_PERMISSION_OPER_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    taskQue_.push_back(taskName);
    applySaveCountMap_[tokenId]++;
    SC_LOG_DEBUG(LABEL, "tokenId: %{public}d current permission apply counts is: %{public}d.",
        tokenId, applySaveCountMap_[tokenId]);
    return SC_OK;
}

int32_t SecCompPermManager::RevokeTempSavePermission(AccessToken::AccessTokenID tokenId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = applySaveCountMap_.find(tokenId);
    if (iter == applySaveCountMap_.end() || applySaveCountMap_[tokenId] == 0) {
        SC_LOG_ERROR(LABEL, "This hap has no permissions to save files.");
        return SC_SERVICE_ERROR_PERMISSION_OPER_FAIL;
    }
    std::string taskName = taskQue_.front();
    if (!RevokeSavePermissionTask(taskName)) {
        return false;
    }
    taskQue_.pop_front();
    SC_LOG_DEBUG(LABEL, "tokenId: %{public}d current permission apply counts is: %{public}d.",
        tokenId, applySaveCountMap_[tokenId]);
    if ((--applySaveCountMap_[tokenId]) == 0) {
        applySaveCountMap_.erase(tokenId);
    }
    return SC_OK;
}

bool SecCompPermManager::InitEventHandler(const std::shared_ptr<SecEventHandler>& secHandler)
{
    secHandler_ = secHandler;
    return true;
}
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
