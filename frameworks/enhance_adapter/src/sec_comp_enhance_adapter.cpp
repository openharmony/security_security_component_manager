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

#include <atomic>
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

static const std::string ENHANCE_INPUT_INTERFACE_LIB = "libsecurity_component_client_enhance.z.so";
static const std::string ENHANCE_SRV_INTERFACE_LIB = "libsecurity_component_service_enhance.z.so";
static const std::string ENHANCE_CLIENT_INTERFACE_LIB = "libsecurity_component_client_enhance.z.so";

std::atomic_bool g_inputHandlerReady = false;
std::atomic_bool g_srvHandlerReady = false;
std::atomic_bool g_clientHandlerReady = false;

struct EnhanceHandlerContext {
    const std::string* libPath = nullptr;
    std::atomic_bool* handlerReady = nullptr;
    bool* isHandlerInit = nullptr;
};

bool GetEnhanceHandlerContext(EnhanceInterfaceType type, EnhanceHandlerContext& context)
{
    switch (type) {
        case SEC_COMP_ENHANCE_INPUT_INTERFACE:
            context = { &ENHANCE_INPUT_INTERFACE_LIB, &g_inputHandlerReady,
                &SecCompEnhanceAdapter::isEnhanceInputHandlerInit };
            return true;
        case SEC_COMP_ENHANCE_SRV_INTERFACE:
            context = { &ENHANCE_SRV_INTERFACE_LIB, &g_srvHandlerReady,
                &SecCompEnhanceAdapter::isEnhanceSrvHandlerInit };
            return true;
        case SEC_COMP_ENHANCE_CLIENT_INTERFACE:
            context = { &ENHANCE_CLIENT_INTERFACE_LIB, &g_clientHandlerReady,
                &SecCompEnhanceAdapter::isEnhanceClientHandlerInit };
            return true;
        default:
            return false;
    }
}

SecCompInputEnhanceInterface* GetInputHandler()
{
    if (!g_inputHandlerReady.load(std::memory_order_acquire)) {
        SecCompEnhanceAdapter::InitEnhanceHandler(SEC_COMP_ENHANCE_INPUT_INTERFACE);
    }
    if (!g_inputHandlerReady.load(std::memory_order_acquire)) {
        return nullptr;
    }
    return SecCompEnhanceAdapter::inputHandler;
}

SecCompSrvEnhanceInterface* GetSrvHandler()
{
    if (!g_srvHandlerReady.load(std::memory_order_acquire)) {
        SecCompEnhanceAdapter::InitEnhanceHandler(SEC_COMP_ENHANCE_SRV_INTERFACE);
    }
    if (!g_srvHandlerReady.load(std::memory_order_acquire)) {
        return nullptr;
    }
    return SecCompEnhanceAdapter::srvHandler;
}

SecCompClientEnhanceInterface* GetClientHandler()
{
    if (!g_clientHandlerReady.load(std::memory_order_acquire)) {
        SecCompEnhanceAdapter::InitEnhanceHandler(SEC_COMP_ENHANCE_CLIENT_INTERFACE);
    }
    if (!g_clientHandlerReady.load(std::memory_order_acquire)) {
        return nullptr;
    }
    return SecCompEnhanceAdapter::clientHandler;
}
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
    EnhanceHandlerContext context;
    if (!GetEnhanceHandlerContext(type, context) || context.handlerReady->load(std::memory_order_acquire)) {
        return;
    }

    std::unique_lock<std::mutex> lck(initMtx);
    if (context.handlerReady->load(std::memory_order_relaxed)) {
        return;
    }

#ifdef SECURITY_COMPONENT_ENHANCE_DISABLE
    *context.isHandlerInit = true;
    context.handlerReady->store(true, std::memory_order_release);
    return;
