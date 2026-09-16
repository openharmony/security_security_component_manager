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
#ifndef SEC_COMP_BUNDLE_NAME_CACHE_H
#define SEC_COMP_BUNDLE_NAME_CACHE_H

#include <mutex>
#include <string>
#include <unordered_map>
#include "accesstoken_kit.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
class __attribute__((visibility("default"))) SecCompBundleNameCache {
public:
    static SecCompBundleNameCache& GetInstance();

    /*
     * Get bundle name by token id. Result is cached after the first successful
     * query, so only cache misses go to access token service by ipc.
     * Return empty string when the token is not a hap token or query fails.
     */
    std::string GetBundleName(AccessToken::AccessTokenID tokenId);

    /*
     * Clear cached bundle names. For test isolation and token invalidation.
     */
    void ClearCache();

private:
    SecCompBundleNameCache() = default;
    ~SecCompBundleNameCache() = default;

    std::mutex bundleNameMtx_;
    std::unordered_map<AccessToken::AccessTokenID, std::string> bundleNameMap_;
};
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
#endif  // SEC_COMP_BUNDLE_NAME_CACHE_H
