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

#ifndef OHOS_ABILITY_BASE_WANT_MOCK_H
#define OHOS_ABILITY_BASE_WANT_MOCK_H

#include <map>
#include <string>
#include "iremote_object.h"

namespace OHOS {
namespace AAFwk {
/*
 * Minimal want mock for unit tests: records element name and params so tests
 * can assert what the ui adapter impl passes to the grant ability.
 */
class Want {
public:
    Want() = default;
    ~Want() = default;

    void SetElementName(const std::string& bundleName, const std::string& abilityName)
    {
        elementBundleName_ = bundleName;
        elementAbilityName_ = abilityName;
    }

    void SetParam(const std::string& key, int32_t value)
    {
        intParams_[key] = value;
    }

    void SetParam(const std::string& key, const sptr<IRemoteObject>& value)
    {
        remoteParams_[key] = value;
    }

    std::string GetElementBundleName() const
    {
        return elementBundleName_;
    }

    std::string GetElementAbilityName() const
    {
        return elementAbilityName_;
    }

    bool GetIntParam(const std::string& key, int32_t& value) const
    {
        auto iter = intParams_.find(key);
        if (iter == intParams_.end()) {
            return false;
        }
        value = iter->second;
        return true;
    }

    sptr<IRemoteObject> GetRemoteParam(const std::string& key) const
    {
        auto iter = remoteParams_.find(key);
        if (iter == remoteParams_.end()) {
            return nullptr;
        }
        return iter->second;
    }

private:
    std::string elementBundleName_;
    std::string elementAbilityName_;
    std::map<std::string, int32_t> intParams_;
    std::map<std::string, sptr<IRemoteObject>> remoteParams_;
};
}  // namespace AAFwk
}  // namespace OHOS
#endif  // OHOS_ABILITY_BASE_WANT_MOCK_H
