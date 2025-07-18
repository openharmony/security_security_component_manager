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
#include <cstdlib>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <sys/types.h>
#include <unistd.h>
#include "ability_manager_client.h"
#include "accesstoken_kit.h"
#include "bundle_mgr_client.h"
#include "display.h"
#include "display_info.h"
#include "display_manager.h"
#include "hisysevent.h"
#include "ipc_skeleton.h"
#include "sec_comp_dialog_callback_proxy.h"
#include "sec_comp_err.h"
#include "sec_comp_log.h"
#include "want_params_wrapper.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "FirstUseDialog"};
static const std::string SECURITY_COMPONENT_MANAGER = "security_component_manager";
static const std::string SEC_COMP_SRV_CFG_PATH = "/data/service/el1/public/security_component_service";
static const std::string FIRST_USE_RECORD_JSON = SEC_COMP_SRV_CFG_PATH + "/first_use_record.json";
static const std::string FIRST_USE_RECORD_TAG = "FirstUseRecord";
static const std::string TOKEN_ID_TAG = "TokenId";
static const std::string COMP_TYPE_TAG = "CompType";
static const char* DATA_FOLDER = "/data";

const std::string GRANT_ABILITY_BUNDLE_NAME = "com.ohos.permissionmanager";
const std::string GRANT_ABILITY_ABILITY_NAME = "com.ohos.permissionmanager.SecurityExtAbility";
const std::string TYPE_KEY = "ohos.user.security.type";
const std::string TOKEN_KEY = "ohos.ability.params.token";
const std::string CALLBACK_KEY = "ohos.ability.params.callback";
const std::string WINDOW_ID_KEY = "ohos.ability.params.windowId";
const std::string CALLER_UID_KEY = "ohos.caller.uid";
const std::string DISPLAY_WIDTH = "ohos.display.width";
const std::string DISPLAY_HEIGHT = "ohos.display.height";
const std::string DISPLAY_TOP = "ohos.display.top";
const std::string DIALOG_OFFSET = "ohos.dialog.offset";
const std::string NOTIFY_TYPE = "ohos.ability.notify.type";
const std::string TOAST_POSITION = "ohos.toast.position";
const std::string TOAST_OFFSET = "ohos.toast.offset";

constexpr int32_t DISPLAY_HALF_RATIO = 2;
constexpr uint32_t MAX_CFG_FILE_SIZE = 100 * 1024; // 100k
constexpr uint64_t LOCATION_BUTTON_FIRST_USE = 1 << 0;
constexpr uint64_t SAVE_BUTTON_FIRST_USE = 1 << 1;
constexpr int32_t ABOVE_BOTTOM_OFFSET = 80;
constexpr int32_t BELOW_TOP_OFFSET = 112;
static std::mutex g_instanceMutex;
static std::unordered_map<TipPosition, int32_t> tipPositionsMap = {{TipPosition::ABOVE_BOTTOM, ABOVE_BOTTOM_OFFSET},
    {TipPosition::BELOW_TOP, BELOW_TOP_OFFSET}};
}

bool ReportUserData(const std::string& filePath, const std::string& folderPath)
{
    struct stat fileInfo;
    if (stat(filePath.c_str(), &fileInfo) != 0) {
        SC_LOG_ERROR(LABEL, "Failed to get file stat, path = %{public}s.", filePath.c_str());
        return false;
    }
    int64_t fileSize = fileInfo.st_size;

    struct statfs stat;
    if (statfs(folderPath.c_str(), &stat) != 0) {
        SC_LOG_ERROR(LABEL, "Failed to get data remain size.");
        return false;
    }
    uint64_t remainSize = static_cast<uint64_t>(stat.f_bfree) * stat.f_bsize;

    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::FILEMANAGEMENT, "USER_DATA_SIZE",
        HiviewDFX::HiSysEvent::EventType::STATISTIC, "COMPONENT_NAME", SECURITY_COMPONENT_MANAGER,
        "PARTITION_NAME", folderPath, "REMAIN_PARTITION_SIZE", remainSize,
        "FILE_OR_FOLDER_PATH", filePath, "FILE_OR_FOLDER_SIZE", fileSize);
    return true;
}

void SecCompDialogSrvCallback::OnDialogClosed(int32_t result)
{
    SC_LOG_INFO(LABEL, "Call dialog close callback scId_ %{public}d", scId_);
    int32_t grantRes = FirstUseDialog::GetInstance().GrantDialogWaitEntity(scId_);
    if (grantRes == SC_SERVICE_ERROR_COMPONENT_NOT_EXIST) {
        SC_LOG_ERROR(LABEL, "Call dialog close callback scId_ %{public}d is not exist", scId_);
        return;
    }
    if (!grantRes && !FirstUseDialog::GetInstance().SetFirstUseMap(sc_)) {
        return;
    }
    auto callback = iface_cast<ISecCompDialogCallback>(dialogCallback_);
    if (callback != nullptr) {
        callback->OnDialogClosed(grantRes);
    }
}

