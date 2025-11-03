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
#include "sec_comp_service.h"

#include <unistd.h>

#include "app_mgr_death_recipient.h"
#include "bundle_info.h"
#include "bundle_mgr_client.h"
#include "hisysevent.h"
#include "hitrace_meter.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "sec_comp_click_event_parcel.h"
#include "sec_comp_enhance_adapter.h"
#include "sec_comp_err.h"
#include "sec_comp_manager.h"
#include "sec_comp_log.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "SecCompService"};
static const int32_t ROOT_UID = 0;
static constexpr int32_t BASE_USER_RANGE = 200000;
#ifndef SA_ID_SECURITY_COMPONENT_SERVICE
constexpr int32_t SA_ID_SECURITY_COMPONENT_SERVICE = 3506;
#endif
}

REGISTER_SYSTEM_ABILITY_BY_ID(SecCompService, SA_ID_SECURITY_COMPONENT_SERVICE, true);

SecCompService::SecCompService(int32_t saId, bool runOnCreate)
    : SystemAbility(saId, runOnCreate), state_(ServiceRunningState::STATE_NOT_START)
{
}

SecCompService::~SecCompService()
{
}

void SecCompService::OnStart()
{
    StartTrace(HITRACE_TAG_ACCESS_CONTROL, "SecurityComponentOnStart");
    if (state_ == ServiceRunningState::STATE_RUNNING) {
        SC_LOG_INFO(LABEL, "SecCompService has already started!");
        FinishTrace(HITRACE_TAG_ACCESS_CONTROL);
        return;
    }
    SC_LOG_INFO(LABEL, "SecCompService is starting");
    if (!RegisterAppStateObserver()) {
        SC_LOG_ERROR(LABEL, "Failed to register app state observer!");
        FinishTrace(HITRACE_TAG_ACCESS_CONTROL);
        return;
    }
    if (!Initialize()) {
        SC_LOG_ERROR(LABEL, "Failed to initialize");
        FinishTrace(HITRACE_TAG_ACCESS_CONTROL);
        return;
    }

    state_ = ServiceRunningState::STATE_RUNNING;
    bool ret = Publish(this);
    if (!ret) {
        SC_LOG_ERROR(LABEL, "Failed to publish service!");
        FinishTrace(HITRACE_TAG_ACCESS_CONTROL);
        return;
    }
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::SEC_COMPONENT, "SERVICE_INIT_SUCCESS",
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR, "PID", getpid());
    SC_LOG_INFO(LABEL, "Congratulations, SecCompService start successfully!");
    FinishTrace(HITRACE_TAG_ACCESS_CONTROL);
}

void SecCompService::OnStop()
{
    SC_LOG_INFO(LABEL, "Stop service");
    state_ = ServiceRunningState::STATE_NOT_START;
    UnregisterAppStateObserver();
}

bool SecCompService::RegisterAppStateObserver()
{
    std::unique_lock<std::mutex> lock(secCompSrvMutex_);
    if (appStateObserver_ != nullptr) {
        SC_LOG_INFO(LABEL, "AppStateObserver instance already create");
        return true;
    }
    appStateObserver_ = new (std::nothrow) AppStateObserver();
    if (appStateObserver_ == nullptr) {
        SC_LOG_ERROR(LABEL, "Failed to create AppStateObserver instance");
        return false;
    }
    sptr<ISystemAbilityManager> samgrClient = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgrClient == nullptr) {
        SC_LOG_ERROR(LABEL, "Failed to get system ability manager");
        appStateObserver_ = nullptr;
        return false;
    }
    auto remoteObject = samgrClient->GetSystemAbility(APP_MGR_SERVICE_ID);
    iAppMgr_ = iface_cast<AppExecFwk::IAppMgr>(remoteObject);
    if (iAppMgr_ == nullptr) {
        SC_LOG_ERROR(LABEL, "Failed to get ability manager service");
        appStateObserver_ = nullptr;
        return false;
    }

    if (iAppMgr_->RegisterApplicationStateObserver(appStateObserver_) != ERR_OK) {
        SC_LOG_ERROR(LABEL, "Failed to Register app state observer");
        iAppMgr_ = nullptr;
        appStateObserver_ = nullptr;
        return false;
    }

    sptr<AppMgrDeathRecipient> appMgrDeathRecipient = new (std::nothrow) AppMgrDeathRecipient();
    if (appMgrDeathRecipient == nullptr) {
        SC_LOG_ERROR(LABEL, "Alloc appMgr death observer fail");
        return false;
    }

    if (!remoteObject->AddDeathRecipient(appMgrDeathRecipient)) {
        SC_LOG_ERROR(LABEL, "Add service death observer fail");
        return false;
    }

    std::vector<AppExecFwk::AppStateData> list;
    if (iAppMgr_->GetForegroundApplications(list) == ERR_OK) {
        for (auto it = list.begin(); it != list.end(); ++it) {
            appStateObserver_->AddProcessToForegroundSet(*it);
        }
    }
    SC_LOG_INFO(LABEL, "Register app state observer success");
    return true;
}

