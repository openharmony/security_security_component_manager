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
#include "sec_comp_enhance_adapter.h"

#include "sec_comp_err.h"
#include "sec_comp_log.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
    LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "MockSecCompEnhanceAdapter"};
}

int32_t SecCompEnhanceAdapter::SetEnhanceCfg(uint8_t* cfg, uint32_t cfgLen)
{
    SC_LOG_DEBUG(LABEL, "SetEnhanceCfg success");
    return SC_OK;
}

int32_t SecCompEnhanceAdapter::GetPointerEventEnhanceData(void* data, uint32_t dataLen,
    uint8_t* enhanceData, uint32_t& enHancedataLen)
{
    SC_LOG_DEBUG(LABEL, "GetPointerEventEnhanceData success");
    return SC_OK;
}

int32_t SecCompEnhanceAdapter::CheckExtraInfo(const SecCompClickEvent& clickInfo)
{
    SC_LOG_DEBUG(LABEL, "CheckExtraInfo success");
    return SC_OK;
}

int32_t SecCompEnhanceAdapter::EnableInputEnhance()
{
    SC_LOG_DEBUG(LABEL, "EnableInputEnhance success");
    return SC_OK;
}

int32_t SecCompEnhanceAdapter::DisableInputEnhance()
{
    SC_LOG_DEBUG(LABEL, "DisableInputEnhance success");
    return SC_OK;
}

bool SecCompEnhanceAdapter::EnhanceDataPreprocess(std::string& componentInfo)
{
    SC_LOG_DEBUG(LABEL, "EnhanceDataPreprocess success");
    return true;
}

bool SecCompEnhanceAdapter::EnhanceDataPreprocess(int32_t scId, std::string& componentInfo)
{
    SC_LOG_DEBUG(LABEL, "EnhanceDataPreprocess success");
    return true;
}

void SecCompEnhanceAdapter::RegisterScIdEnhance(int32_t scId)
{
    SC_LOG_DEBUG(LABEL, "RegisterScIdEnhance success");
}

void SecCompEnhanceAdapter::UnregisterScIdEnhance(int32_t scId)
{
    SC_LOG_DEBUG(LABEL, "UnregisterScIdEnhance success");
}

void SecCompEnhanceAdapter::StartEnhanceService()
{
    SC_LOG_DEBUG(LABEL, "StartEnhanceService success");
}

void SecCompEnhanceAdapter::ExistEnhanceService()
{
    SC_LOG_DEBUG(LABEL, "ExistEnhanceService success");
}

void SecCompEnhanceAdapter::NotifyProcessDied(int32_t pid)
{
    SC_LOG_DEBUG(LABEL, "NotifyProcessDied success");
}

int32_t SecCompEnhanceAdapter::CheckComponentInfoEnhnace(int32_t pid,
    std::shared_ptr<SecCompBase>& compInfo, const nlohmann::json& jsonComponent)
{
    SC_LOG_DEBUG(LABEL, "CheckComponentInfoEnhnace success");
    return SC_OK;
}

sptr<IRemoteObject> SecCompEnhanceAdapter::GetEnhanceRemoteObject()
{
    SC_LOG_DEBUG(LABEL, "GetEnhanceRemoteObject success");
    return nullptr;
}
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