FirstUseDialog& FirstUseDialog::GetInstance()
{
    static FirstUseDialog* instance = nullptr;
    if (instance == nullptr) {
        std::lock_guard<std::mutex> lock(g_instanceMutex);
        if (instance == nullptr) {
            instance = new FirstUseDialog();
        }
    }
    return *instance;
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
    char* canonicalPath = realpath(FIRST_USE_RECORD_JSON.c_str(), nullptr);
    if (canonicalPath == nullptr) {
        SC_LOG_ERROR(LABEL, "Cannot get canonical path for %{public}s, errno %{public}d.",
            FIRST_USE_RECORD_JSON.c_str(), errno);
        return false;
    }
    std::ifstream i(canonicalPath);
    if (!i.is_open()) {
        SC_LOG_ERROR(LABEL, "cannot open file %{public}s, errno %{public}d.", canonicalPath, errno);
        free(canonicalPath);
        return false;
    }
    buffer << i.rdbuf();
    content = buffer.str();
    i.close();
    free(canonicalPath);
    return true;
}

void FirstUseDialog::WriteCfgContent(const std::string& content)
{
    char* canonicalPath = realpath(FIRST_USE_RECORD_JSON.c_str(), nullptr);
    if (canonicalPath == nullptr) {
        SC_LOG_ERROR(LABEL, "Cannot get canonical path for %{public}s, errno %{public}d.",
            FIRST_USE_RECORD_JSON.c_str(), errno);
        return;
    }
    std::ofstream out(canonicalPath);
    if (!out.is_open()) {
        SC_LOG_ERROR(LABEL, "cannot open file %{public}s, errno %{public}d.", canonicalPath, errno);
        free(canonicalPath);
        return;
    }
    out << content;
    free(canonicalPath);
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
        SC_LOG_ERROR(LABEL, "TokenId is invalid.");
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
            continue;
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
    if (!ReportUserData(FIRST_USE_RECORD_JSON, DATA_FOLDER)) {
        SC_LOG_ERROR(LABEL, "report user data failed.");
    }
    WriteCfgContent(jsonRes.dump());
}

void FirstUseDialog::RemoveDialogWaitEntitys(int32_t pid)
{
    std::unique_lock<std::mutex> lock(useMapMutex_);
    for (auto iter = dialogWaitMap_.begin(); iter != dialogWaitMap_.end();) {
        std::shared_ptr<SecCompEntity> entity = iter->second;
        if ((entity != nullptr) && (entity->pid_ == pid)) {
            iter = dialogWaitMap_.erase(iter);
        } else {
            ++iter;
        }
    }
}

int32_t FirstUseDialog::GrantDialogWaitEntity(int32_t scId)
{
    std::unique_lock<std::mutex> lock(useMapMutex_);
    auto it = dialogWaitMap_.find(scId);
    if (it == dialogWaitMap_.end()) {
        SC_LOG_ERROR(LABEL, "Grant dialog wait security component %{public}d is not exist", scId);
        return SC_SERVICE_ERROR_COMPONENT_NOT_EXIST;
    }

    std::shared_ptr<SecCompEntity> sc = it->second;
    if (sc == nullptr) {
        SC_LOG_ERROR(LABEL, "Grant dialog wait security component %{public}d is nullptr", scId);
        return SC_SERVICE_ERROR_COMPONENT_NOT_EXIST;
    }
    int32_t res = sc->GrantTempPermission();
    if (res != SC_OK) {
        OHOS::AppExecFwk::BundleMgrClient bmsClient;
        std::string bundleName = "";
        bmsClient.GetNameForUid(sc->uid_, bundleName);
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::SEC_COMPONENT, "TEMP_GRANT_FAILED",
            HiviewDFX::HiSysEvent::EventType::FAULT, "CALLER_UID", sc->uid_, "CALLER_BUNDLE_NAME", bundleName,
            "CALLER_PID", sc->pid_, "SC_ID", scId, "SC_TYPE", sc->GetType());
    } else {
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::SEC_COMPONENT, "TEMP_GRANT_SUCCESS",
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR, "CALLER_UID", sc->uid_,
            "CALLER_PID", sc->pid_, "SC_ID", scId, "SC_TYPE", sc->GetType());
    }
    dialogWaitMap_.erase(scId);
    return res;
}

