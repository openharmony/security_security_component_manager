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
#include "first_use_dialog.h"

#include <fcntl.h>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "ability_manager_client.h"
#include "accesstoken_kit.h"
#include "sec_comp_err.h"
#include "sec_comp_log.h"
#include "want_params_wrapper.h"
#include "want.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "FirstUseDialog"};
static const std::string SEC_COMP_SRV_CFG_PATH = "/data/service/el1/public/security_component_service";
static const std::string FIRST_USE_RECORD_JSON = SEC_COMP_SRV_CFG_PATH + "/first_use_record.json";
static const std::string FIRST_USE_RECORD_TAG = "FirstUseRecord";
static const std::string TOKEN_ID_TAG = "TokenId";
static const std::string COMP_TYPE_TAG = "CompType";

const std::string GRANT_ABILITY_BUNDLE_NAME = "com.ohos.permissionmanager";
const std::string GRANT_ABILITY_ABILITY_NAME = "com.ohos.permissionmanager.SecurityExtAbility";
const std::string TYPE_KEY = "ohos.user.security.type";
const std::string TOKEN_KEY = "ohos.ability.params.token";

constexpr uint32_t MAX_CFG_FILE_SIZE = 100 * 1024; // 100k
constexpr uint64_t LOCATION_BUTTON_FIRST_USE = 1 << 0;
constexpr uint64_t SAVE_BUTTON_FIRST_USE = 1 << 1;
}

bool FirstUseDialog::IsCfgDirExist(void)
{
    struct stat fstat = {};
    if (stat(SEC_COMP_SRV_CFG_PATH.c_str(), &fstat) != 0) {
        SC_LOG_INFO(LABEL, "path %{public}s errno %{public}d.", SEC_COMP_SRV_CFG_PATH.c_str(), errno);
        return false;
    }

    if (!S_ISDIR(fstat.st_mode)) {
        SC_LOG_ERROR(LABEL, "path %{public}s is not directory.", SEC_COMP_SRV_CFG_PATH.c_str());
        return false;
    }
    return true;
}

bool FirstUseDialog::IsCfgFileExist(void)
{
    struct stat fstat = {};
    if (stat(FIRST_USE_RECORD_JSON.c_str(), &fstat) != 0) {
        SC_LOG_INFO(LABEL, "path %{public}s errno %{public}d.", FIRST_USE_RECORD_JSON.c_str(), errno);
        return false;
    }
    return true;
}

bool FirstUseDialog::IsCfgFileValid(void)
{
    struct stat fstat = {};
    if (stat(FIRST_USE_RECORD_JSON.c_str(), &fstat) != 0) {
        SC_LOG_INFO(LABEL, "path %{public}s errno %{public}d.", FIRST_USE_RECORD_JSON.c_str(), errno);
        return false;
    }
    if (fstat.st_size > MAX_CFG_FILE_SIZE) {
        SC_LOG_INFO(LABEL, "path %{public}s size too large.", FIRST_USE_RECORD_JSON.c_str());
        return false;
    }
    return true;
}

bool FirstUseDialog::ReadCfgContent(std::string& content)
{
    std::stringstream buffer;
    std::ifstream i(FIRST_USE_RECORD_JSON);
    if (!i.is_open()) {
        SC_LOG_ERROR(LABEL, "cannot open file %{public}s, errno %{public}d.", FIRST_USE_RECORD_JSON.c_str(), errno);
        return false;
    }
    buffer << i.rdbuf();
    content = buffer.str();
    i.close();
    return true;
}

void FirstUseDialog::WriteCfgContent(const std::string& content)
{
    std::ofstream out(FIRST_USE_RECORD_JSON);
    if (!out.is_open()) {
        SC_LOG_ERROR(LABEL, "cannot open file %{public}s, errno %{public}d.", FIRST_USE_RECORD_JSON.c_str(), errno);
        return;
    }
    out << content;
    out.close();
}

bool FirstUseDialog::ParseRecord(nlohmann::json& jsonRes,
    AccessToken::AccessTokenID& id, uint64_t& type)
{
    if (jsonRes.find(TOKEN_ID_TAG) == jsonRes.end() ||
        !jsonRes.at(TOKEN_ID_TAG).is_number()) {
        SC_LOG_ERROR(LABEL, "parse TokenId failed.");
        return false;
    }
    id = jsonRes.at(TOKEN_ID_TAG).get<uint32_t>();
    if (id == AccessToken::INVALID_TOKENID) {
        SC_LOG_ERROR(LABEL, "TokenId is not invalid.");
        return false;
    }

    if (jsonRes.find(COMP_TYPE_TAG) == jsonRes.end() ||
        !jsonRes.at(COMP_TYPE_TAG).is_number()) {
        SC_LOG_ERROR(LABEL, "parse CompType failed.");
        return false;
    }
    type = jsonRes.at(COMP_TYPE_TAG).get<uint64_t>();
    return true;
}

void FirstUseDialog::ParseRecords(nlohmann::json& jsonRes)
{
    std::unique_lock<std::mutex> lock(useMapMutex_);
    if (jsonRes.find(FIRST_USE_RECORD_TAG) == jsonRes.end() ||
        !jsonRes.at(FIRST_USE_RECORD_TAG).is_array()) {
        SC_LOG_ERROR(LABEL, "parse tag failed.");
        return;
    }

    nlohmann::json recordListJson = jsonRes.at(FIRST_USE_RECORD_TAG);
    for (auto& recordJson : recordListJson) {
        AccessToken::AccessTokenID id;
        uint64_t type;
        if (!ParseRecord(recordJson, id, type)) {
            SC_LOG_ERROR(LABEL, "parse record failed.");
            return;
        }
        firstUseMap_[id] = type;
    }
}

