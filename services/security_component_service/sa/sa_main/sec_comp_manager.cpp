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
#include "sec_comp_manager.h"

#include "delay_exit_task.h"
#include "hisysevent.h"
#include "i_sec_comp_service.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "sec_comp_enhance_adapter.h"
#include "sec_comp_err.h"
#include "sec_comp_info_helper.h"
#include "sec_comp_log.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "SecCompManager"};
static constexpr int32_t SC_ID_START = 1000;
static constexpr int32_t MAX_INT_NUM = 0x7fffffff;
static constexpr int32_t MAX_SINGLE_PROC_COMP_SIZE = 500;
static constexpr unsigned long REPORT_REMOTE_OBJECT_SIZE = 2UL;
}

SecCompManager::SecCompManager()
{
    scIdStart_ = SC_ID_START;
}

SecCompManager& SecCompManager::GetInstance()
{
    static SecCompManager instance;
    return instance;
}

int32_t SecCompManager::CreateScId()
{
    std::lock_guard<std::mutex> lock(scIdMtx_);
    if (scIdStart_ == MAX_INT_NUM) {
        scIdStart_ = SC_ID_START;
    } else {
        scIdStart_++;
    }
    return scIdStart_;
}

int32_t SecCompManager::AddSecurityComponentToList(int32_t pid,
    AccessToken::AccessTokenID tokenId, std::shared_ptr<SecCompEntity> newEntity)
{
    OHOS::Utils::UniqueWriteGuard<OHOS::Utils::RWLock> lk(this->componentInfoLock_);
    if (isSaExit_) {
        SC_LOG_ERROR(LABEL, "SA is exiting, retry...");
        return SC_SERVICE_ERROR_SERVICE_NOT_EXIST;
    }

    auto iter = componentMap_.find(pid);
    if (iter != componentMap_.end()) {
        if (iter->second.compList.size() > MAX_SINGLE_PROC_COMP_SIZE) {
            SC_LOG_ERROR(LABEL, "single proccess has too many component.");
            return SC_SERVICE_ERROR_VALUE_INVALID;
        }
        iter->second.isForeground = true;
        iter->second.compList.emplace_back(newEntity);
        SecCompEnhanceAdapter::EnableInputEnhance();
        DelayExitTask::GetInstance().Stop();
        return SC_OK;
    }

    ProcessCompInfos newProcess;
    newProcess.isForeground = true;
    newProcess.tokenId = tokenId;
    newProcess.compList.emplace_back(newEntity);
    componentMap_[pid] = newProcess;
    SecCompEnhanceAdapter::EnableInputEnhance();
    DelayExitTask::GetInstance().Stop();
    return SC_OK;
}

int32_t SecCompManager::DeleteSecurityComponentFromList(int32_t pid, int32_t scId)
{
    OHOS::Utils::UniqueWriteGuard<OHOS::Utils::RWLock> lk(this->componentInfoLock_);
    auto iter = componentMap_.find(pid);
    if (iter == componentMap_.end()) {
        SC_LOG_ERROR(LABEL, "Can not find registered process");
        return SC_SERVICE_ERROR_COMPONENT_NOT_EXIST;
    }
    auto& list = iter->second.compList;
    for (auto it = list.begin(); it != list.end(); ++it) {
        std::shared_ptr<SecCompEntity> sc = *it;
        if (sc == nullptr) {
            SC_LOG_ERROR(LABEL, "Secomp entity is nullptr");
            continue;
        }
        if (sc->scId_ == scId) {
            list.erase(it);
            if (!IsForegroundCompExist()) {
                SecCompEnhanceAdapter::DisableInputEnhance();
            }
            DelayExitTask::GetInstance().Start();
            return SC_OK;
        }
    }
    SC_LOG_ERROR(LABEL, "Can not find component");
    return SC_SERVICE_ERROR_COMPONENT_NOT_EXIST;
}

