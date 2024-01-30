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

#include <dlfcn.h>
#include "sec_comp_err.h"
#include "sec_comp_log.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
    LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "SecCompEnhanceAdapter"};

#ifdef _ARM64_
static const std::string LIB_PATH = "/system/lib64/";
#else
static const std::string LIB_PATH = "/system/lib/";
#endif
static const std::string ENHANCE_INPUT_INTERFACE_LIB = LIB_PATH + "libsec_comp_input_enhance.z.so";
static const std::string ENHANCE_SRV_INTERFACE_LIB = LIB_PATH + "libsec_comp_service_enhance.z.so";
static const std::string ENHANCE_CLIENT_INTERFACE_LIB = LIB_PATH + "libsec_comp_client_enhance.z.so";
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
    if (dlopen(libPath.c_str(), RTLD_LAZY) == nullptr) {
        SC_LOG_ERROR(LABEL, "init enhance lib %{public}s failed, error %{public}s", libPath.c_str(), dlerror());
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

void SecCompEnhanceAdapter::ExistEnhanceService()
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

int32_t SecCompEnhanceAdapter::CheckComponentInfoEnhnace(int32_t pid,
    std::shared_ptr<SecCompBase>& compInfo, const nlohmann::json& jsonComponent)
{
    if (!isEnhanceSrvHandlerInit) {
        InitEnhanceHandler(SEC_COMP_ENHANCE_SRV_INTERFACE);
    }
    if (srvHandler != nullptr) {
        return srvHandler->CheckComponentInfoEnhnace(pid, compInfo, jsonComponent);
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
