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
#ifndef SECURITY_COMPONENT_ENHANCE_ADAPTER_H
#define SECURITY_COMPONENT_ENHANCE_ADAPTER_H

#include <mutex>
#include "iremote_object.h"
#include "nlohmann/json.hpp"
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

// for multimodalinput to add enhance data to PointerEvent
class SecCompInputEnhanceInterface {
public:
    // for multimodalinput to set enhance cfg which is from security component enhance service
    virtual int32_t SetEnhanceCfg(uint8_t* cfg, uint32_t cfgLen) = 0;

    // for multimodalinput to get enhance data
    virtual int32_t GetPointerEventEnhanceData(void* data, uint32_t dataLen,
        uint8_t* enhanceData, uint32_t& enHancedataLen) = 0;
};

// for security component service to send command to enhance service
class SecCompSrvEnhanceInterface {
public:
    // enable input enhance, then enhance service send config to multimodalinput
    virtual int32_t EnableInputEnhance() = 0;

    // disable input enhance
    virtual int32_t DisableInputEnhance() = 0;

    // send click event to enhance service for checking extra data validity
    virtual int32_t CheckExtraInfo(const SecCompClickEvent& clickInfo) = 0;

    // send component info to enhance service for checking its validity
    virtual int32_t CheckComponentInfoEnhance(int32_t pid, std::shared_ptr<SecCompBase>& compInfo,
        const nlohmann::json& jsonComponent) = 0;

    // get RemoteObject of enhance service to connect it
    virtual sptr<IRemoteObject> GetEnhanceRemoteObject() = 0;

    // start enhance service
    virtual void StartEnhanceService() = 0;

    // exit enhance service
    virtual void ExitEnhanceService() = 0;

    // notify process died
    virtual void NotifyProcessDied(int32_t pid) = 0;

    // notify process registered
    virtual void AddSecurityComponentProcess(int32_t pid) = 0;

    virtual bool SerializeSessionInfoEnhance(MessageParcel& tmpReply, MessageParcel& reply, int32_t pid) = 0;
    virtual bool DeserializeSessionInfoEnhance(MessageParcel& oldData, MessageParcel& newData,
        MessageParcel& reply, int32_t pid) = 0;
};

// for client
class SecCompClientEnhanceInterface {
public:
    // preprocess component info which is send to security component service, e.g. RegisterSecurityComponent
    virtual bool EnhanceDataPreprocess(const uintptr_t caller, std::string& componentInfo) = 0;
    virtual bool EnhanceDataPreprocess(const uintptr_t caller, int32_t scId, std::string& componentInfo) = 0;

    virtual bool EnhanceSerializeSessionInfo(const uintptr_t caller,
        MessageParcel& tmpData, MessageParcel& data) = 0;
    virtual bool EnhanceDeserializeSessionInfo(const uintptr_t caller, MessageParcel& oldData,
        MessageParcel& newData) = 0;

    // regiter scid to enhance client
    virtual void RegisterScIdEnhance(const uintptr_t caller, int32_t scId) = 0;
    // unregiter scid to enhance client
    virtual void UnregisterScIdEnhance(const uintptr_t caller, int32_t scId) = 0;
};

class SecCompEnhanceAdapter final {
public:
    static void InitEnhanceHandler(EnhanceInterfaceType type);
    static int32_t SetEnhanceCfg(uint8_t* cfg, uint32_t cfgLen);
    static int32_t GetPointerEventEnhanceData(void* data, uint32_t dataLen,
        uint8_t* enhanceData, uint32_t& enHancedataLen);

    static int32_t CheckExtraInfo(const SecCompClickEvent& clickInfo);
    static int32_t EnableInputEnhance();
    static int32_t DisableInputEnhance();
    static int32_t CheckComponentInfoEnhance(int32_t pid, std::shared_ptr<SecCompBase>& compInfo,
        const nlohmann::json& jsonComponent);
    static sptr<IRemoteObject> GetEnhanceRemoteObject();
    static void StartEnhanceService();
    static void ExitEnhanceService();
    static void NotifyProcessDied(int32_t pid);

    static bool EnhanceDataPreprocess(std::string& componentInfo);
    static bool EnhanceDataPreprocess(int32_t scId, std::string& componentInfo);
    static bool EnhanceSerializeSessionInfo(MessageParcel& tmpData, MessageParcel& data);
    static bool EnhanceDeserializeSessionInfo(MessageParcel& oldData, MessageParcel& newData);
    static void RegisterScIdEnhance(int32_t scId);
    static void UnregisterScIdEnhance(int32_t scId);

    static void AddSecurityComponentProcess(int32_t pid);

    static bool SerializeSessionInfoEnhance(MessageParcel& tmpReply, MessageParcel& reply, int32_t pid);
    static bool DeserializeSessionInfoEnhance(MessageParcel& oldData, MessageParcel& newData,
        MessageParcel& reply, int32_t pid);
    static __attribute__((visibility("default"))) SecCompInputEnhanceInterface* inputHandler;
    static bool isEnhanceInputHandlerInit;

    static __attribute__((visibility("default"))) SecCompSrvEnhanceInterface* srvHandler;
    static bool isEnhanceSrvHandlerInit;

    static __attribute__((visibility("default"))) SecCompClientEnhanceInterface* clientHandler;
    static bool isEnhanceClientHandlerInit;

    static std::mutex initMtx;
};
typedef SecCompClientEnhanceInterface* (*EnhanceInterface) (void);
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
#endif  // SECURITY_COMPONENT_ENHANCE_ADAPTER_H