static std::string TransformCallBackResult(enum SCErrCode error)
{
    std::string errMsg = "";
    switch (error) {
        case SC_ENHANCE_ERROR_NOT_EXIST_ENHANCE:
            errMsg = "enhance do not exist";
            break;
        case SC_ENHANCE_ERROR_VALUE_INVALID:
            errMsg = "value is invalid";
            break;
        case SC_ENHANCE_ERROR_CALLBACK_NOT_EXIST:
            errMsg = "callback do not exist";
            break;
        case SC_ENHANCE_ERROR_CALLBACK_OPER_FAIL:
            errMsg = "callback operate fail";
            break;
        case SC_SERVICE_ERROR_COMPONENT_INFO_INVALID:
            errMsg = "component info invalid";
            break;
        case SC_ENHANCE_ERROR_CALLBACK_CHECK_FAIL:
            errMsg = "callback check fail";
            break;
        default:
            errMsg = "unknown error";
            break;
    }
    return errMsg;
}

std::shared_ptr<SecCompEntity> SecCompManager::GetSecurityComponentFromList(int32_t pid, int32_t scId)
{
    auto iter = componentMap_.find(pid);
    if (iter == componentMap_.end()) {
        return nullptr;
    }
    auto& list = iter->second.compList;
    for (auto it = list.begin(); it != list.end(); ++it) {
        std::shared_ptr<SecCompEntity> sc = *it;
        if (sc == nullptr) {
            SC_LOG_ERROR(LABEL, "Secomp entity is nullptr");
            continue;
        }
        if (sc->scId_ == scId) {
            return *it;
        }
    }
    return nullptr;
}

bool SecCompManager::IsForegroundCompExist()
{
    return std::any_of(componentMap_.begin(), componentMap_.end(), [](const auto & iter) {
        return (iter.second.isForeground) && (iter.second.compList.size() > 0);
    });
}

bool SecCompManager::IsCompExist()
{
    return std::any_of(componentMap_.begin(), componentMap_.end(), [](const auto & iter) {
        return (iter.second.compList.size() > 0);
    });
}

void SecCompManager::NotifyProcessForeground(int32_t pid)
{
    OHOS::Utils::UniqueWriteGuard<OHOS::Utils::RWLock> lk(this->componentInfoLock_);
    auto iter = componentMap_.find(pid);
    if (iter == componentMap_.end()) {
        return;
    }
    SecCompPermManager::GetInstance().CancelAppRevokingPermisions(iter->second.tokenId);
    iter->second.isForeground = true;
    if (IsForegroundCompExist()) {
        SecCompEnhanceAdapter::EnableInputEnhance();
    }

    SC_LOG_INFO(LABEL, "App pid %{public}d to foreground", pid);
}

void SecCompManager::NotifyProcessBackground(int32_t pid)
{
    OHOS::Utils::UniqueWriteGuard<OHOS::Utils::RWLock> lk(this->componentInfoLock_);
    auto iter = componentMap_.find(pid);
    if (iter == componentMap_.end()) {
        return;
    }

    FirstUseDialog::GetInstance().RemoveDialogWaitEntitys(pid);

    SecCompPermManager::GetInstance().RevokeAppPermisionsDelayed(iter->second.tokenId);
    iter->second.isForeground = false;
    if (!IsForegroundCompExist()) {
        SecCompEnhanceAdapter::DisableInputEnhance();
    }
    SC_LOG_INFO(LABEL, "App pid %{public}d to background", pid);
}

void SecCompManager::NotifyProcessDied(int32_t pid)
{
    // notify enhance process died.
    SecCompEnhanceAdapter::NotifyProcessDied(pid);

    malicious_.RemoveAppFromMaliciousAppList(pid);
    OHOS::Utils::UniqueWriteGuard<OHOS::Utils::RWLock> lk(this->componentInfoLock_);
    auto iter = componentMap_.find(pid);
    if (iter == componentMap_.end()) {
        return;
    }

    FirstUseDialog::GetInstance().RemoveDialogWaitEntitys(pid);

    SC_LOG_INFO(LABEL, "App pid %{public}d died", pid);
    iter->second.compList.clear();
    SecCompPermManager::GetInstance().RevokeAppPermissions(iter->second.tokenId);
    SecCompPermManager::GetInstance().RevokeTempSavePermission(iter->second.tokenId);
    componentMap_.erase(pid);

    if (!IsForegroundCompExist()) {
        SecCompEnhanceAdapter::DisableInputEnhance();
    }

    DelayExitTask::GetInstance().Start();
}

