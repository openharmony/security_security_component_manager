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
#ifndef FIRST_USE_DIALOG_H
#define FIRST_USE_DIALOG_H

#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include "access_token.h"
#include "iremote_object.h"
#include "nlohmann/json.hpp"
#include "sec_comp_dialog_callback_stub.h"
#include "sec_comp_entity.h"
#include "sec_comp_err.h"
#include "sec_comp_info.h"
#include "sec_event_handler.h"
#include "want.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
bool ReportUserData(const std::string& filePath, const std::string& folderPath);

class SecCompDialogSrvCallback : public SecCompDialogCallbackStub {
public:
    explicit SecCompDialogSrvCallback(int32_t scId, std::shared_ptr<SecCompEntity> sc,
        sptr<IRemoteObject> dialogCallback) : scId_(scId), sc_(sc), dialogCallback_(dialogCallback) {};

    ~SecCompDialogSrvCallback() override
    {
        dialogCallback_ = nullptr;
    };

    void OnDialogClosed(int32_t result) override;
private:
    int32_t scId_;
    std::shared_ptr<SecCompEntity> sc_;
    sptr<IRemoteObject> dialogCallback_;
};

class FirstUseDialog final {
public:
struct DisplayInfo {
    const uint64_t displayId;
    const CrossAxisState crossAxisState;
    const int32_t windowId;
    const int32_t superFoldOffsetY;
};

    static FirstUseDialog& GetInstance();

    ~FirstUseDialog() = default;
    int32_t NotifyFirstUseDialog(std::shared_ptr<SecCompEntity> entity, sptr<IRemoteObject> callerToken,
        sptr<IRemoteObject> dialogCallback, const DisplayInfo& displayInfo);
    void Init(std::shared_ptr<SecEventHandler> secHandler);
    int32_t GrantDialogWaitEntity(int32_t scId);
    void RemoveDialogWaitEntitys(int32_t pid);
    bool SetFirstUseMap(std::shared_ptr<SecCompEntity> entity);

private:
    FirstUseDialog() {};
    bool IsCfgDirExist(void);
    bool IsCfgFileExist(void);
    bool IsCfgFileValid(void);
    bool ReadCfgContent(std::string& content);
    void WriteCfgContent(const std::string& content);
    bool ParseRecord(nlohmann::json& jsonRes,
        AccessToken::AccessTokenID& id, uint64_t& type);
    void ParseRecords(nlohmann::json& jsonRes);
    void LoadFirstUseRecord(void);
    void SaveFirstUseRecord(void);
    bool StartDialogAbility(std::shared_ptr<SecCompEntity> entity, sptr<IRemoteObject> callerToken,
        sptr<IRemoteObject> dialogCallback, const DisplayInfo& displayInfo);
    void StartToastAbility(const std::shared_ptr<SecCompEntity> entity, const sptr<IRemoteObject> callerToken,
        const DisplayInfo& displayInfo);
    bool SetDisplayInfo(AAFwk::Want& want, const DisplayInfo& displayInfo);
    void SendSaveEventHandler(void);

    std::mutex useMapMutex_;
    std::unordered_map<AccessToken::AccessTokenID, uint64_t> firstUseMap_;
    std::unordered_map<int32_t, std::shared_ptr<SecCompEntity>> dialogWaitMap_;
    std::shared_ptr<SecEventHandler> secHandler_;
};
}  // namespace SecurityComponentEnhance
}  // namespace Security
}  // namespace OHOS
#endif  // FIRST_USE_DIALOG_H