#else
    void* handler = dlopen(context.libPath->c_str(), RTLD_LAZY);
    if (handler == nullptr) {
        SC_LOG_ERROR(LABEL, "init enhance lib %{public}s failed, error %{public}s",
            context.libPath->c_str(), dlerror());
        return;
    }
    if (type == SEC_COMP_ENHANCE_CLIENT_INTERFACE) {
        EnhanceInterface getClientInstance = reinterpret_cast<EnhanceInterface>(dlsym(handler, "GetClientInstance"));
        if (getClientInstance == nullptr) {
            SC_LOG_ERROR(LABEL, "GetClientInstance failed.");
            dlclose(handler);
            return;
        }
        SecCompClientEnhanceInterface* instance = getClientInstance();
        if (instance == nullptr) {
            dlclose(handler);
            return;
        }
        SC_LOG_DEBUG(LABEL, "Dlopen client enhance successful.");
        clientHandler = instance;
    }
    *context.isHandlerInit = true;
    context.handlerReady->store(true, std::memory_order_release);
#endif
}

int32_t SecCompEnhanceAdapter::SetEnhanceCfg(uint8_t* cfg, uint32_t cfgLen)
{
    SecCompInputEnhanceInterface* handler = GetInputHandler();
    if (handler != nullptr) {
        return handler->SetEnhanceCfg(cfg, cfgLen);
    }
    return SC_ENHANCE_ERROR_NOT_EXIST_ENHANCE;
}

int32_t SecCompEnhanceAdapter::GetPointerEventEnhanceData(void* data, uint32_t dataLen,
    uint8_t* enhanceData, uint32_t& enHancedataLen)
{
    SecCompInputEnhanceInterface* handler = GetInputHandler();
    if (handler != nullptr) {
        return handler->GetPointerEventEnhanceData(data, dataLen, enhanceData, enHancedataLen);
    }
    return SC_ENHANCE_ERROR_NOT_EXIST_ENHANCE;
}

int32_t SecCompEnhanceAdapter::CheckAndUpdateExtraInfo(SecCompClickEvent& clickInfo)
{
    SecCompSrvEnhanceInterface* handler = GetSrvHandler();
    if (handler != nullptr) {
        if (clickInfo.extraInfo.dataSize == 0 || clickInfo.extraInfo.data == nullptr) {
            SC_LOG_ERROR(LABEL, "HMAC info is invalid");
            return SC_SERVICE_ERROR_CLICK_EVENT_INVALID;
        }
        return handler->CheckAndUpdateExtraInfo(clickInfo);
    }
    return SC_ENHANCE_ERROR_NOT_EXIST_ENHANCE;
}

void SecCompEnhanceAdapter::AddSecurityComponentProcess(int32_t pid)
{
    SecCompSrvEnhanceInterface* handler = GetSrvHandler();
    if (handler != nullptr) {
        handler->AddSecurityComponentProcess(pid);
    }
}

bool SecCompEnhanceAdapter::IsBypassPermitted(const std::string& bundleName)
{
    SecCompSrvEnhanceInterface* handler = GetSrvHandler();
    if (handler != nullptr) {
        return handler->IsBypassPermitted(bundleName);
    }
    return false;
}

__attribute__((noinline)) bool SecCompEnhanceAdapter::EnhanceDataPreprocess(std::string& componentInfo)
{
    SecCompClientEnhanceInterface* handler = GetClientHandler();
    uintptr_t enhanceCallerAddr = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
    if (handler != nullptr) {
        return handler->EnhanceDataPreprocess(enhanceCallerAddr, componentInfo);
    }
    return true;
}

__attribute__((noinline)) bool SecCompEnhanceAdapter::EnhanceDataPreprocess(
    int32_t scId, std::string& componentInfo)
{
    SecCompClientEnhanceInterface* handler = GetClientHandler();
    uintptr_t enhanceCallerAddr = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
    if (handler != nullptr) {
        return handler->EnhanceDataPreprocess(enhanceCallerAddr, scId, componentInfo);
    }
    return true;
}

static bool WriteMessageParcel(MessageParcel& tmpData, SecCompRawdata& data)
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

    data.size = bufferLength;
    int32_t ret = data.RawDataCpy(reinterpret_cast<void *>(buffer));
    if (ret != SC_OK) {
        SC_LOG_ERROR(LABEL, "Copy tmpData to rawdata failed.");
        return false;
    }
    return true;
}

