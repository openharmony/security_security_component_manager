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
static const int32_t DELAY_REVOKE_MILLISECONDS = 10 * 1000;
static const std::string REVOKE_TASK_PREFIX = "RevokeAll";
static const std::string REVOKE_SAVE_PERM_TASK_PREFIX = "RevokeSavePerm";
}

SecCompPermManager& SecCompPermManager::GetInstance()
{
    static SecCompPermManager instance;
    return instance;
}

bool SecCompPermManager::DelaySaveRevokePermission(AccessToken::AccessTokenID tokenId, const std::string& taskName)
{
    if (secHandler_ == nullptr) {
        SC_LOG_ERROR(LABEL, "fail to get EventHandler");
        return false;
    }

    std::function<void()> delayed = ([tokenId]() {
        SC_LOG_DEBUG(LABEL, "delay revoke save permission");
        SecCompPermManager::GetInstance().RevokeTempSavePermissionCount(tokenId);
    });

    SC_LOG_DEBUG(LABEL, "revoke save permission after %{public}d ms", DELAY_REVOKE_MILLISECONDS);
    secHandler_->ProxyPostTask(delayed, taskName, DELAY_REVOKE_MILLISECONDS);
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

int32_t SecCompPermManager::GrantTempSavePermission(AccessToken::AccessTokenID tokenId)
{
    auto current = static_cast<uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::string taskName = std::to_string(tokenId) + std::to_string(current);
    if (!DelaySaveRevokePermission(tokenId, taskName)) {
        return SC_SERVICE_ERROR_PERMISSION_OPER_FAIL;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    saveTaskDequeMap_[tokenId].push_back(taskName);
    applySaveCountMap_[tokenId]++;
    SC_LOG_DEBUG(LABEL, "tokenId: %{public}d current permission apply counts is: %{public}d.",
        tokenId, applySaveCountMap_[tokenId]);
    return SC_OK;
}

void SecCompPermManager::RevokeTempSavePermissionCount(AccessToken::AccessTokenID tokenId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = applySaveCountMap_.find(tokenId);
    if (iter == applySaveCountMap_.end()) {
        SC_LOG_ERROR(LABEL, "This hap has no permissions to save files.");
        return;
    }
    if (saveTaskDequeMap_[tokenId].size() == 0) {
        SC_LOG_ERROR(LABEL, "Current no task need to be revoke.");
        return;
    }
    std::string taskName = saveTaskDequeMap_[tokenId].front();
    if (!RevokeSavePermissionTask(taskName)) {
        return;
    }
    saveTaskDequeMap_[tokenId].pop_front();
    SC_LOG_DEBUG(LABEL, "tokenId: %{public}d current permission apply counts is: %{public}d.",
        tokenId, applySaveCountMap_[tokenId]);
    if ((--applySaveCountMap_[tokenId]) == 0) {
        applySaveCountMap_.erase(tokenId);
        SC_LOG_INFO(LABEL, "tokenId: %{public}d save permission count is 0, revoke it.", tokenId);
    }
    return;
}

void SecCompPermManager::RevokeTempSavePermission(AccessToken::AccessTokenID tokenId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    applySaveCountMap_.erase(tokenId);
    auto& taskDeque = saveTaskDequeMap_[tokenId];
    for (auto iter = taskDeque.begin(); iter != taskDeque.end(); ++iter) {
        if (!RevokeSavePermissionTask(*iter)) {
            continue;
        }
    }
    taskDeque.clear();
    SC_LOG_INFO(LABEL, "tokenId: %{public}d revoke save permission.", tokenId);
    return;
}

bool SecCompPermManager::VerifySavePermission(AccessToken::AccessTokenID tokenId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = applySaveCountMap_.find(tokenId);
    if (iter == applySaveCountMap_.end() || applySaveCountMap_[tokenId] == 0) {
        SC_LOG_ERROR(LABEL, "This hap has no permissions to save files.");
        return false;
    }
    return true;
}

bool SecCompPermManager::VerifyPermission(AccessToken::AccessTokenID tokenId, SecCompType type)
{
    int32_t res;
    switch (type) {
        case LOCATION_COMPONENT:
            res = AccessToken::AccessTokenKit::VerifyAccessToken(tokenId, "ohos.permission.LOCATION");
            if (res != AccessToken::TypePermissionState::PERMISSION_GRANTED) {
                return false;
            }
            res = AccessToken::AccessTokenKit::VerifyAccessToken(tokenId, "ohos.permission.APPROXIMATELY_LOCATION");
            return (res == AccessToken::TypePermissionState::PERMISSION_GRANTED);
        case PASTE_COMPONENT:
            res = AccessToken::AccessTokenKit::VerifyAccessToken(tokenId,
                "ohos.permission.SECURE_PASTE");
            return (res == AccessToken::TypePermissionState::PERMISSION_GRANTED);
        case SAVE_COMPONENT:
            return VerifySavePermission(tokenId);
        default:
            SC_LOG_ERROR(LABEL, "Unknown component type.");
    }
    return false;
}

void SecCompPermManager::AddAppGrantPermissionRecord(AccessToken::AccessTokenID tokenId,
    const std::string& permissionName)
{
    auto iter = grantMap_.find(tokenId);
    if (iter != grantMap_.end()) {
        iter->second.insert(permissionName);
        return;
    }
    std::set<std::string> permSet;
    permSet.insert(permissionName);
    grantMap_[tokenId] = permSet;
}

void SecCompPermManager::RemoveAppGrantPermissionRecord(AccessToken::AccessTokenID tokenId,
    const std::string& permissionName)
{
    auto iter = grantMap_.find(tokenId);
    if (iter == grantMap_.end()) {
        return;
    }

    grantMap_[tokenId].erase(permissionName);
}

int32_t SecCompPermManager::GrantAppPermission(AccessToken::AccessTokenID tokenId,
    const std::string& permissionName)
{
    std::lock_guard<std::mutex> lock(grantMtx_);
    int32_t res = AccessToken::AccessTokenKit::GrantPermission(tokenId, permissionName,
        AccessToken::PermissionFlag::PERMISSION_COMPONENT_SET);
    SC_LOG_INFO(LABEL, "grant permission res: %{public}d, permission: %{public}s, tokenId:%{public}d",
        res, permissionName.c_str(), tokenId);

    AddAppGrantPermissionRecord(tokenId, permissionName);
    return res;
}

int32_t SecCompPermManager::RevokeAppPermission(AccessToken::AccessTokenID tokenId,
    const std::string& permissionName)
{
    std::lock_guard<std::mutex> lock(grantMtx_);
    int32_t res = AccessToken::AccessTokenKit::RevokePermission(tokenId, permissionName,
        AccessToken::PermissionFlag::PERMISSION_COMPONENT_SET);
    SC_LOG_INFO(LABEL, "revoke permission res: %{public}d, permission: %{public}s, tokenId:%{public}d",
        res, permissionName.c_str(), tokenId);

    RemoveAppGrantPermissionRecord(tokenId, permissionName);
    return res;
}

void SecCompPermManager::RevokeAppPermissions(AccessToken::AccessTokenID tokenId)
{
    RevokeAppPermisionsImmediately(tokenId);
    CancelAppRevokingPermisions(tokenId);
}

void SecCompPermManager::RevokeAppPermisionsDelayed(AccessToken::AccessTokenID tokenId)
{
    if (secHandler_ == nullptr) {
        SC_LOG_ERROR(LABEL, "fail to get EventHandler");
        return;
    }

    std::function<void()> delayed = ([tokenId]() {
        SC_LOG_DEBUG(LABEL, "delay revoke token id %{public}d permissions", tokenId);
        SecCompPermManager::GetInstance().RevokeAppPermisionsImmediately(tokenId);
    });

    SC_LOG_DEBUG(LABEL, "revoke token id %{public}d permissions after %{public}d ms",
        tokenId, DELAY_REVOKE_MILLISECONDS);
    std::string taskName = REVOKE_TASK_PREFIX + std::to_string(tokenId);
    secHandler_->ProxyPostTask(delayed, taskName, DELAY_REVOKE_MILLISECONDS);
}

void SecCompPermManager::RevokeAppPermisionsImmediately(AccessToken::AccessTokenID tokenId)
{
    std::lock_guard<std::mutex> lock(grantMtx_);
    auto it = grantMap_.find(tokenId);
    if (it == grantMap_.end()) {
        return;
    }

    auto& grantSet = grantMap_[tokenId];
    for (auto iter = grantSet.begin(); iter != grantSet.end(); ++iter) {
        int32_t res = AccessToken::AccessTokenKit::RevokePermission(tokenId, *iter,
            AccessToken::PermissionFlag::PERMISSION_COMPONENT_SET);
        SC_LOG_INFO(LABEL, "revoke token id %{public}d permission %{public}s res %{public}d",
            tokenId, iter->c_str(), res);
    }
    grantSet.clear();
}

void SecCompPermManager::CancelAppRevokingPermisions(AccessToken::AccessTokenID tokenId)
{
    if (secHandler_ == nullptr) {
        SC_LOG_ERROR(LABEL, "fail to get EventHandler");
        return;
    }

    SC_LOG_DEBUG(LABEL, "cancel revoke token id %{public}d permission", tokenId);
    std::string taskName = REVOKE_TASK_PREFIX + std::to_string(tokenId);
    secHandler_->ProxyRemoveTask(taskName);
}

bool SecCompPermManager::InitEventHandler(const std::shared_ptr<SecEventHandler>& secHandler)
{
    secHandler_ = secHandler;
    return true;
}
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
