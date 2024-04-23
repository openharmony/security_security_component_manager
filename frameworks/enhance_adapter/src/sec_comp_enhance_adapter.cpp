/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include <dlfcn.h>
#include <sys/types.h>

#include "ipc_skeleton.h"
#include "parcel.h"
#include "sec_comp_err.h"
#include "sec_comp_log.h"
#include "securec.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
    LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "SecCompEnhanceAdapter"};

#if defined(__LP64__)
static const std::string LIB_PATH = "/system/lib64/";
#else
static const std::string LIB_PATH = "/system/lib/";
#endif
static const std::string INNER_PATH = "platformsdk/";
static const std::string ENHANCE_INPUT_INTERFACE_LIB =
    LIB_PATH + INNER_PATH + "libsecurity_component_client_enhance.z.so";
static const std::string ENHANCE_SRV_INTERFACE_LIB = LIB_PATH + "libsecurity_component_service_enhance.z.so";
static const std::string ENHANCE_CLIENT_INTERFACE_LIB =
    LIB_PATH + INNER_PATH + "libsecurity_component_client_enhance.z.so";
}

SecCompInputEnhanceInterface* SecCompEnhanceAdapter::inputHandler = nullptr;
bool SecCompEnhanceAdapter::isEnhanceInputHandlerInit = false;

SecCompSrvEnhanceInterface* SecCompEnhanceAdapter::srvHandler = nullptr;
bool SecCompEnhanceAdapter::isEnhanceSrvHandlerInit = false;

SecCompClientEnhanceInterface* SecCompEnhanceAdapter::clientHandler = nullptr;
bool SecCompEnhanceAdapter::isEnhanceClientHandlerInit = false;

std::mutex SecCompEnhanceAdapter::initMtx;

void SecCompEnhanceAdapter::InitEnhanceHandler(EnhanceInterfaceType type)
{
    std::unique_lock<std::mutex> lck(initMtx);
    std::string libPath = "";
    switch (type) {
        case SEC_COMP_ENHANCE_INPUT_INTERFACE:
            libPath = ENHANCE_INPUT_INTERFACE_LIB;
            isEnhanceInputHandlerInit = true;
            break;
        case SEC_COMP_ENHANCE_SRV_INTERFACE:
            libPath = ENHANCE_SRV_INTERFACE_LIB;
            isEnhanceSrvHandlerInit = true;
            break;
        case SEC_COMP_ENHANCE_CLIENT_INTERFACE:
            libPath = ENHANCE_CLIENT_INTERFACE_LIB;
            isEnhanceClientHandlerInit = true;
            break;
        default:
            break;
    }

    void* handler = dlopen(libPath.c_str(), RTLD_LAZY);
    if (handler == nullptr) {
        SC_LOG_ERROR(LABEL, "init enhance lib %{public}s failed, error %{public}s", libPath.c_str(), dlerror());
        return;
    }
    if (type == SEC_COMP_ENHANCE_CLIENT_INTERFACE) {
        EnhanceInterface getClientInstance = reinterpret_cast<EnhanceInterface>(dlsym(handler, "GetClientInstance"));
        if (getClientInstance == nullptr) {
            SC_LOG_ERROR(LABEL, "GetClientInstance failed.");
            return;
        }
        SecCompClientEnhanceInterface* instance = getClientInstance();
        if (instance != nullptr) {
            SC_LOG_DEBUG(LABEL, "Dlopen client enhance successful.");
            clientHandler = instance;
        }
    }
}

int32_t SecCompEnhanceAdapter::SetEnhanceCfg(uint8_t* cfg, uint32_t cfgLen)
{
    if (!isEnhanceInputHandlerInit) {
        InitEnhanceHandler(SEC_COMP_ENHANCE_INPUT_INTERFACE);
    }
    if (inputHandler != nullptr) {
        return inputHandler->SetEnhanceCfg(cfg, cfgLen);
    }
    return SC_ENHANCE_ERROR_NOT_EXIST_ENHANCE;
}

int32_t SecCompEnhanceAdapter::GetPointerEventEnhanceData(void* data, uint32_t dataLen,
    uint8_t* enhanceData, uint32_t& enHancedataLen)
{
    if (!isEnhanceInputHandlerInit) {
        InitEnhanceHandler(SEC_COMP_ENHANCE_INPUT_INTERFACE);
    }
    if (inputHandler != nullptr) {
        return inputHandler->GetPointerEventEnhanceData(data, dataLen, enhanceData, enHancedataLen);
    }
    return SC_ENHANCE_ERROR_NOT_EXIST_ENHANCE;
}

