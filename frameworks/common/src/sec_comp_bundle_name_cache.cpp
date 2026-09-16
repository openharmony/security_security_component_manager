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
#include "sec_comp_bundle_name_cache.h"

#include "sec_comp_log.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
    LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "SecCompBundleNameCache"};
}

SecCompBundleNameCache& SecCompBundleNameCache::GetInstance()
{
    static SecCompBundleNameCache* instance = nullptr;
    if (instance == nullptr) {
        static std::mutex instanceMutex;
        std::lock_guard<std::mutex> lock(instanceMutex);
        if (instance == nullptr) {
            instance = new (std::nothrow) SecCompBundleNameCache();
        }
    }
    return *instance;
}

void SecCompBundleNameCache::ClearCache()
{
    std::lock_guard<std::mutex> lock(bundleNameMtx_);
    bundleNameMap_.clear();
}

std::string SecCompBundleNameCache::GetBundleName(AccessToken::AccessTokenID tokenId)
{
    if (tokenId == 0) {
        return "";
    }
    {
        std::lock_guard<std::mutex> lock(bundleNameMtx_);
        auto iter = bundleNameMap_.find(tokenId);
        if (iter != bundleNameMap_.end()) {
            return iter->second;
        }
    }
    AccessToken::HapTokenInfo tokenInfo;
    if (AccessToken::AccessTokenKit::GetHapTokenInfo(tokenId, tokenInfo) != AccessToken::RET_SUCCESS) {
        SC_LOG_DEBUG(LABEL, "Get hap token info failed, tokenId %{public}u", tokenId);
        return "";
    }
    {
        std::lock_guard<std::mutex> lock(bundleNameMtx_);
        bundleNameMap_[tokenId] = tokenInfo.bundleName;
    }
    return tokenInfo.bundleName;
}
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