bool FirstUseDialog::SetDisplayInfo(AAFwk::Want& want, const DisplayInfo& displayInfo)
{
    sptr<OHOS::Rosen::Display> display =
        OHOS::Rosen::DisplayManager::GetInstance().GetDisplayById(displayInfo.displayId);
    if (display == nullptr) {
        SC_LOG_ERROR(LABEL, "Get display manager failed");
        return false;
    }

    auto info = display->GetDisplayInfo();
    if (info == nullptr) {
        SC_LOG_ERROR(LABEL, "Get display info failed");
        return false;
    }
    /* crossAxisState is INVALID or NO_CROSS */
    int32_t width = info->GetWidth();
    int32_t height = info->GetHeight();
    int32_t offset = 0;
    /* crossAxisState is CROSS */
    if (displayInfo.crossAxisState == CrossAxisState::STATE_CROSS) {
        height = info->GetPhysicalHeight();
        offset = static_cast<int32_t>(info->GetAvailableHeight()) / DISPLAY_HALF_RATIO;
    }
    SC_LOG_INFO(LABEL, "Display info width %{public}d height %{public}d, dialog offset %{public}d",
        width, height, offset);

    want.SetParam(DISPLAY_WIDTH, width);
    want.SetParam(DISPLAY_HEIGHT, height);
    want.SetParam(DIALOG_OFFSET, offset);
    return true;
}

void FirstUseDialog::StartToastAbility(const std::shared_ptr<SecCompEntity> entity,
    const sptr<IRemoteObject> callerToken, const DisplayInfo& displayInfo)
{
    if (!entity->AllowToShowToast()) {
        return;
    }
    bool needToShow = AccessToken::AccessTokenKit::IsToastShownNeeded(entity->pid_);
    if (!needToShow) {
        SC_LOG_INFO(LABEL, "Process pid=%{public}d needs not to show", entity->pid_);
        return;
    }
    AAFwk::Want want;
    want.SetElementName(GRANT_ABILITY_BUNDLE_NAME, GRANT_ABILITY_ABILITY_NAME);
    want.SetParam(NOTIFY_TYPE, NotifyType::TOAST);
    want.SetParam(TOAST_POSITION, entity->componentInfo_->tipPosition_);
    int32_t toastOffset = ABOVE_BOTTOM_OFFSET;
    if (tipPositionsMap.find(entity->componentInfo_->tipPosition_) != tipPositionsMap.end()) {
        toastOffset = tipPositionsMap[entity->componentInfo_->tipPosition_];
    }
    want.SetParam(TOAST_OFFSET, toastOffset);
    int32_t superFoldOffsetY = 0;
    if (entity->IsInPCVirtualScreen(displayInfo.crossAxisState)) {
        superFoldOffsetY = displayInfo.superFoldOffsetY;
    }
    want.SetParam(DISPLAY_TOP, superFoldOffsetY);
    if (!SetDisplayInfo(want, displayInfo)) {
        SC_LOG_ERROR(LABEL, "Set display info failed.");
        return;
    }

    int startRes = AAFwk::AbilityManagerClient::GetInstance()->StartExtensionAbility(want, callerToken);
    SC_LOG_INFO(LABEL, "Start toast ability res %{public}d", startRes);
}

void FirstUseDialog::StartDialogAbility(std::shared_ptr<SecCompEntity> entity, sptr<IRemoteObject> callerToken,
    sptr<IRemoteObject> dialogCallback, const DisplayInfo& displayInfo)
{
    int32_t typeNum;
    SecCompType type = entity->GetType();
    if (type == LOCATION_COMPONENT) {
        typeNum = 0;
    } else if (type == SAVE_COMPONENT) {
        typeNum = 1;
    } else {
        SC_LOG_ERROR(LABEL, "Unknown type.");
        return;
    }
    int32_t scId = entity->scId_;
    SecCompDialogSrvCallback *call = new (std::nothrow)SecCompDialogSrvCallback(scId, entity, dialogCallback);
    sptr<IRemoteObject> srvCallback = call;
    if (srvCallback == nullptr) {
        SC_LOG_ERROR(LABEL, "New SecCompDialogCallback fail");
        return;
    }
    dialogWaitMap_[scId] = entity;
    AAFwk::Want want;
    want.SetElementName(GRANT_ABILITY_BUNDLE_NAME, GRANT_ABILITY_ABILITY_NAME);
    want.SetParam(TYPE_KEY, typeNum);
    want.SetParam(TOKEN_KEY, callerToken);
    want.SetParam(CALLBACK_KEY, srvCallback);
    want.SetParam(WINDOW_ID_KEY, displayInfo.windowId);
    want.SetParam(NOTIFY_TYPE, NotifyType::DIALOG);
    int32_t uid = IPCSkeleton::GetCallingUid();
    want.SetParam(CALLER_UID_KEY, uid);
    if (!SetDisplayInfo(want, displayInfo)) {
        SC_LOG_ERROR(LABEL, "Set display info failed.");
        return;
    }

    int startRes = AAFwk::AbilityManagerClient::GetInstance()->StartExtensionAbility(want, callerToken);
    SC_LOG_INFO(LABEL, "Start ability res %{public}d", startRes);
    if (startRes != 0) {
        dialogWaitMap_.erase(scId);
    }
}