void SecCompService::UnregisterAppStateObserver()
{
    std::unique_lock<std::mutex> lock(secCompSrvMutex_);
    if (iAppMgr_ != nullptr && appStateObserver_ != nullptr) {
        iAppMgr_->UnregisterApplicationStateObserver(appStateObserver_);
    }
}

bool SecCompService::GetCallerInfo(SecCompCallerInfo& caller)
{
    AccessToken::AccessTokenID tokenId = IPCSkeleton::GetCallingTokenID();
    int32_t pid = IPCSkeleton::GetCallingPid();
    int32_t uid = IPCSkeleton::GetCallingUid();
    if ((uid != ROOT_UID) && (AccessToken::AccessTokenKit::GetTokenTypeFlag(tokenId) != AccessToken::TOKEN_HAP)) {
        SC_LOG_ERROR(LABEL, "Get caller tokenId invalid");
        return false;
    }
    std::unique_lock<std::mutex> lock(secCompSrvMutex_);
    if ((uid != ROOT_UID) && (!appStateObserver_->IsProcessForeground(pid, uid))) {
        SC_LOG_ERROR(LABEL, "caller pid is not in foreground");
        return false;
    }
    caller.tokenId = tokenId;
    caller.pid = pid;
    caller.uid = uid;
    return true;
}

int32_t SecCompService::ParseParams(const std::string& componentInfo,
    SecCompCallerInfo& caller, nlohmann::json& jsonRes)
{
    if (!GetCallerInfo(caller)) {
        SC_LOG_ERROR(LABEL, "Check caller failed");
        return SC_SERVICE_ERROR_VALUE_INVALID;
    }

    jsonRes = nlohmann::json::parse(componentInfo, nullptr, false);
    if (jsonRes.is_discarded()) {
        SC_LOG_ERROR(LABEL, "component info invalid %{public}s", componentInfo.c_str());
        return SC_SERVICE_ERROR_VALUE_INVALID;
    }
    return SC_OK;
}

