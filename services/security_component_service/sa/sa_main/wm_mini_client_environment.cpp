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
#include "wm_mini_client_environment.h"

#include <new>

#include "iservice_registry.h"
#include "parameters.h"
#include "sec_comp_log.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "WMClientMini"};
constexpr char MULTI_INSTANCE_PARAMETER[] = "persist.dms.concurrentuser";
}

sptr<IRemoteObject> WmMiniClientEnvironment::GetWindowManagerServiceRemote()
{
    auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityManager == nullptr) {
        SC_LOG_ERROR(LABEL, "Get system ability manager failed");
        return nullptr;
    }

    sptr<IRemoteObject> wmsRemote = systemAbilityManager->GetSystemAbility(WINDOW_MANAGER_SERVICE_ID);
    if (wmsRemote == nullptr) {
        SC_LOG_ERROR(LABEL, "Get window manager service remote failed");
        return nullptr;
    }
    return wmsRemote;
}

bool WmMiniClientEnvironment::IsMultiInstanceEnabled()
{
    static const bool enabled = system::GetBoolParameter(MULTI_INSTANCE_PARAMETER, false);
    return enabled;
}

bool WmMiniClientEnvironment::WriteInterfaceToken(
    MessageParcel& parcel, const std::u16string& descriptor, WmMiniParcelField field)
{
    (void)field;
    return parcel.WriteInterfaceToken(descriptor);
}

bool WmMiniClientEnvironment::WriteInt32(MessageParcel& parcel, int32_t value, WmMiniParcelField field)
{
    (void)field;
    return parcel.WriteInt32(value);
}

Security::SecurityComponent::MiniAccessibilityWindowInfo* WmMiniClientEnvironment::CreateWindowInfo()
{
    return new (std::nothrow) Security::SecurityComponent::MiniAccessibilityWindowInfo();
}

Security::SecurityComponent::MiniUnreliableWindowInfo* WmMiniClientEnvironment::CreateUnreliableWindowInfo()
{
    return new (std::nothrow) Security::SecurityComponent::MiniUnreliableWindowInfo();
}

bool WmMiniClientEnvironment::IsParcelableVectorSizeValid(size_t size, size_t maxSize)
{
    return size <= maxSize;
}

bool WmMiniClientEnvironment::ResizeParcelableVector(
    std::vector<sptr<Security::SecurityComponent::MiniAccessibilityWindowInfo>>& infos, size_t size)
{
    infos.resize(size);
    return infos.size() >= size;
}

bool WmMiniClientEnvironment::ResizeParcelableVector(
    std::vector<sptr<Security::SecurityComponent::MiniUnreliableWindowInfo>>& infos, size_t size)
{
    infos.resize(size);
    return infos.size() >= size;
}
}  // namespace Rosen
}  // namespace OHOS