static bool ReadMessageParcel(SecCompRawdata& tmpData, MessageParcel& data)
{
    uint32_t size = tmpData.size;

    const void *iter = tmpData.data;
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

__attribute__((noinline)) bool SecCompEnhanceAdapter::EnhanceClientSerialize(
    MessageParcel& input, SecCompRawdata& output)
{
    SecCompClientEnhanceInterface* handler = GetClientHandler();
    uintptr_t enhanceCallerAddr = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
    if (handler != nullptr) {
        return handler->EnhanceClientSerialize(enhanceCallerAddr, input, output);
    }

    return WriteMessageParcel(input, output);
}

__attribute__((noinline)) bool SecCompEnhanceAdapter::EnhanceClientDeserialize(
    SecCompRawdata& input, MessageParcel& output)
{
    SecCompClientEnhanceInterface* handler = GetClientHandler();
    uintptr_t enhanceCallerAddr = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
    if (handler != nullptr) {
        return handler->EnhanceClientDeserialize(enhanceCallerAddr, input, output);
    }

    return ReadMessageParcel(input, output);
}

bool SecCompEnhanceAdapter::EnhanceSrvSerialize(MessageParcel& input, SecCompRawdata& output)
{
    SecCompSrvEnhanceInterface* handler = GetSrvHandler();
    if (handler != nullptr) {
        return handler->EnhanceSrvSerialize(input, output);
    }

    return WriteMessageParcel(input, output);
}

bool SecCompEnhanceAdapter::EnhanceSrvDeserialize(SecCompRawdata& input, MessageParcel& output)
{
    SecCompSrvEnhanceInterface* handler = GetSrvHandler();
    if (handler != nullptr) {
        return handler->EnhanceSrvDeserialize(input, output);
    }

    return ReadMessageParcel(input, output);
}

__attribute__((noinline)) void SecCompEnhanceAdapter::RegisterScIdEnhance(int32_t scId)
{
    SecCompClientEnhanceInterface* handler = GetClientHandler();
    uintptr_t enhanceCallerAddr = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
    if (handler != nullptr) {
        handler->RegisterScIdEnhance(enhanceCallerAddr, scId);
    }
}

__attribute__((noinline)) void SecCompEnhanceAdapter::UnregisterScIdEnhance(int32_t scId)
{
    SecCompClientEnhanceInterface* handler = GetClientHandler();
    uintptr_t enhanceCallerAddr = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
    if (handler != nullptr) {
        handler->UnregisterScIdEnhance(enhanceCallerAddr, scId);
    }
}

int32_t SecCompEnhanceAdapter::EnableInputEnhance()
{
    SecCompSrvEnhanceInterface* handler = GetSrvHandler();
    if (handler != nullptr) {
        return handler->EnableInputEnhance();
    }
    return SC_ENHANCE_ERROR_NOT_EXIST_ENHANCE;
}

int32_t SecCompEnhanceAdapter::DisableInputEnhance()
{
    SecCompSrvEnhanceInterface* handler = GetSrvHandler();
    if (handler != nullptr) {
        return handler->DisableInputEnhance();
    }
    return SC_ENHANCE_ERROR_NOT_EXIST_ENHANCE;
}

void SecCompEnhanceAdapter::StartEnhanceService()
{
    SecCompSrvEnhanceInterface* handler = GetSrvHandler();
    if (handler != nullptr) {
        handler->StartEnhanceService();
    }
}

void SecCompEnhanceAdapter::ExitEnhanceService()
{
    SecCompSrvEnhanceInterface* handler = GetSrvHandler();
    if (handler != nullptr) {
        handler->ExitEnhanceService();
    }
}

void SecCompEnhanceAdapter::NotifyProcessDied(int32_t pid)
{
    SecCompSrvEnhanceInterface* handler = GetSrvHandler();
    if (handler != nullptr) {
        handler->NotifyProcessDied(pid);
    }
}

int32_t SecCompEnhanceAdapter::CheckComponentInfoEnhance(int32_t pid,
    std::shared_ptr<SecCompBase>& compInfo, const nlohmann::json& jsonComponent)
{
    SecCompSrvEnhanceInterface* handler = GetSrvHandler();
    if (handler != nullptr) {
        return handler->CheckComponentInfoEnhance(pid, compInfo, jsonComponent);
    }
    return SC_OK;
}
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