void SecCompManager::ExitSaProcess()
{
    OHOS::Utils::UniqueReadGuard<OHOS::Utils::RWLock> lk(this->componentInfoLock_);
    if (IsCompExist()) {
        SC_LOG_INFO(LABEL, "Apps using security component still exist, no exit sa");
        return;
    }

    isSaExit_ = true;
    SecCompEnhanceAdapter::DisableInputEnhance();
    SecCompEnhanceAdapter::ExitEnhanceService();

    SC_LOG_INFO(LABEL, "All processes using security component died, start sa exit");
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityMgr == nullptr) {
        SC_LOG_ERROR(LABEL, "Failed to get SystemAbilityManager.");
        return;
    }
    int32_t ret = systemAbilityMgr->UnloadSystemAbility(SA_ID_SECURITY_COMPONENT_SERVICE);
    if (ret != SC_OK) {
        SC_LOG_ERROR(LABEL, "Failed to UnloadSystemAbility service! errcode=%{public}d", ret);
        return;
    }
    SC_LOG_INFO(LABEL, "UnloadSystemAbility successfully!");
}

void SecCompManager::ExitWhenAppMgrDied()
{
    OHOS::Utils::UniqueWriteGuard<OHOS::Utils::RWLock> lk(this->componentInfoLock_);
    for (auto iter = componentMap_.begin(); iter != componentMap_.end(); ++iter) {
        iter->second.compList.clear();
        SecCompPermManager::GetInstance().RevokeAppPermissions(iter->second.tokenId);
        SecCompPermManager::GetInstance().RevokeTempSavePermission(iter->second.tokenId);
    }
    componentMap_.clear();

    // no need exit enhance service, only disable input enhance.
    SecCompEnhanceAdapter::DisableInputEnhance();
    isSaExit_ = true;

    SC_LOG_INFO(LABEL, "app mgr died, start sa exit");
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityMgr == nullptr) {
        SC_LOG_ERROR(LABEL, "failed to get SystemAbilityManager.");
        return;
    }
    int32_t ret = systemAbilityMgr->UnloadSystemAbility(SA_ID_SECURITY_COMPONENT_SERVICE);
    if (ret != SC_OK) {
        SC_LOG_ERROR(LABEL, "failed to UnloadSystemAbility service! errcode=%{public}d", ret);
        return;
    }
    SC_LOG_INFO(LABEL, "UnloadSystemAbility successfully!");
}

void SecCompManager::SendCheckInfoEnhanceSysEvent(int32_t scId,
    SecCompType type, const std::string& scene, int32_t res)
{
    if (res == SC_ENHANCE_ERROR_CHALLENGE_CHECK_FAIL) {
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::SEC_COMPONENT, "CHALLENGE_CHECK_FAILED",
            HiviewDFX::HiSysEvent::EventType::SECURITY, "CALLER_UID", IPCSkeleton::GetCallingUid(),
            "CALLER_PID", IPCSkeleton::GetCallingPid(), "SC_ID", scId, "SC_TYPE", type, "CALL_SCENE",
            scene);
    } else {
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::SEC_COMPONENT, "CALLBACK_FAILED",
            HiviewDFX::HiSysEvent::EventType::SECURITY, "CALLER_UID", IPCSkeleton::GetCallingUid(),
            "CALLER_PID", IPCSkeleton::GetCallingPid(), "SC_TYPE", type,
            "CALL_SCENE", scene, "REASON", TransformCallBackResult(static_cast<enum SCErrCode>(res)));
    }
}

int32_t SecCompManager::AddSecurityComponentProcess(const SecCompCallerInfo& caller)
{
    DelayExitTask::GetInstance().Stop();
    {
        OHOS::Utils::UniqueWriteGuard<OHOS::Utils::RWLock> lk(this->componentInfoLock_);
        if (isSaExit_) {
            SC_LOG_ERROR(LABEL, "SA is exiting, retry...");
            return SC_SERVICE_ERROR_SERVICE_NOT_EXIST;
        }

        auto iter = componentMap_.find(caller.pid);
        if (iter == componentMap_.end()) {
            ProcessCompInfos newProcess;
            newProcess.isForeground = true;
            newProcess.tokenId = caller.tokenId;
            componentMap_[caller.pid] = newProcess;
        }
        SecCompEnhanceAdapter::EnableInputEnhance();
    }
    SecCompEnhanceAdapter::AddSecurityComponentProcess(caller.pid);
    return SC_OK;
}

