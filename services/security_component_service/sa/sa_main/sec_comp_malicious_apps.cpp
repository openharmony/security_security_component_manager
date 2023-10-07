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

namespace OHOS {
namespace Security {
namespace SecurityComponent {
namespace {
static constexpr int32_t ROOT_UID = 0;
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
    maliciousAppList_.insert(pid);
}

void SecCompMaliciousApps::RemoveAppFromMaliciousAppList(int32_t pid)
{
    std::lock_guard<std::mutex> lock(maliciousMtx_);
    maliciousAppList_.erase(pid);
}

bool SecCompMaliciousApps::IsMaliciousAppListEmpty()
{
    std::lock_guard<std::mutex> lock(maliciousMtx_);
    return (maliciousAppList_.size() == 0);
}
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
