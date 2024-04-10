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
#include "hisysevent.h"
#include "hitrace_meter.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
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
    SC_LOG_ERROR(LABEL, "Register app state observer success");
    return true;
}

void SecCompService::UnregisterAppStateObserver()
{
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

int32_t SecCompService::RegisterSecurityComponent(SecCompType type,
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
    return res;
}

int32_t SecCompService::UpdateSecurityComponent(int32_t scId, const std::string& componentInfo)
{
    SecCompCallerInfo caller;
    nlohmann::json jsonRes;
    if (ParseParams(componentInfo, caller, jsonRes) != SC_OK) {
        return SC_SERVICE_ERROR_VALUE_INVALID;
    }
    return SecCompManager::GetInstance().UpdateSecurityComponent(scId, jsonRes, caller);
}

int32_t SecCompService::UnregisterSecurityComponent(int32_t scId)
{
    SecCompCallerInfo caller;
    caller.tokenId = IPCSkeleton::GetCallingTokenID();
    caller.pid = IPCSkeleton::GetCallingPid();
    caller.uid = IPCSkeleton::GetCallingUid();

    return SecCompManager::GetInstance().UnregisterSecurityComponent(scId, caller);
}

int32_t SecCompService::ReportSecurityComponentClickEvent(int32_t scId,
    const std::string& componentInfo, const SecCompClickEvent& clickInfo,
    sptr<IRemoteObject> callerToken, sptr<IRemoteObject> dialogCallback)
{
    StartTrace(HITRACE_TAG_ACCESS_CONTROL, "SecurityComponentClick");
    SecCompCallerInfo caller;
    nlohmann::json jsonRes;
    if (ParseParams(componentInfo, caller, jsonRes) != SC_OK) {
        FinishTrace(HITRACE_TAG_ACCESS_CONTROL);
        return SC_SERVICE_ERROR_VALUE_INVALID;
    }
    std::vector<sptr<IRemoteObject>> remoteArr = { callerToken, dialogCallback };
    int32_t res =
        SecCompManager::GetInstance().ReportSecurityComponentClickEvent(scId, jsonRes, caller, clickInfo,
        remoteArr);
    FinishTrace(HITRACE_TAG_ACCESS_CONTROL);
    return res;
}

int32_t SecCompService::PreRegisterSecCompProcess()
{
    SecCompCallerInfo caller;
    if (!GetCallerInfo(caller)) {
        SC_LOG_ERROR(LABEL, "Check caller failed");
        return SC_SERVICE_ERROR_VALUE_INVALID;
    }
    return SecCompManager::GetInstance().AddSecurityComponentProcess(caller);
}

bool SecCompService::VerifySavePermission(AccessToken::AccessTokenID tokenId)
{
    return SecCompPermManager::GetInstance().VerifySavePermission(tokenId);
}

sptr<IRemoteObject> SecCompService::GetEnhanceRemoteObject()
{
    return SecCompEnhanceAdapter::GetEnhanceRemoteObject();
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