int32_t SecCompManager::RegisterSecurityComponent(SecCompType type,
    const nlohmann::json& jsonComponent, const SecCompCallerInfo& caller, int32_t& scId)
{
    SC_LOG_DEBUG(LABEL, "PID: %{public}d, register security component", caller.pid);
    if (malicious_.IsInMaliciousAppList(caller.pid, caller.uid)) {
        SC_LOG_ERROR(LABEL, "app is in MaliciousAppList, never allow it");
        return SC_ENHANCE_ERROR_IN_MALICIOUS_LIST;
    }

    SecCompBase* componentPtr = SecCompInfoHelper::ParseComponent(type, jsonComponent);
    std::shared_ptr<SecCompBase> component(componentPtr);
    if (component == nullptr) {
        SC_LOG_ERROR(LABEL, "Parse component info invalid");
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::SEC_COMPONENT, "COMPONENT_INFO_CHECK_FAILED",
            HiviewDFX::HiSysEvent::EventType::SECURITY, "CALLER_UID", IPCSkeleton::GetCallingUid(),
            "CALLER_PID", IPCSkeleton::GetCallingPid(), "SC_ID", scId, "CALL_SCENE", "REGITSTER", "SC_TYPE", type);
        return SC_SERVICE_ERROR_COMPONENT_INFO_INVALID;
    }

    int32_t enhanceRes =
        SecCompEnhanceAdapter::CheckComponentInfoEnhance(caller.pid, component, jsonComponent);
    if (enhanceRes != SC_OK) {
        SendCheckInfoEnhanceSysEvent(INVALID_SC_ID, type, "REGISTER", enhanceRes);
        SC_LOG_ERROR(LABEL, "enhance check failed");
        malicious_.AddAppToMaliciousAppList(caller.pid);
        return enhanceRes;
    }

    int32_t registerId = CreateScId();
    std::shared_ptr<SecCompEntity> entity =
        std::make_shared<SecCompEntity>(component, caller.tokenId, registerId, caller.pid, caller.uid);
    int32_t ret = AddSecurityComponentToList(caller.pid, caller.tokenId, entity);
    if (ret == SC_OK) {
        scId = registerId;
    } else {
        SC_LOG_ERROR(LABEL, "Register security component failed");
        scId = INVALID_SC_ID;
    }
    return ret;
}

int32_t SecCompManager::UpdateSecurityComponent(int32_t scId, const nlohmann::json& jsonComponent,
    const SecCompCallerInfo& caller)
{
    SC_LOG_DEBUG(LABEL, "PID: %{public}d, update security component", caller.pid);
    if (malicious_.IsInMaliciousAppList(caller.pid, caller.uid)) {
        SC_LOG_ERROR(LABEL, "app is in MaliciousAppList, never allow it");
        return SC_ENHANCE_ERROR_IN_MALICIOUS_LIST;
    }

    OHOS::Utils::UniqueWriteGuard<OHOS::Utils::RWLock> lk(this->componentInfoLock_);
    std::shared_ptr<SecCompEntity> sc = GetSecurityComponentFromList(caller.pid, scId);
    if (sc == nullptr) {
        SC_LOG_ERROR(LABEL, "Can not find target component");
        return SC_SERVICE_ERROR_COMPONENT_NOT_EXIST;
    }
    SecCompBase* report = SecCompInfoHelper::ParseComponent(sc->GetType(), jsonComponent);
    std::shared_ptr<SecCompBase> reportComponentInfo(report);
    if (reportComponentInfo == nullptr) {
        SC_LOG_ERROR(LABEL, "Update component info invalid");
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::SEC_COMPONENT, "COMPONENT_INFO_CHECK_FAILED",
            HiviewDFX::HiSysEvent::EventType::SECURITY, "CALLER_UID", IPCSkeleton::GetCallingUid(),
            "CALLER_PID", IPCSkeleton::GetCallingPid(), "SC_ID", scId, "CALL_SCENE", "UPDATE",
            "SC_TYPE", sc->GetType());
        return SC_SERVICE_ERROR_COMPONENT_INFO_INVALID;
    }

    int32_t enhanceRes =
        SecCompEnhanceAdapter::CheckComponentInfoEnhance(caller.pid, reportComponentInfo, jsonComponent);
    if (enhanceRes != SC_OK) {
        SendCheckInfoEnhanceSysEvent(scId, sc->GetType(), "UPDATE", enhanceRes);
        SC_LOG_ERROR(LABEL, "enhance check failed");
        malicious_.AddAppToMaliciousAppList(caller.pid);
        return enhanceRes;
    }

    sc->componentInfo_ = reportComponentInfo;
    return SC_OK;
}

