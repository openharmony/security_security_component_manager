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
#ifndef SECURITY_COMPONENT_ENHANCE_ADAPTER_H
#define SECURITY_COMPONENT_ENHANCE_ADAPTER_H
#include <mutex>
#include "iremote_object.h"
#include "nlohmann/json.hpp"
#include "parcel.h"
#include "sec_comp_base.h"
#include "sec_comp_info.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
enum EnhanceInterfaceType {
    SEC_COMP_ENHANCE_INPUT_INTERFACE = 0,
    SEC_COMP_ENHANCE_SRV_INTERFACE = 1,
    SEC_COMP_ENHANCE_CLIENT_INTERFACE = 2,
};

class SecCompEnhanceCfgBase {
};

class SecCompInputEnhanceInterface {
public:
    virtual int32_t SetEnhanceCfg(uint8_t* cfg, uint32_t cfgLen) = 0;
    virtual int32_t GetPointerEventEnhanceData(void* data, uint32_t dataLen,
        uint8_t* enhanceData, uint32_t& enHancedataLen) = 0;
};

class SecCompSrvEnhanceInterface {
 public:
    virtual int32_t EnableInputEnhance() = 0;
    virtual int32_t DisableInputEnhance() = 0;
    virtual int32_t CheckExtraInfo(const SecCompClickEvent& touchInfo) = 0;
    virtual int32_t CheckComponentInfoEnhnace(int32_t pid, std::shared_ptr<SecCompBase>& compInfo,
        const nlohmann::json& jsonComponent) = 0;
    virtual sptr<IRemoteObject> GetEnhanceRemoteObject() = 0;
    virtual void StartEnhanceService() = 0;
    virtual void ExitEnhanceService() = 0;
    virtual void NotifyProcessDied(int32_t pid) = 0;
};

class SecCompClientEnhanceInterface {
public:
    virtual bool EnhanceDataPreprocess(const uintptr_t caller, std::string& componentInfo) = 0;
    virtual bool EnhanceDataPreprocess(const uintptr_t caller, int32_t scId, std::string& componentInfo) = 0;
    virtual void RegisterScIdEnhance(const uintptr_t caller, int32_t scId) = 0;
    virtual void UnregisterScIdEnhance(const uintptr_t caller, int32_t scId) = 0;
};

struct SecCompEnhanceAdapter {
    static void InitEnhanceHandler(EnhanceInterfaceType type);
    static int32_t SetEnhanceCfg(uint8_t* cfg, uint32_t cfgLen);
    static int32_t GetPointerEventEnhanceData(void* data, uint32_t dataLen,
        uint8_t* enhanceData, uint32_t& enHancedataLen);
    static int32_t CheckExtraInfo(const SecCompClickEvent& touchInfo);
    static int32_t EnableInputEnhance();
    static int32_t DisableInputEnhance();

    static int32_t CheckComponentInfoEnhnace(int32_t pid, std::shared_ptr<SecCompBase>& compInfo,
        const nlohmann::json& jsonComponent);
    static sptr<IRemoteObject> GetEnhanceRemoteObject();
    static bool EnhanceDataPreprocess(std::string& componentInfo);
    static bool EnhanceDataPreprocess(int32_t scId, std::string& componentInfo);
    static void RegisterScIdEnhance(int32_t scId);
    static void UnregisterScIdEnhance(int32_t scId);

    static void StartEnhanceService();
    static void ExistEnhanceService();
    static void NotifyProcessDied(int32_t pid);

    static SecCompInputEnhanceInterface* inputHandler;
    static bool isEnhanceInputHandlerInit;

    static SecCompSrvEnhanceInterface* srvHandler;
    static bool isEnhanceSrvHandlerInit;

    static SecCompClientEnhanceInterface* clientHandler;
    static bool isEnhanceClientHandlerInit;

    static std::mutex initMtx;
};
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
#endif  // SECURITY_COMPONENT_ENHANCE_ADAPTER_H
