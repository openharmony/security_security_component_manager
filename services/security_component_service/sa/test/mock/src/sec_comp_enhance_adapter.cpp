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

#ifndef FUZZ_ENABLE
static bool CopyMessageParcel(MessageParcel& oldData, MessageParcel& newData)
{
    size_t bufferLength = oldData.GetDataSize();
    if (bufferLength == 0) {
        SC_LOG_INFO(LABEL, "TmpData is empty.");
        return true;
    }

    char* buffer = reinterpret_cast<char *>(oldData.GetData());
    if (buffer == nullptr) {
        SC_LOG_ERROR(LABEL, "Get tmpData data failed.");
        return false;
    }

    if (!newData.WriteBuffer(reinterpret_cast<void *>(buffer), bufferLength)) {
        SC_LOG_ERROR(LABEL, "Write rawData failed.");
        return false;
    }
    return true;
}

bool SecCompEnhanceAdapter::EnhanceClientSerialize(MessageParcel& input, MessageParcel& output)
{
    SC_LOG_DEBUG(LABEL, "EnhanceClientSerialize successful.");
    return CopyMessageParcel(input, output);
}

bool SecCompEnhanceAdapter::EnhanceClientDeserialize(MessageParcel& input, MessageParcel& output)
{
    SC_LOG_DEBUG(LABEL, "EnhanceClientDeserialize successful.");
    return CopyMessageParcel(input, output);
}

bool SecCompEnhanceAdapter::EnhanceSrvSerialize(MessageParcel& input, MessageParcel& output)
{
    SC_LOG_DEBUG(LABEL, "EnhanceSrvSerialize successful.");
    return CopyMessageParcel(input, output);
}

bool SecCompEnhanceAdapter::EnhanceSrvDeserialize(MessageParcel& input, MessageParcel& output,
    MessageParcel& reply)
{
    SC_LOG_DEBUG(LABEL, "EnhanceSrvDeserialize successful.");
    return CopyMessageParcel(input, output);
}
#else
bool WriteMessageParcel(MessageParcel& input, MessageParcel& output)
{
    size_t bufLen = input.GetDataSize();
    if (!output.WriteInt32(bufLen)) {
        SC_LOG_ERROR(LABEL, "Write buf len fail.");
        return false;
    }

    if (bufLen == 0) {
        SC_LOG_INFO(LABEL, "Input data empty.");
        return true;
    }

    char* buf = reinterpret_cast<char*>(input.GetData());
    if (buf == nullptr) {
        SC_LOG_ERROR(LABEL, "Get buf err.");
        return false;
    }

    if (!output.WriteRawData(reinterpret_cast<void*>(buf), bufLen)) {
        SC_LOG_ERROR(LABEL, "Write buf fail.");
        return false;
    }
    return true;
}

bool ReadMessageParcel(MessageParcel& input, MessageParcel& output)
{
    int32_t size;
    if (!input.ReadInt32(size)) {
        SC_LOG_ERROR(LABEL, "Read buf len fail.");
        return false;
    }
    if (size == 0) {
        SC_LOG_INFO(LABEL, "Read buf len empty.");
        return true;
    }

    const void* it = input.ReadRawData(size);
    if (it == nullptr) {
        SC_LOG_ERROR(LABEL, "Read buf fail.");
        return false;
    }
    char* ptr = reinterpret_cast<char*>(const_cast<void*>(it));
    if (!output.WriteBuffer(reinterpret_cast<void*>(ptr), size)) {
        SC_LOG_ERROR(LABEL, "Write output buf fail.");
        return false;
    }
    return true;
}

bool SecCompEnhanceAdapter::EnhanceClientSerialize(MessageParcel& input, MessageParcel& output)
{
    SC_LOG_DEBUG(LABEL, "EnhanceClientSerialize successful.");
    return WriteMessageParcel(input, output);
}

bool SecCompEnhanceAdapter::EnhanceClientDeserialize(MessageParcel& input, MessageParcel& output)
{
    SC_LOG_DEBUG(LABEL, "EnhanceClientDeserialize successful.");
    return ReadMessageParcel(input, output);
}

bool SecCompEnhanceAdapter::EnhanceSrvSerialize(MessageParcel& input, MessageParcel& output)
{
    SC_LOG_DEBUG(LABEL, "EnhanceSrvSerialize successful.");
    return WriteMessageParcel(input, output);
}

bool SecCompEnhanceAdapter::EnhanceSrvDeserialize(MessageParcel& input, MessageParcel& output,
    MessageParcel& reply)
{
    SC_LOG_DEBUG(LABEL, "EnhanceSrvDeserialize successful.");
    return ReadMessageParcel(input, output);
}
#endif // FUZZ_ENABLE

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

void SecCompEnhanceAdapter::ExitEnhanceService()
{
    SC_LOG_DEBUG(LABEL, "ExitEnhanceService success");
}

void SecCompEnhanceAdapter::NotifyProcessDied(int32_t pid)
{
    SC_LOG_DEBUG(LABEL, "NotifyProcessDied success");
}

int32_t SecCompEnhanceAdapter::CheckComponentInfoEnhance(int32_t pid,
    std::shared_ptr<SecCompBase>& compInfo, const nlohmann::json& jsonComponent)
{
    SC_LOG_DEBUG(LABEL, "CheckComponentInfoEnhance success");
    return SC_OK;
}

sptr<IRemoteObject> SecCompEnhanceAdapter::GetEnhanceRemoteObject()
{
    SC_LOG_DEBUG(LABEL, "GetEnhanceRemoteObject success");
    return nullptr;
}

void SecCompEnhanceAdapter::AddSecurityComponentProcess(int32_t pid)
{
    SC_LOG_DEBUG(LABEL, "AddSecurityComponentProcess success");
}
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