int32_t SecCompManager::UnregisterSecurityComponent(int32_t scId, const SecCompCallerInfo& caller)
{
    SC_LOG_DEBUG(LABEL, "PID: %{public}d, unregister security component", caller.pid);
    if (scId < 0) {
        SC_LOG_ERROR(LABEL, "ScId is invalid");
        return SC_SERVICE_ERROR_VALUE_INVALID;
    }
    return DeleteSecurityComponentFromList(caller.pid, scId);
}

int32_t SecCompManager::CheckClickSecurityComponentInfo(std::shared_ptr<SecCompEntity> sc, int32_t scId,
    const nlohmann::json& jsonComponent, const SecCompCallerInfo& caller)
{
    SC_LOG_DEBUG(LABEL, "PID: %{public}d, Check security component", caller.pid);
    SecCompBase* report = SecCompInfoHelper::ParseComponent(sc->GetType(), jsonComponent);
    std::shared_ptr<SecCompBase> reportComponentInfo(report);
    if ((reportComponentInfo == nullptr) || (!reportComponentInfo->GetValid())) {
        SC_LOG_ERROR(LABEL, "report component info invalid");
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::SEC_COMPONENT, "COMPONENT_INFO_CHECK_FAILED",
            HiviewDFX::HiSysEvent::EventType::SECURITY, "CALLER_UID", IPCSkeleton::GetCallingUid(),
            "CALLER_PID", IPCSkeleton::GetCallingPid(), "SC_ID", scId, "CALL_SCENE", "CLICK", "SC_TYPE",
            sc->GetType());
        return SC_SERVICE_ERROR_COMPONENT_INFO_INVALID;
    }
    if ((!SecCompInfoHelper::CheckRectValid(reportComponentInfo->rect_, reportComponentInfo->windowRect_))) {
        SC_LOG_ERROR(LABEL, "compare component info failed.");
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::SEC_COMPONENT, "COMPONENT_INFO_CHECK_FAILED",
            HiviewDFX::HiSysEvent::EventType::SECURITY, "CALLER_UID", IPCSkeleton::GetCallingUid(),
            "CALLER_PID", IPCSkeleton::GetCallingPid(), "SC_ID", scId, "CALL_SCENE", "CLICK", "SC_TYPE",
            sc->GetType());
        return SC_SERVICE_ERROR_COMPONENT_INFO_INVALID;
    }
    int32_t enhanceRes =
        SecCompEnhanceAdapter::CheckComponentInfoEnhance(caller.pid, reportComponentInfo, jsonComponent);
    if (enhanceRes != SC_OK) {
        SendCheckInfoEnhanceSysEvent(scId, sc->GetType(), "CLICK", enhanceRes);
        SC_LOG_ERROR(LABEL, "enhance check failed");
        malicious_.AddAppToMaliciousAppList(caller.pid);
        return enhanceRes;
    }

    sc->componentInfo_ = reportComponentInfo;
    return SC_OK;
}

