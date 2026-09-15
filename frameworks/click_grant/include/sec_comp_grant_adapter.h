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
#ifndef SEC_COMP_GRANT_ADAPTER_H
#define SEC_COMP_GRANT_ADAPTER_H

#include <atomic>
#include <cstdint>
#include <mutex>
#include "iremote_object.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
/*
 * Params for starting the grant dialog ability. Keep every field a plain
 * old data type: the ui adapter so must not leak want or display types into
 * the main security component libraries, otherwise those dependencies would
 * be linked back into the eager load closure.
 */
struct SecCompGrantParams {
    int32_t typeNum = 0;           // 0 location, 1 save
    int32_t callerUid = 0;
    int32_t userId = 0;
    int32_t windowId = 0;
    uint64_t displayId = 0;
    int32_t crossAxisState = 0;    // CrossAxisState value
};

class SecCompGrantInterface {
public:
    virtual bool GetDisplaySize(uint64_t displayId, int32_t crossAxisState,
        int32_t& width, int32_t& height, bool& isRoundScreen) = 0;
    virtual bool GetFoldCreaseBottomY(int32_t& bottomY) = 0;
    virtual bool StartGrantAbility(const SecCompGrantParams& params,
        const sptr<IRemoteObject>& callerToken, const sptr<IRemoteObject>& srvCallback) = 0;
    virtual ~SecCompGrantInterface() = default;
};

/*
 * Lazy adapter for ui related dependencies (window_manager dm_lite,
 * ability_base want and ability_runtime ability_manager). The implementation
 * lives in libsecurity_component_click_grant.z.so which is dlopened on first
 * use or by the warmup task posted after the service is published. Callers
 * never need to care about whether the so has been loaded: on load failure
 * every entry falls back to the same failure branch the direct call used.
 */
class SecCompGrantAdapter final {
public:
    static void InitGrantAdapter();

    static bool GetDisplaySize(uint64_t displayId, int32_t crossAxisState,
        int32_t& width, int32_t& height, bool& isRoundScreen);
    static bool GetFoldCreaseBottomY(int32_t& bottomY);
    static bool StartGrantAbility(const SecCompGrantParams& params,
        const sptr<IRemoteObject>& callerToken, const sptr<IRemoteObject>& srvCallback);

private:
    static SecCompGrantInterface* GetGrantHandler();
    static void InitGrantHandler();

    static SecCompGrantInterface* grantHandler_;
    static std::atomic_bool grantHandlerReady_;
    static std::mutex grantInitMutex_;
};
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
#endif  // SEC_COMP_GRANT_ADAPTER_H