int32_t SecCompEnhanceAdapter::CheckExtraInfo(const SecCompClickEvent& clickInfo)
{
    if (!isEnhanceSrvHandlerInit) {
        InitEnhanceHandler(SEC_COMP_ENHANCE_SRV_INTERFACE);
    }
    if (srvHandler != nullptr) {
        if (clickInfo.extraInfo.dataSize == 0 || clickInfo.extraInfo.data == nullptr) {
            SC_LOG_ERROR(LABEL, "HMAC info is invalid");
            return SC_SERVICE_ERROR_CLICK_EVENT_INVALID;
        }
        return srvHandler->CheckExtraInfo(clickInfo);
    }
    return SC_ENHANCE_ERROR_NOT_EXIST_ENHANCE;
}

void SecCompEnhanceAdapter::AddSecurityComponentProcess(int32_t pid)
{
    if (!isEnhanceSrvHandlerInit) {
        InitEnhanceHandler(SEC_COMP_ENHANCE_SRV_INTERFACE);
    }
    if (srvHandler != nullptr) {
        srvHandler->AddSecurityComponentProcess(pid);
    }
}

bool SecCompEnhanceAdapter::EnhanceDataPreprocess(std::string& componentInfo)
{
    if (!isEnhanceClientHandlerInit) {
        InitEnhanceHandler(SEC_COMP_ENHANCE_CLIENT_INTERFACE);
    }

    uintptr_t enhanceCallerAddr = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
    if (clientHandler != nullptr) {
        return clientHandler->EnhanceDataPreprocess(enhanceCallerAddr, componentInfo);
    }
    return true;
}

bool SecCompEnhanceAdapter::EnhanceDataPreprocess(int32_t scId, std::string& componentInfo)
{
    if (!isEnhanceClientHandlerInit) {
        InitEnhanceHandler(SEC_COMP_ENHANCE_CLIENT_INTERFACE);
    }

    uintptr_t enhanceCallerAddr = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
    if (clientHandler != nullptr) {
        return clientHandler->EnhanceDataPreprocess(enhanceCallerAddr, scId, componentInfo);
    }
    return true;
}

static bool WriteMessageParcel(MessageParcel& tmpData, MessageParcel& data)
{
    size_t bufferLength = tmpData.GetDataSize();
    if (bufferLength == 0) {
        SC_LOG_INFO(LABEL, "TmpData is empty.");
        return true;
    }

    char* buffer = reinterpret_cast<char *>(tmpData.GetData());
    if (buffer == nullptr) {
        SC_LOG_ERROR(LABEL, "Get tmpData data failed.");
        return false;
    }

    if (!data.WriteInt32(bufferLength)) {
        SC_LOG_ERROR(LABEL, "Write bufferLength failed.");
        return false;
    }

    if (!data.WriteRawData(reinterpret_cast<void *>(buffer), bufferLength)) {
        SC_LOG_ERROR(LABEL, "Write data failed.");
        return false;
    }
    return true;
}

static bool ReadMessageParcel(MessageParcel& tmpData, MessageParcel& data)
{
    int32_t size;
    if (!tmpData.ReadInt32(size)) {
        SC_LOG_ERROR(LABEL, "Read size failed.");
        return false;
    }

    const void *iter = tmpData.ReadRawData(size);
    if (iter == nullptr) {
        SC_LOG_ERROR(LABEL, "Read const void failed.");
        return false;
    }
    char* ptr = reinterpret_cast<char *>(const_cast<void *>(iter));

    if (!data.WriteBuffer(reinterpret_cast<void *>(ptr), size)) {
        SC_LOG_ERROR(LABEL, "Write rawData failed.");
        return false;
    }
    return true;
}

bool SecCompEnhanceAdapter::EnhanceSerializeSessionInfo(MessageParcel& tmpData, MessageParcel& data)
{
    if (!isEnhanceClientHandlerInit) {
        InitEnhanceHandler(SEC_COMP_ENHANCE_CLIENT_INTERFACE);
    }

    uintptr_t enhanceCallerAddr = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
    if (clientHandler != nullptr) {
        return clientHandler->EnhanceSerializeSessionInfo(enhanceCallerAddr, tmpData, data);
    }

    return WriteMessageParcel(tmpData, data);
}

bool SecCompEnhanceAdapter::EnhanceDeserializeSessionInfo(MessageParcel& oldData, MessageParcel& newData)
{
    if (!isEnhanceClientHandlerInit) {
        InitEnhanceHandler(SEC_COMP_ENHANCE_CLIENT_INTERFACE);
    }

    uintptr_t enhanceCallerAddr = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
    if (clientHandler != nullptr) {
        return clientHandler->EnhanceDeserializeSessionInfo(enhanceCallerAddr, oldData, newData);
    }

    return ReadMessageParcel(oldData, newData);
}