int32_t SecCompManager::ReportSecurityComponentClickEvent(int32_t scId,
    const nlohmann::json& jsonComponent, const SecCompCallerInfo& caller,
    const SecCompClickEvent& clickInfo, const std::vector<sptr<IRemoteObject>>& remote)
{
    if (remote.size() < REPORT_REMOTE_OBJECT_SIZE) {
        return SC_SERVICE_ERROR_VALUE_INVALID;
    }
    auto callerToken = remote[0];
    auto dialogCallback = remote[1];

    if (malicious_.IsInMaliciousAppList(caller.pid, caller.uid)) {
        SC_LOG_ERROR(LABEL, "app is in MaliciousAppList, never allow it");
        return SC_ENHANCE_ERROR_IN_MALICIOUS_LIST;
    }

    OHOS::Utils::UniqueReadGuard<OHOS::Utils::RWLock> lk(this->componentInfoLock_);
    std::shared_ptr<SecCompEntity> sc = GetSecurityComponentFromList(caller.pid, scId);
    if (sc == nullptr) {
        SC_LOG_ERROR(LABEL, "Can not find target component");
        return SC_SERVICE_ERROR_COMPONENT_NOT_EXIST;
    }

    int32_t res = CheckClickSecurityComponentInfo(sc, scId, jsonComponent, caller);
    if (res != SC_OK) {
        return res;
    }

    res = sc->CheckClickInfo(clickInfo);
    if (res != SC_OK) {
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::SEC_COMPONENT, "CLICK_INFO_CHECK_FAILED",
            HiviewDFX::HiSysEvent::EventType::SECURITY, "CALLER_UID", IPCSkeleton::GetCallingUid(),
            "CALLER_PID", IPCSkeleton::GetCallingPid(), "SC_ID", scId, "SC_TYPE", sc->GetType());
        if (res == SC_ENHANCE_ERROR_CLICK_EXTRA_CHECK_FAIL) {
            malicious_.AddAppToMaliciousAppList(caller.pid);
        }

        return SC_SERVICE_ERROR_CLICK_EVENT_INVALID;
    }

    if (FirstUseDialog::GetInstance().NotifyFirstUseDialog(sc, callerToken, dialogCallback) ==
        SC_SERVICE_ERROR_WAIT_FOR_DIALOG_CLOSE) {
        SC_LOG_INFO(LABEL, "start dialog, onclick will be trap after dialog closed.");
        return SC_SERVICE_ERROR_WAIT_FOR_DIALOG_CLOSE;
    }

    res = sc->GrantTempPermission();
    if (res != SC_OK) {
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::SEC_COMPONENT, "TEMP_GRANT_FAILED",
            HiviewDFX::HiSysEvent::EventType::FAULT, "CALLER_UID", IPCSkeleton::GetCallingUid(),
            "CALLER_PID", IPCSkeleton::GetCallingPid(), "SC_ID", scId, "SC_TYPE", sc->GetType());
        return res;
    }
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::SEC_COMPONENT, "TEMP_GRANT_SUCCESS",
        HiviewDFX::HiSysEvent::EventType::BEHAVIOR, "CALLER_UID", IPCSkeleton::GetCallingUid(),
        "CALLER_PID", IPCSkeleton::GetCallingPid(), "SC_ID", scId, "SC_TYPE", sc->GetType());
    return res;
}

void SecCompManager::DumpSecComp(std::string& dumpStr)
{
    OHOS::Utils::UniqueReadGuard<OHOS::Utils::RWLock> lk(this->componentInfoLock_);
    for (auto iter = componentMap_.begin(); iter != componentMap_.end(); ++iter) {
        AccessToken::AccessTokenID tokenId = iter->second.tokenId;
        bool locationPerm = SecCompPermManager::GetInstance().VerifyPermission(tokenId, LOCATION_COMPONENT);
        bool pastePerm = SecCompPermManager::GetInstance().VerifyPermission(tokenId, PASTE_COMPONENT);
        bool savePerm = SecCompPermManager::GetInstance().VerifyPermission(tokenId, SAVE_COMPONENT);
        dumpStr.append("pid:" + std::to_string(iter->first) + ", tokenId:" + std::to_string(pastePerm) +
            ", locationPerm:" + std::to_string(locationPerm) + ", pastePerm:" + std::to_string(pastePerm) +
            ", savePerm:" + std::to_string(savePerm) + " \n");
        for (auto compIter = iter->second.compList.begin(); compIter != iter->second.compList.end(); ++compIter) {
            nlohmann::json json;
            std::shared_ptr<SecCompEntity> sc = *compIter;
            if (sc == nullptr || sc->componentInfo_ == nullptr) {
                continue;
            }

            sc->componentInfo_->ToJson(json);
            dumpStr.append("    scId:" + std::to_string(sc->scId_) +
                ", isGrant:" + std::to_string(sc->IsGrant()) + ", " + json.dump() + "\n");
        }
    }
}

bool SecCompManager::Initialize()
{
    SC_LOG_DEBUG(LABEL, "Initialize!!");
    SecCompEnhanceAdapter::StartEnhanceService();

    secRunner_ = AppExecFwk::EventRunner::Create(true);
    if (!secRunner_) {
        SC_LOG_ERROR(LABEL, "failed to create a recvRunner.");
        return false;
    }

    secHandler_ = std::make_shared<SecEventHandler>(secRunner_);
    DelayExitTask::GetInstance().Init(secHandler_);
    FirstUseDialog::GetInstance().Init(secHandler_);
    SecCompEnhanceAdapter::EnableInputEnhance();

    return SecCompPermManager::GetInstance().InitEventHandler(secHandler_);
}
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
