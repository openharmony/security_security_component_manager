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

#include <dlfcn.h>
#include <new>

#include "sec_comp_log.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
    LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "SecCompGrantAdapter"};
static const std::string CLICK_GRANT_LIB = "libsecurity_component_click_grant.z.so";
static constexpr uint32_t MAX_INIT_RETRY_TIMES = 3;
}

SecCompGrantInterface* SecCompGrantAdapter::grantHandler_ = nullptr;
std::atomic_bool SecCompGrantAdapter::grantHandlerReady_ = false;
std::mutex SecCompGrantAdapter::grantInitMutex_;

void SecCompGrantAdapter::InitGrantHandler()
{
    std::lock_guard<std::mutex> lock(grantInitMutex_);
    if (grantHandlerReady_.load(std::memory_order_relaxed)) {
        return;
    }
    for (uint32_t attempt = 1; attempt <= MAX_INIT_RETRY_TIMES; ++attempt) {
        void* handler = dlopen(CLICK_GRANT_LIB.c_str(), RTLD_LAZY);
        if (handler == nullptr) {
            SC_LOG_ERROR(LABEL, "dlopen %{public}s failed at attempt %{public}u, error %{public}s",
                CLICK_GRANT_LIB.c_str(), attempt, dlerror());
            continue;
        }
        using GrantAdapterFactory = SecCompGrantInterface* (*)(void);
        GrantAdapterFactory getGrantInstance =
            reinterpret_cast<GrantAdapterFactory>(dlsym(handler, "GetGrantAdapterInstance"));
        if (getGrantInstance == nullptr) {
            SC_LOG_ERROR(LABEL, "dlsym GetGrantAdapterInstance failed at attempt %{public}u", attempt);
            return;
        }
        SecCompGrantInterface* instance = getGrantInstance();
        if (instance == nullptr) {
            SC_LOG_ERROR(LABEL, "GetGrantAdapterInstance returns null at attempt %{public}u", attempt);
            return;
        }
        SC_LOG_DEBUG(LABEL, "Dlopen click grant adapter successful.");
        grantHandler_ = instance;
        grantHandlerReady_.store(true, std::memory_order_release);
        return;
    }
}

SecCompGrantInterface* SecCompGrantAdapter::GetGrantHandler()
{
    if (!grantHandlerReady_.load(std::memory_order_acquire)) {
        InitGrantHandler();
    }
    if (!grantHandlerReady_.load(std::memory_order_acquire)) {
        return nullptr;
    }
    return grantHandler_;
}

void SecCompGrantAdapter::InitGrantAdapter()
{
    InitGrantHandler();
}

bool SecCompGrantAdapter::GetDisplaySize(uint64_t displayId, int32_t crossAxisState,
    int32_t& width, int32_t& height, bool& isRoundScreen)
{
    SecCompGrantInterface* handler = GetGrantHandler();
    if (handler == nullptr) {
        SC_LOG_ERROR(LABEL, "Ui adapter is not ready");
        return false;
    }
    return handler->GetDisplaySize(displayId, crossAxisState, width, height, isRoundScreen);
}

bool SecCompGrantAdapter::GetFoldCreaseBottomY(int32_t& bottomY)
{
    SecCompGrantInterface* handler = GetGrantHandler();
    if (handler == nullptr) {
        SC_LOG_ERROR(LABEL, "Ui adapter is not ready");
        return false;
    }
    return handler->GetFoldCreaseBottomY(bottomY);
}

bool SecCompGrantAdapter::StartGrantAbility(const SecCompGrantParams& params,
    const sptr<IRemoteObject>& callerToken, const sptr<IRemoteObject>& srvCallback)
{
    SecCompGrantInterface* handler = GetGrantHandler();
    if (handler == nullptr) {
        SC_LOG_ERROR(LABEL, "Ui adapter is not ready");
        return false;
    }
    return handler->StartGrantAbility(params, callerToken, srvCallback);
}
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