bool SecCompEnhanceAdapter::SerializeSessionInfoEnhance(MessageParcel& tmpReply, MessageParcel& reply, int32_t pid)
{
    if (!isEnhanceSrvHandlerInit) {
        InitEnhanceHandler(SEC_COMP_ENHANCE_SRV_INTERFACE);
    }
    if (srvHandler != nullptr) {
        return srvHandler->SerializeSessionInfoEnhance(tmpReply, reply, pid);
    }

    return WriteMessageParcel(tmpReply, reply);
}

bool SecCompEnhanceAdapter::DeserializeSessionInfoEnhance(MessageParcel& oldData, MessageParcel& newData,
    MessageParcel& reply, int32_t pid)
{
    if (!isEnhanceSrvHandlerInit) {
        InitEnhanceHandler(SEC_COMP_ENHANCE_SRV_INTERFACE);
    }
    if (srvHandler != nullptr) {
        return srvHandler->DeserializeSessionInfoEnhance(oldData, newData, reply, pid);
    }

    return ReadMessageParcel(oldData, newData);
}

void SecCompEnhanceAdapter::RegisterScIdEnhance(int32_t scId)
{
    if (!isEnhanceClientHandlerInit) {
        InitEnhanceHandler(SEC_COMP_ENHANCE_CLIENT_INTERFACE);
    }

    uintptr_t enhanceCallerAddr = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
    if (clientHandler != nullptr) {
        clientHandler->RegisterScIdEnhance(enhanceCallerAddr, scId);
    }
}

void SecCompEnhanceAdapter::UnregisterScIdEnhance(int32_t scId)
{
    if (!isEnhanceClientHandlerInit) {
        InitEnhanceHandler(SEC_COMP_ENHANCE_CLIENT_INTERFACE);
    }

    uintptr_t enhanceCallerAddr = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
    if (clientHandler != nullptr) {
        clientHandler->UnregisterScIdEnhance(enhanceCallerAddr, scId);
    }
}

int32_t SecCompEnhanceAdapter::EnableInputEnhance()
{
    if (!isEnhanceSrvHandlerInit) {
        InitEnhanceHandler(SEC_COMP_ENHANCE_SRV_INTERFACE);
    }
    if (srvHandler != nullptr) {
        return srvHandler->EnableInputEnhance();
    }
    return SC_ENHANCE_ERROR_NOT_EXIST_ENHANCE;
}

int32_t SecCompEnhanceAdapter::DisableInputEnhance()
{
    if (!isEnhanceSrvHandlerInit) {
        InitEnhanceHandler(SEC_COMP_ENHANCE_SRV_INTERFACE);
    }
    if (srvHandler != nullptr) {
        return srvHandler->DisableInputEnhance();
    }
    return SC_ENHANCE_ERROR_NOT_EXIST_ENHANCE;
}

void SecCompEnhanceAdapter::StartEnhanceService()
{
    if (!isEnhanceSrvHandlerInit) {
        InitEnhanceHandler(SEC_COMP_ENHANCE_SRV_INTERFACE);
    }
    if (srvHandler != nullptr) {
        srvHandler->StartEnhanceService();
    }
}

void SecCompEnhanceAdapter::ExitEnhanceService()
{
    if (!isEnhanceSrvHandlerInit) {
        InitEnhanceHandler(SEC_COMP_ENHANCE_SRV_INTERFACE);
    }
    if (srvHandler != nullptr) {
        srvHandler->ExitEnhanceService();
    }
}

void SecCompEnhanceAdapter::NotifyProcessDied(int32_t pid)
{
    if (!isEnhanceSrvHandlerInit) {
        InitEnhanceHandler(SEC_COMP_ENHANCE_SRV_INTERFACE);
    }
    if (srvHandler != nullptr) {
        srvHandler->NotifyProcessDied(pid);
    }
}

int32_t SecCompEnhanceAdapter::CheckComponentInfoEnhance(int32_t pid,
    std::shared_ptr<SecCompBase>& compInfo, const nlohmann::json& jsonComponent)
{
    if (!isEnhanceSrvHandlerInit) {
        InitEnhanceHandler(SEC_COMP_ENHANCE_SRV_INTERFACE);
    }
    if (srvHandler != nullptr) {
        return srvHandler->CheckComponentInfoEnhance(pid, compInfo, jsonComponent);
    }
    return SC_OK;
}

sptr<IRemoteObject> SecCompEnhanceAdapter::GetEnhanceRemoteObject()
{
    if (!isEnhanceSrvHandlerInit) {
        InitEnhanceHandler(SEC_COMP_ENHANCE_SRV_INTERFACE);
    }
    if (srvHandler != nullptr) {
        auto service = srvHandler->GetEnhanceRemoteObject();
        return service;
    }
    return nullptr;
}
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
