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
#ifndef SECURITY_COMPONENT_MALICIOUS_APPS_H
#define SECURITY_COMPONENT_MALICIOUS_APPS_H

#include <mutex>
#include <set>

namespace OHOS {
namespace Security {
namespace SecurityComponent {
class SecCompMaliciousApps {
public:
    SecCompMaliciousApps() = default;
    virtual ~SecCompMaliciousApps() = default;
    bool IsInMaliciousAppList(int32_t pid, int32_t uid);
    void AddAppToMaliciousAppList(int32_t pid);
    void RemoveAppFromMaliciousAppList(int32_t pid);
    bool IsMaliciousAppListEmpty();

private:
    std::set<int32_t> maliciousAppList_; // pid set
    std::mutex maliciousMtx_;
};
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
#endif  // SECURITY_COMPONENT_MALICIOUS_APPS_H
