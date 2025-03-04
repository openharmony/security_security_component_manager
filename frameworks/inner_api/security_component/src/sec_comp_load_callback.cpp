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
#include "sec_comp_load_callback.h"

#include "i_sec_comp_service.h"
#include "sec_comp_client.h"
#include "sec_comp_log.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "SecCompClient"};
}  // namespace
SecCompLoadCallback::SecCompLoadCallback() {}

void SecCompLoadCallback::OnLoadSystemAbilitySuccess(
    int32_t systemAbilityId, const sptr<IRemoteObject>& remoteObject)
{
    if (systemAbilityId != SA_ID_SECURITY_COMPONENT_SERVICE) {
        SC_LOG_ERROR(LABEL, "start systemabilityId is not security_component!");
        return;
    }

    if (remoteObject == nullptr) {
        SC_LOG_ERROR(LABEL, "remoteObject is null.");
        SecCompClient::GetInstance().FinishStartSAFail();
        return;
    }

    SC_LOG_INFO(LABEL, "Start systemAbilityId: %{public}d success!", systemAbilityId);

    SecCompClient::GetInstance().FinishStartSASuccess(remoteObject);
}

void SecCompLoadCallback::OnLoadSystemAbilityFail(int32_t systemAbilityId)
{
    if (systemAbilityId != SA_ID_SECURITY_COMPONENT_SERVICE) {
        SC_LOG_ERROR(LABEL, "start systemabilityId is not security_component!");
        return;
    }

    SC_LOG_ERROR(LABEL, "Start systemAbilityId: %{public}d failed.", systemAbilityId);

    SecCompClient::GetInstance().FinishStartSAFail();
}
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
