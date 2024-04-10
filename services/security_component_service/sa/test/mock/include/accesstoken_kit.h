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
#ifndef SECURITY_COMPONENT_INTERFACES_INNER_KITS_ACCESSTOKEN_KIT_H
#define SECURITY_COMPONENT_INTERFACES_INNER_KITS_ACCESSTOKEN_KIT_H

#include <map>
#include <mutex>
#include <set>
#include <string>
#include "access_token.h"

namespace OHOS {
namespace Security {
namespace AccessToken {
struct HapTokenInfo {
};

typedef enum TypePermissionState {
    PERMISSION_DENIED = -1,
    PERMISSION_GRANTED = 0,
} PermissionState;

class AccessTokenKit {
public:
    static int RevokePermission(AccessTokenID tokenID, const std::string& permissionName, int flag);

    static int GrantPermission(AccessTokenID tokenID, const std::string& permissionName, int flag);

    static int VerifyAccessToken(AccessTokenID tokenID, const std::string& permissionName);

    static int GetHapTokenInfo(AccessTokenID tokenID, HapTokenInfo& hapTokenInfoRes)
    {
        return AccessTokenKit::getHapTokenInfoRes;
    };

    static AccessTokenID GetHapTokenID(int32_t userID, const std::string& bundleName, int32_t instIndex)
    {
        return 0;
    };

    static ATokenTypeEnum GetTokenTypeFlag(AccessTokenID tokenID)
    {
        AccessTokenIDInner *idInner = reinterpret_cast<AccessTokenIDInner *>(&tokenID);
        return static_cast<ATokenTypeEnum>(idInner->type);
    };

    static std::mutex mutex_;
    static std::map<AccessTokenID, std::set<std::string>> permMap_;
    static int getHapTokenInfoRes;
};
} // namespace SECURITY_COMPONENT_INTERFACES_INNER_KITS_ACCESSTOKEN_KIT_H
} // namespace Security
} // namespace OHOS
#endif
