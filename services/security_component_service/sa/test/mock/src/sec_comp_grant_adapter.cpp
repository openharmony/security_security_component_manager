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
#include "sec_comp_grant_adapter.h"

#include <atomic>
#include "sec_comp_log.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
    LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "MockSecCompGrantAdapter"};
static constexpr int32_t DEFAULT_SCREEN_SIZE = 1500;
bool g_alwaysFail = false;
int32_t g_lastUserId = -1;
std::atomic<uint32_t> g_initGrantAdapterCalls { 0 };
}

SecCompGrantInterface* SecCompGrantAdapter::grantHandler_ = nullptr;
std::atomic_bool SecCompGrantAdapter::grantHandlerReady_ = false;
std::mutex SecCompGrantAdapter::grantInitMutex_;

void SecCompGrantAdapter::InitGrantHandler()
{
    SC_LOG_DEBUG(LABEL, "Mock InitGrantHandler");
}

SecCompGrantInterface* SecCompGrantAdapter::GetGrantHandler()
{
    return nullptr;
}

void SecCompGrantAdapter::InitGrantAdapter()
{
    g_initGrantAdapterCalls.fetch_add(1, std::memory_order_relaxed);
    SC_LOG_DEBUG(LABEL, "Mock InitGrantAdapter");
}

bool SecCompGrantAdapter::GetDisplaySize(uint64_t displayId, int32_t crossAxisState,
    int32_t& width, int32_t& height, bool& isRoundScreen)
{
    (void)displayId;
    (void)crossAxisState;
    if (g_alwaysFail) {
        width = 0;
        height = 0;
        isRoundScreen = false;
        return false;
    }
    width = DEFAULT_SCREEN_SIZE;
    height = DEFAULT_SCREEN_SIZE;
    isRoundScreen = false;
    return true;
}

bool SecCompGrantAdapter::GetFoldCreaseBottomY(int32_t& bottomY)
{
    if (g_alwaysFail) {
        bottomY = 0;
        return false;
    }
    bottomY = 0;
    return true;
}

bool SecCompGrantAdapter::StartGrantAbility(const SecCompGrantParams& params,
    const sptr<IRemoteObject>& callerToken, const sptr<IRemoteObject>& srvCallback)
{
    (void)callerToken;
    (void)srvCallback;
    if (g_alwaysFail) {
        return false;
    }
    g_lastUserId = params.userId;
    return true;
}

int32_t GetGrantAdapterLastUserId()
{
    return g_lastUserId;
}

void ResetGrantAdapterLastUserId()
{
    g_lastUserId = -1;
}

void SetGrantAdapterAlwaysFail(bool alwaysFail)
{
    g_alwaysFail = alwaysFail;
}

bool GetGrantAdapterAlwaysFail()
{
    return g_alwaysFail;
}

void ResetGrantAdapterCallState()
{
    g_alwaysFail = false;
    g_lastUserId = -1;
    g_initGrantAdapterCalls.store(0, std::memory_order_relaxed);
}

uint32_t GetGrantAdapterInitCallCount()
{
    return g_initGrantAdapterCalls.load(std::memory_order_relaxed);
}
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