void FirstUseDialog::SendSaveEventHandler(void)
{
    std::function<void()> delayed = ([this]() {
        this->SaveFirstUseRecord();
    });

    SC_LOG_INFO(LABEL, "Delay first_use_record json");
    secHandler_->ProxyPostTask(delayed);
}

bool FirstUseDialog::SetFirstUseMap(std::shared_ptr<SecCompEntity> entity)
{
    uint64_t typeMask;
    if (entity == nullptr) {
        SC_LOG_ERROR(LABEL, "Entity is invalid.");
        return false;
    }

    SecCompType type = entity->GetType();
    if (type == LOCATION_COMPONENT) {
        SC_LOG_INFO(LABEL, "LocationButton need not notify dialog to user.");
        return true;
    }
    if (type == SAVE_COMPONENT) {
        typeMask = SAVE_BUTTON_FIRST_USE;
    } else {
        SC_LOG_INFO(LABEL, "This type need not notify dialog to user.");
        return false;
    }

    std::unique_lock<std::mutex> lock(useMapMutex_);
    AccessToken::AccessTokenID tokenId = entity->tokenId_;
    auto iter = firstUseMap_.find(tokenId);
    if (iter == firstUseMap_.end()) {
        firstUseMap_[tokenId] = typeMask;
        SendSaveEventHandler();
        return true;
    }

    firstUseMap_[tokenId] |= typeMask;
    SendSaveEventHandler();
    return true;
}

int32_t FirstUseDialog::NotifyFirstUseDialog(std::shared_ptr<SecCompEntity> entity, sptr<IRemoteObject> callerToken,
    sptr<IRemoteObject> dialogCallback, const DisplayInfo& displayInfo)
{
    if (entity == nullptr) {
        SC_LOG_ERROR(LABEL, "Entity is invalid.");
        return SC_SERVICE_ERROR_VALUE_INVALID;
    }
    if (secHandler_ == nullptr) {
        SC_LOG_ERROR(LABEL, "event handler invalid.");
        return SC_SERVICE_ERROR_VALUE_INVALID;
    }
    if (callerToken == nullptr) {
        SC_LOG_INFO(LABEL, "callerToken is null, no need to notify dialog");
        return SC_SERVICE_ERROR_VALUE_INVALID;
    }

    if (dialogCallback == nullptr) {
        SC_LOG_INFO(LABEL, "DialogCallback is null, no need to notify dialog");
        return SC_SERVICE_ERROR_VALUE_INVALID;
    }

    uint64_t typeMask;
    SecCompType type = entity->GetType();
    if (type == LOCATION_COMPONENT) {
        typeMask = LOCATION_BUTTON_FIRST_USE;
    } else if (type == SAVE_COMPONENT) {
        typeMask = SAVE_BUTTON_FIRST_USE;
    } else {
        SC_LOG_INFO(LABEL, "this type need not notify dialog to user");
        return SC_OK;
    }

    std::unique_lock<std::mutex> lock(useMapMutex_);
    AccessToken::AccessTokenID tokenId = entity->tokenId_;
    auto iter = firstUseMap_.find(tokenId);
    if (iter == firstUseMap_.end()) {
        SC_LOG_INFO(LABEL, "has not use record, start dialog");
        StartDialogAbility(entity, callerToken, dialogCallback, displayInfo);
        return SC_SERVICE_ERROR_WAIT_FOR_DIALOG_CLOSE;
    }

    uint64_t compTypes = firstUseMap_[tokenId];
    if ((typeMask == SAVE_BUTTON_FIRST_USE) && ((compTypes & typeMask) == typeMask)) {
        SC_LOG_INFO(LABEL, "no need notify dialog again.");
        StartToastAbility(entity, callerToken, displayInfo);
        return SC_OK;
    }
    StartDialogAbility(entity, callerToken, dialogCallback, displayInfo);
    return SC_SERVICE_ERROR_WAIT_FOR_DIALOG_CLOSE;
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