void FirstUseDialog::LoadFirstUseRecord(void)
{
    if (!IsCfgFileValid()) {
        SC_LOG_INFO(LABEL, "first use record is invalid.");
        return;
    }

    std::string content;
    if (!ReadCfgContent(content)) {
        return;
    }

    nlohmann::json jsonRes = nlohmann::json::parse(content, nullptr, false);
    if (jsonRes.is_discarded()) {
        SC_LOG_ERROR(LABEL, "cfg info format is invalid");
        return;
    }

    ParseRecords(jsonRes);
}

void FirstUseDialog::SaveFirstUseRecord(void)
{
    SC_LOG_INFO(LABEL, "start save first_use_record json");
    if (!IsCfgDirExist()) {
        SC_LOG_ERROR(LABEL, "dir %{public}s is not exist, errno %{public}d",
            SEC_COMP_SRV_CFG_PATH.c_str(), errno);
        return;
    }

    if (!IsCfgFileExist()) {
        if (creat(FIRST_USE_RECORD_JSON.c_str(), S_IRUSR | S_IWUSR) == -1) {
            SC_LOG_ERROR(LABEL, "create %{public}s failed, errno %{public}d",
                FIRST_USE_RECORD_JSON.c_str(), errno);
            return;
        }
    }

    nlohmann::json jsonRes;
    {
        std::unique_lock<std::mutex> lock(useMapMutex_);
        nlohmann::json recordsJson;
        for (auto iter = firstUseMap_.begin(); iter != firstUseMap_.end(); ++iter) {
            AccessToken::AccessTokenID id = iter->first;
            AccessToken::HapTokenInfo info;
            if (AccessToken::AccessTokenKit::GetHapTokenInfo(id, info) != AccessToken::RET_SUCCESS) {
                SC_LOG_INFO(LABEL, "token id %{public}d is not exist, do not update it.", id);
                continue;
            }
            nlohmann::json recordJson;
            recordJson[TOKEN_ID_TAG] = id;
            recordJson[COMP_TYPE_TAG] = iter->second;
            recordsJson.emplace_back(recordJson);
        }

        jsonRes[FIRST_USE_RECORD_TAG] = recordsJson;
    }
    WriteCfgContent(jsonRes.dump());
}

void FirstUseDialog::StartDialogAbility(SecCompType type, sptr<IRemoteObject> callerToken)
{
    int32_t typeNum;
    if (type == LOCATION_COMPONENT) {
        typeNum = 0;
    } else if (type == SAVE_COMPONENT) {
        typeNum = 1;
    } else {
        SC_LOG_ERROR(LABEL, "unknown type.");
        return;
    }

    AAFwk::Want want;
    want.SetElementName(GRANT_ABILITY_BUNDLE_NAME, GRANT_ABILITY_ABILITY_NAME);
    want.SetParam(TYPE_KEY, typeNum);
    want.SetParam(TOKEN_KEY, callerToken);
    int startRes = AAFwk::AbilityManagerClient::GetInstance()->StartExtensionAbility(want, callerToken);
    SC_LOG_INFO(LABEL, "start ability res %{public}d", startRes);
}

void FirstUseDialog::SendSaveEventHandler(void)
{
    std::function<void()> delayed = ([this]() {
        this->SaveFirstUseRecord();
    });

    SC_LOG_INFO(LABEL, "Delay first_use_record json");
    secHandler_->ProxyPostTask(delayed);
}

bool FirstUseDialog::NotifyFirstUseDialog(AccessToken::AccessTokenID tokenId, SecCompType type,
    sptr<IRemoteObject> callerToken)
{
    if (secHandler_ == nullptr) {
        SC_LOG_ERROR(LABEL, "event handler invalid.");
        return false;
    }
    if (callerToken == nullptr) {
        SC_LOG_INFO(LABEL, "callerToken is null, no need to notify dialog");
        return false;
    }

    uint64_t typeMask;
    if (type == LOCATION_COMPONENT) {
        typeMask = LOCATION_BUTTON_FIRST_USE;
    } else if (type == SAVE_COMPONENT) {
        typeMask = SAVE_BUTTON_FIRST_USE;
    } else {
        SC_LOG_INFO(LABEL, "this type need not notify dialog to user");
        return false;
    }

    std::unique_lock<std::mutex> lock(useMapMutex_);
    auto iter = firstUseMap_.find(tokenId);
    if (iter == firstUseMap_.end()) {
        SC_LOG_INFO(LABEL, "has not use record, start dialog");
        StartDialogAbility(type, callerToken);
        firstUseMap_[tokenId] = typeMask;
        SendSaveEventHandler();
        return true;
    }

    uint64_t compTypes = firstUseMap_[tokenId];
    if ((compTypes & typeMask) == typeMask) {
        SC_LOG_INFO(LABEL, "no need notify again.");
        return true;
    }
    StartDialogAbility(type, callerToken);
    firstUseMap_[tokenId] |= typeMask;
    SendSaveEventHandler();
    return true;
}

void FirstUseDialog::Init(std::shared_ptr<SecEventHandler> secHandler)
{
    SC_LOG_DEBUG(LABEL, "Init!!");
    secHandler_ = secHandler;
    LoadFirstUseRecord();
}
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
