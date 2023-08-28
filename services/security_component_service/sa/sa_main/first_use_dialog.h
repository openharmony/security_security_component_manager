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

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include "access_token.h"
#include "iremote_object.h"
#include "nlohmann/json.hpp"
#include "sec_comp_err.h"
#include "sec_comp_info.h"
#include "sec_event_handler.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
class FirstUseDialog final {
public:
    FirstUseDialog() = default;
    ~FirstUseDialog() = default;
    bool NotifyFirstUseDialog(AccessToken::AccessTokenID tokenId, SecCompType type, sptr<IRemoteObject> callerToken);
    void Init(std::shared_ptr<SecEventHandler> secHandler);

private:
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
    void StartDialogAbility(SecCompType type, sptr<IRemoteObject> callerToken);
    void SendSaveEventHandler(void);

    std::mutex useMapMutex_;
    std::unordered_map<AccessToken::AccessTokenID, uint64_t> firstUseMap_;
    std::shared_ptr<SecEventHandler> secHandler_;
};
}  // namespace SecurityComponentEnhance
}  // namespace Security
}  // namespace OHOS
#endif  // FIRST_USE_DIALOG_H
