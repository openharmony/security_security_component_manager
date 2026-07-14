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
#include "sec_comp_malicious_apps.h"

#include "sec_comp_log.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
    LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "SecCompMaliciousApps"};
static constexpr int32_t ROOT_UID = 0;
static constexpr uint32_t MAX_CONTINUOUS_ENHANCE_FAIL_COUNT = 3;
}

bool SecCompMaliciousApps::IsInMaliciousAppList(int32_t pid, int32_t uid)
{
    std::lock_guard<std::mutex> lock(maliciousMtx_);
    if (uid == ROOT_UID) {
        return false;
    }
    return (maliciousAppList_.find(pid) != maliciousAppList_.end());
}

void SecCompMaliciousApps::AddAppToMaliciousAppList(int32_t pid)
{
    std::lock_guard<std::mutex> lock(maliciousMtx_);
    uint32_t& failCount = maliciousFailCountMap_[pid];
    failCount++;
    if (failCount > MAX_CONTINUOUS_ENHANCE_FAIL_COUNT) {
        maliciousAppList_.insert(pid);
        SC_LOG_WARN(LABEL, "Pid %{public}d entered malicious app list, failCount=%{public}u", pid, failCount);
        return;
    }
    SC_LOG_INFO(LABEL, "Pid %{public}d malicious failCount=%{public}u", pid, failCount);
}

void SecCompMaliciousApps::RemoveAppFromMaliciousAppList(int32_t pid)
{
    std::lock_guard<std::mutex> lock(maliciousMtx_);
    maliciousAppList_.erase(pid);
    maliciousFailCountMap_.erase(pid);
}

void SecCompMaliciousApps::ResetAppMaliciousFailCount(int32_t pid)
{
    std::lock_guard<std::mutex> lock(maliciousMtx_);
    maliciousFailCountMap_.erase(pid);
}

bool SecCompMaliciousApps::IsMaliciousAppListEmpty()
{
    std::lock_guard<std::mutex> lock(maliciousMtx_);
    return (maliciousAppList_.size() == 0);
}
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