int32_t SecCompService::WriteError(int32_t res, SecCompRawdata& rawReply)
{
    MessageParcel replyParcel;
    if (!replyParcel.WriteInt32(res)) {
        SC_LOG_ERROR(LABEL, "Write error res failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (!SecCompEnhanceAdapter::EnhanceSrvSerialize(replyParcel, rawReply)) {
        SC_LOG_ERROR(LABEL, "Serialize error session info failed");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }
    return SC_OK;
}

int32_t SecCompService::RegisterReadFromRawdata(SecCompRawdata& rawData, SecCompType& type, std::string& componentInfo)
{
    MessageParcel deserializedData;
    if (!SecCompEnhanceAdapter::EnhanceSrvDeserialize(rawData, deserializedData)) {
        SC_LOG_ERROR(LABEL, "Register deserialize session info failed");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    uint32_t uintType;
    if (!deserializedData.ReadUint32(uintType)) {
        SC_LOG_ERROR(LABEL, "Register read component type failed");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (uintType <= UNKNOWN_SC_TYPE || uintType >= MAX_SC_TYPE) {
        SC_LOG_ERROR(LABEL, "Register security component type invalid");
        return SC_SERVICE_ERROR_VALUE_INVALID;
    }
    type = static_cast<SecCompType>(uintType);

    if (!deserializedData.ReadString(componentInfo)) {
        SC_LOG_ERROR(LABEL, "Register read component info failed");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }
    return SC_OK;
}

int32_t SecCompService::RegisterSecurityComponentBody(SecCompType type,
    const std::string& componentInfo, int32_t& scId)
{
    StartTrace(HITRACE_TAG_ACCESS_CONTROL, "SecurityComponentRegister");
    SecCompCallerInfo caller;
    caller.tokenId = IPCSkeleton::GetCallingTokenID();
    caller.pid = IPCSkeleton::GetCallingPid();
    caller.uid = IPCSkeleton::GetCallingUid();
    if ((caller.uid != ROOT_UID)
        && (AccessToken::AccessTokenKit::GetTokenTypeFlag(caller.tokenId) != AccessToken::TOKEN_HAP)) {
        SC_LOG_ERROR(LABEL, "Get caller tokenId invalid");
        FinishTrace(HITRACE_TAG_ACCESS_CONTROL);
        return SC_SERVICE_ERROR_VALUE_INVALID;
    }
    nlohmann::json jsonRes = nlohmann::json::parse(componentInfo, nullptr, false);
    if (jsonRes.is_discarded()) {
        SC_LOG_ERROR(LABEL, "component info invalid %{public}s", componentInfo.c_str());
        FinishTrace(HITRACE_TAG_ACCESS_CONTROL);
        return SC_SERVICE_ERROR_VALUE_INVALID;
    }

    int32_t res = SecCompManager::GetInstance().RegisterSecurityComponent(type, jsonRes, caller, scId);
    FinishTrace(HITRACE_TAG_ACCESS_CONTROL);
    if (res != SC_OK) {
        return res;
    }

    int32_t uid = IPCSkeleton::GetCallingUid();
    OHOS::AppExecFwk::BundleMgrClient bmsClient;
    std::string bundleName = "";
    if (bmsClient.GetNameForUid(uid, bundleName) != SC_OK) {
        SC_LOG_ERROR(LABEL, "Failed to get bundle name for UID %{public}d", uid);
        return res;
    }

    AppExecFwk::BundleInfo bundleInfo;
    int32_t userId = uid / BASE_USER_RANGE;
    if (bmsClient.GetBundleInfo(bundleName, AppExecFwk::BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo, userId) != SC_OK) {
        SC_LOG_ERROR(LABEL, "Failed to get bundle info for bundle name %{public}s", bundleName.c_str());
        return res;
    }

    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::SEC_COMPONENT, "REGISTER_SUCCESS",
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR, "CALLER_UID", IPCSkeleton::GetCallingUid(),
        "CALLER_PID", IPCSkeleton::GetCallingRealPid(), "CALLER_BUNDLE_NAME", bundleName, "CALLER_BUNDLE_VERSION",
        bundleInfo.versionName, "SC_ID", scId, "SC_TYPE", type);
    return res;
}

int32_t SecCompService::RegisterWriteToRawdata(int32_t res, int32_t scId, SecCompRawdata& rawReply)
{
    MessageParcel replyParcel;
    if (!replyParcel.WriteInt32(res)) {
        SC_LOG_ERROR(LABEL, "Register security component result failed");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (!replyParcel.WriteInt32(scId)) {
        SC_LOG_ERROR(LABEL, "Register security component result failed");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (!SecCompEnhanceAdapter::EnhanceSrvSerialize(replyParcel, rawReply)) {
        SC_LOG_ERROR(LABEL, "Register serialize session info failed");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }
    return SC_OK;
}

int32_t SecCompService::RegisterSecurityComponent(const SecCompRawdata& rawData, SecCompRawdata& rawReply)
{
    SecCompType type;
    std::string componentInfo;
    int32_t res;
    do {
        res = RegisterReadFromRawdata(const_cast<SecCompRawdata&>(rawData), type, componentInfo);
        if (res != SC_OK) {
            break;
        }
        int32_t scId = INVALID_SC_ID;

        res = RegisterSecurityComponentBody(type, componentInfo, scId);
        if (res != SC_OK) {
            break;
        }
        res = RegisterWriteToRawdata(res, scId, rawReply);
    } while (0);
    if (res != SC_OK) {
        if (WriteError(res, rawReply) != SC_OK) {
            SC_LOG_ERROR(LABEL, "Write rawReply error.");
            return res;
        }
    }
    return SC_OK;
}

int32_t SecCompService::UpdateReadFromRawdata(SecCompRawdata& rawData, int32_t& scId, std::string& componentInfo)
{
    MessageParcel deserializedData;
    if (!SecCompEnhanceAdapter::EnhanceSrvDeserialize(rawData, deserializedData)) {
        SC_LOG_ERROR(LABEL, "Update deserialize session info failed");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (!deserializedData.ReadInt32(scId)) {
        SC_LOG_ERROR(LABEL, "Update read component id failed");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (scId < 0) {
        SC_LOG_ERROR(LABEL, "Update security component id invalid");
        return SC_SERVICE_ERROR_VALUE_INVALID;
    }

    if (!deserializedData.ReadString(componentInfo)) {
        SC_LOG_ERROR(LABEL, "Update read component info failed");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }
    return SC_OK;
}

int32_t SecCompService::UpdateSecurityComponentBody(int32_t scId, const std::string& componentInfo)
{
    SecCompCallerInfo caller;
    nlohmann::json jsonRes;
    if (ParseParams(componentInfo, caller, jsonRes) != SC_OK) {
        return SC_SERVICE_ERROR_VALUE_INVALID;
    }
    return SecCompManager::GetInstance().UpdateSecurityComponent(scId, jsonRes, caller);
}

int32_t SecCompService::UpdateWriteToRawdata(int32_t res, SecCompRawdata& rawReply)
{
    MessageParcel replyParcel;
    if (!replyParcel.WriteInt32(res)) {
        SC_LOG_ERROR(LABEL, "Update security component result failed");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (!SecCompEnhanceAdapter::EnhanceSrvSerialize(replyParcel, rawReply)) {
        SC_LOG_ERROR(LABEL, "Update serialize session info failed");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }
    return SC_OK;
}

int32_t SecCompService::UpdateSecurityComponent(const SecCompRawdata& rawData, SecCompRawdata& rawReply)
{
    int32_t scId;
    std::string componentInfo;
    int32_t res;
    do {
        res = UpdateReadFromRawdata(const_cast<SecCompRawdata&>(rawData), scId, componentInfo);
        if (res != SC_OK) {
            break;
        }
        res = UpdateSecurityComponentBody(scId, componentInfo);
        if (res != SC_OK) {
            break;
        }
        res = UpdateWriteToRawdata(res, rawReply);
    } while (0);
    if (res != SC_OK) {
        if (WriteError(res, rawReply) != SC_OK) {
            SC_LOG_ERROR(LABEL, "Write rawReply error.");
            return res;
        }
    }
    return SC_OK;
}

int32_t SecCompService::UnregisterReadFromRawdata(SecCompRawdata& rawData, int32_t& scId)
{
    MessageParcel deserializedData;
    if (!SecCompEnhanceAdapter::EnhanceSrvDeserialize(rawData, deserializedData)) {
        SC_LOG_ERROR(LABEL, "Unregister deserialize session info failed");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }
    if (!deserializedData.ReadInt32(scId)) {
        SC_LOG_ERROR(LABEL, "Unregister read component id failed");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (scId < 0) {
        SC_LOG_ERROR(LABEL, "Unregister security component id invalid");
        return SC_SERVICE_ERROR_VALUE_INVALID;
    }
    return SC_OK;
}

int32_t SecCompService::UnregisterSecurityComponentBody(int32_t scId)
{
    SecCompCallerInfo caller;
    caller.tokenId = IPCSkeleton::GetCallingTokenID();
    caller.pid = IPCSkeleton::GetCallingPid();
    caller.uid = IPCSkeleton::GetCallingUid();

    return SecCompManager::GetInstance().UnregisterSecurityComponent(scId, caller);
}

int32_t SecCompService::UnregisterWriteToRawdata(int32_t res, SecCompRawdata& rawReply)
{
    MessageParcel replyParcel;
    if (!replyParcel.WriteInt32(res)) {
        SC_LOG_ERROR(LABEL, "Unregister security component result failed");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (!SecCompEnhanceAdapter::EnhanceSrvSerialize(replyParcel, rawReply)) {
        SC_LOG_ERROR(LABEL, "Unregister serialize session info failed");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }
    return SC_OK;
}

int32_t SecCompService::UnregisterSecurityComponent(const SecCompRawdata& rawData, SecCompRawdata& rawReply)
{
    int32_t scId;
    int32_t res;
    do {
        res = UnregisterReadFromRawdata(const_cast<SecCompRawdata&>(rawData), scId);
        if (res != SC_OK) {
            break;
        }

        res = UnregisterSecurityComponentBody(scId);
        if (res != SC_OK) {
            break;
        }
        res = UnregisterWriteToRawdata(res, rawReply);
    } while (0);
    if (res != SC_OK) {
        if (WriteError(res, rawReply) != SC_OK) {
            SC_LOG_ERROR(LABEL, "Write rawReply error.");
            return res;
        }
    }
    return SC_OK;
}

int32_t SecCompService::ReportSecurityComponentClickEventBody(SecCompInfo& secCompInfo,
    sptr<IRemoteObject> callerToken, sptr<IRemoteObject> dialogCallback, std::string& message)
{
    StartTrace(HITRACE_TAG_ACCESS_CONTROL, "SecurityComponentClick");
    SecCompCallerInfo caller;
    nlohmann::json jsonRes;
    if (ParseParams(secCompInfo.componentInfo, caller, jsonRes) != SC_OK) {
        FinishTrace(HITRACE_TAG_ACCESS_CONTROL);
        return SC_SERVICE_ERROR_VALUE_INVALID;
    }
    std::vector<sptr<IRemoteObject>> remoteArr = { callerToken, dialogCallback };
    int32_t res =
        SecCompManager::GetInstance().ReportSecurityComponentClickEvent(secCompInfo, jsonRes, caller, remoteArr,
        message);
    FinishTrace(HITRACE_TAG_ACCESS_CONTROL);
    return res;
}

int32_t SecCompService::ReportWriteToRawdata(int32_t res, std::string message, SecCompRawdata& rawReply)
{
    MessageParcel replyParcel;
    if (!replyParcel.WriteInt32(res)) {
        SC_LOG_ERROR(LABEL, "Report security component result failed");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (!replyParcel.WriteString(message)) {
        SC_LOG_ERROR(LABEL, "Report security component error message failed");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (!SecCompEnhanceAdapter::EnhanceSrvSerialize(replyParcel, rawReply)) {
        SC_LOG_ERROR(LABEL, "Report serialize session info failed");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    return SC_OK;
}

int32_t SecCompService::ReportSecurityComponentClickEvent(const sptr<IRemoteObject>& callerToken,
    const sptr<IRemoteObject>& dialogCallback, const SecCompRawdata& rawData, SecCompRawdata& rawReply)
{
    int32_t res;
    do {
        MessageParcel deserializedData;
        if (!SecCompEnhanceAdapter::EnhanceSrvDeserialize(const_cast<SecCompRawdata&>(rawData), deserializedData)) {
            SC_LOG_ERROR(LABEL, "Report deserialize session info failed");
            res = SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
            break;
        }

        int32_t scId;
        if (!deserializedData.ReadInt32(scId)) {
            SC_LOG_ERROR(LABEL, "Report read component id failed");
            res = SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
            break;
        }

        if (scId < 0) {
            SC_LOG_ERROR(LABEL, "Report security component id invalid");
            res = SC_SERVICE_ERROR_VALUE_INVALID;
            break;
        }

        std::string componentInfo;
        if (!deserializedData.ReadString(componentInfo)) {
            SC_LOG_ERROR(LABEL, "Report read component info failed");
            res = SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
            break;
        }
        std::string message;
        if (!deserializedData.ReadString(message)) {
            SC_LOG_ERROR(LABEL, "Report read message failed");
            res = SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
            break;
        }
        sptr<SecCompClickEventParcel> clickInfoParcel = deserializedData.ReadParcelable<SecCompClickEventParcel>();
        if (clickInfoParcel == nullptr) {
            SC_LOG_ERROR(LABEL, "Report read clickInfo info failed");
            res = SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
            break;
        }

        SecCompInfo secCompInfo{ scId, componentInfo, clickInfoParcel->clickInfoParams_ };
        res = ReportSecurityComponentClickEventBody(secCompInfo, callerToken, dialogCallback, message);
        res = ReportWriteToRawdata(res, message, rawReply);
    } while (0);
    if (res != SC_OK) {
        if (WriteError(res, rawReply) != SC_OK) {
            SC_LOG_ERROR(LABEL, "Write rawReply error.");
            return res;
        }
    }
    return SC_OK;
}

int32_t SecCompService::PreRegisterReadFromRawdata(SecCompRawdata& rawData)
{
    MessageParcel deserializedData;
    if (!SecCompEnhanceAdapter::EnhanceSrvDeserialize(rawData, deserializedData)) {
        SC_LOG_ERROR(LABEL, "preRegister deserialize session info failed");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }
    return SC_OK;
}

int32_t SecCompService::PreRegisterSecCompProcessBody()
{
    SecCompCallerInfo caller;
    if (!GetCallerInfo(caller)) {
        SC_LOG_ERROR(LABEL, "Check caller failed");
        return SC_SERVICE_ERROR_VALUE_INVALID;
    }
    return SecCompManager::GetInstance().AddSecurityComponentProcess(caller);
}

int32_t SecCompService::PreRegisterWriteToRawdata(int32_t res, SecCompRawdata& rawReply)
{
    MessageParcel replyParcel;
    if (!replyParcel.WriteInt32(res)) {
        SC_LOG_ERROR(LABEL, "preRegister write result failed");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (!SecCompEnhanceAdapter::EnhanceSrvSerialize(replyParcel, rawReply)) {
        SC_LOG_ERROR(LABEL, "preRegister serialize session info failed");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }
    return SC_OK;
}

int32_t SecCompService::PreRegisterSecCompProcess(const SecCompRawdata& rawData, SecCompRawdata& rawReply)
{
    int32_t res;
    do {
        res = PreRegisterReadFromRawdata(const_cast<SecCompRawdata&>(rawData));
        if (res != SC_OK) {
            break;
        }

        res = PreRegisterSecCompProcessBody();
        if (res != SC_OK) {
            break;
        }
        res = PreRegisterWriteToRawdata(res, rawReply);
    } while (0);
    if (res != SC_OK) {
        if (WriteError(res, rawReply) != SC_OK) {
            SC_LOG_ERROR(LABEL, "Write rawReply error.");
            return res;
        }
    }
    return SC_OK;
}

int32_t SecCompService::VerifySavePermission(AccessToken::AccessTokenID tokenId, bool& isGranted)
{
    if (!IsMediaLibraryCalling()) {
        SC_LOG_ERROR(LABEL, "Not medialibrary called");
        return SC_SERVICE_ERROR_CALLER_INVALID;
    }

    if (tokenId == 0) {
        SC_LOG_ERROR(LABEL, "Verify AccessTokenId invalid");
        return SC_SERVICE_ERROR_VALUE_INVALID;
    }
    isGranted = SecCompPermManager::GetInstance().VerifySavePermission(tokenId);
    return SC_OK;
}

bool SecCompService::IsMediaLibraryCalling()
{
    std::unique_lock<std::mutex> lock(mediaLibMutex_);
    int32_t uid = IPCSkeleton::GetCallingUid();
    if (uid == ROOT_UID) {
        return true;
    }
    int32_t userId = uid / BASE_USER_RANGE;
    uint32_t tokenCaller = IPCSkeleton::GetCallingTokenID();
    if (mediaLibraryTokenId_ != tokenCaller) {
        mediaLibraryTokenId_ = AccessToken::AccessTokenKit::GetHapTokenID(
            userId, "com.ohos.medialibrary.medialibrarydata", 0);
    }
    return tokenCaller == mediaLibraryTokenId_;
}

int SecCompService::Dump(int fd, const std::vector<std::u16string>& args)
{
    if (fd < 0) {
        return ERR_INVALID_VALUE;
    }

    dprintf(fd, "SecCompService Dump:\n");
    std::string arg0 = ((args.size() == 0)? "" : Str16ToStr8(args.at(0)));
    if (arg0.compare("-h") == 0) {
        dprintf(fd, "Usage:\n");
        dprintf(fd, "       -h: command help\n");
        dprintf(fd, "       -a: dump all sec component\n");
        dprintf(fd, "       -p: dump foreground processes\n");
    } else if (arg0.compare("-p") == 0) {
        std::string dumpStr;
        std::unique_lock<std::mutex> lock(secCompSrvMutex_);
        appStateObserver_->DumpProcess(dumpStr);
        dprintf(fd, "%s\n", dumpStr.c_str());
    }  else if (arg0.compare("-a") == 0 || arg0 == "") {
        std::string dumpStr;
        SecCompManager::GetInstance().DumpSecComp(dumpStr);
        dprintf(fd, "%s\n", dumpStr.c_str());
    }
    return ERR_OK;
}

bool SecCompService::Initialize() const
{
    return SecCompManager::GetInstance().Initialize();
}
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
