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
#include "sec_comp_client.h"

#include "accesstoken_kit.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "sec_comp_click_event_parcel.h"
#include "sec_comp_load_callback.h"
#include "sec_comp_log.h"
#include "sec_comp_service_proxy.h"
#include "sys_binder.h"
#include "tokenid_kit.h"
#include <algorithm>
#include <chrono>

namespace OHOS {
namespace Security {
namespace SecurityComponent {
namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "SecCompClient"};
constexpr int32_t SA_ID_SECURITY_COMPONENT_SERVICE = 3506;
static std::mutex g_instanceMutex;
static constexpr int32_t SENDREQ_FAIL_ERR = 32;
static const std::vector<int32_t> RETRY_CODE_LIST = {
    SC_SERVICE_ERROR_SERVICE_NOT_EXIST, BR_DEAD_REPLY, BR_FAILED_REPLY, SENDREQ_FAIL_ERR };
static constexpr int32_t SA_DIED_TIME_OUT = 500;
constexpr int32_t SA_LOAD_TIME_OUT = 3000;
}  // namespace

SecCompClient& SecCompClient::GetInstance()
{
    static SecCompClient* instance = nullptr;
    if (instance == nullptr) {
        std::lock_guard<std::mutex> lock(g_instanceMutex);
        if (instance == nullptr) {
            SecCompClient* tmp = new (std::nothrow)SecCompClient();
            instance = std::move(tmp);
        }
    }
    return *instance;
}

SecCompClient::SecCompClient()
{}

SecCompClient::~SecCompClient()
{
    if (proxy_ == nullptr) {
        return;
    }
    auto remoteObj = proxy_->AsObject();
    if ((remoteObj != nullptr) && (serviceDeathObserver_ != nullptr)) {
        remoteObj->RemoveDeathRecipient(serviceDeathObserver_);
    }
}

int32_t SecCompClient::RegisterWriteToRawdata(SecCompType type, const std::string& componentInfo,
    SecCompRawdata& rawData)
{
    MessageParcel dataParcel;

    if (!dataParcel.WriteUint32(type)) {
        SC_LOG_ERROR(LABEL, "Register write type failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (!dataParcel.WriteString(componentInfo)) {
        SC_LOG_ERROR(LABEL, "Register write componentInfo failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (!SecCompEnhanceAdapter::EnhanceClientSerialize(dataParcel, rawData)) {
        SC_LOG_ERROR(LABEL, "Register serialize session info failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }
    return SC_OK;
}

int32_t SecCompClient::TryRegisterSecurityComponent(SecCompType type, const std::string& componentInfo,
    int32_t& scId, sptr<ISecCompService> proxy)
{
    std::lock_guard<std::mutex> lock(useIPCMutex_);
    SecCompRawdata rawData;
    if (RegisterWriteToRawdata(type, componentInfo, rawData) != SC_OK) {
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    SecCompRawdata rawReply;
    int32_t res = proxy->RegisterSecurityComponent(rawData, rawReply);
    MessageParcel deserializedReply;

    if (res != SC_OK) {
        SC_LOG_ERROR(LABEL, "Register request failed, result: %{public}d.", res);
        return res;
    }

    if (!SecCompEnhanceAdapter::EnhanceClientDeserialize(rawReply, deserializedReply)) {
        SC_LOG_ERROR(LABEL, "Register deserialize session info failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    int32_t serviceRes;
    if (!deserializedReply.ReadInt32(serviceRes)) {
        SC_LOG_ERROR(LABEL, "Register read serviceRes failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (serviceRes != SC_OK) {
        scId = INVALID_SC_ID;
        return serviceRes;
    }

    if (!deserializedReply.ReadInt32(scId)) {
        SC_LOG_ERROR(LABEL, "Register read scId failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }
    return res;
}

int32_t SecCompClient::RegisterSecurityComponent(SecCompType type,
    const std::string& componentInfo, int32_t& scId)
{
    auto proxy = GetProxy(true);
    if (proxy == nullptr) {
        SC_LOG_ERROR(LABEL, "Proxy is null.");
        return SC_SERVICE_ERROR_VALUE_INVALID;
    }

    auto res = TryRegisterSecurityComponent(type, componentInfo, scId, proxy);
    if (std::find(RETRY_CODE_LIST.begin(), RETRY_CODE_LIST.end(), res) == RETRY_CODE_LIST.end()) {
        return res;
    }

    std::unique_lock<std::mutex> lock(secCompSaMutex_);
    auto waitStatus = secCompSACon_.wait_for(lock, std::chrono::milliseconds(SA_DIED_TIME_OUT),
        [this]() { return secCompSAFlag_; });
    if (waitStatus) {
        proxy = GetProxy(true);
        return TryRegisterSecurityComponent(type, componentInfo, scId, proxy);
    }
    return res;
}

int32_t SecCompClient::UpdateWriteToRawdata(int32_t scId, const std::string& componentInfo, SecCompRawdata& rawData)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInt32(scId)) {
        SC_LOG_ERROR(LABEL, "Update write scId failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }
    if (!dataParcel.WriteString(componentInfo)) {
        SC_LOG_ERROR(LABEL, "Update write componentInfo failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (!SecCompEnhanceAdapter::EnhanceClientSerialize(dataParcel, rawData)) {
        SC_LOG_ERROR(LABEL, "Update serialize session info failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }
    return SC_OK;
}

int32_t SecCompClient::UpdateSecurityComponent(int32_t scId, const std::string& componentInfo)
{
    auto proxy = GetProxy(true);
    if (proxy == nullptr) {
        SC_LOG_ERROR(LABEL, "Proxy is null");
        return SC_SERVICE_ERROR_VALUE_INVALID;
    }

    std::lock_guard<std::mutex> lock(useIPCMutex_);
    SecCompRawdata rawData;
    int32_t res = UpdateWriteToRawdata(scId, componentInfo, rawData);
    if (res != SC_OK) {
        return res;
    }

    SecCompRawdata rawReply;
    res = proxy->UpdateSecurityComponent(rawData, rawReply);

    MessageParcel deserializedReply;
    if (!SecCompEnhanceAdapter::EnhanceClientDeserialize(rawReply, deserializedReply)) {
        SC_LOG_ERROR(LABEL, "Update deserialize session info failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (res != SC_OK) {
        SC_LOG_ERROR(LABEL, "Update request failed, result: %{public}d.", res);
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    int32_t serviceRes;
    if (!deserializedReply.ReadInt32(serviceRes)) {
        SC_LOG_ERROR(LABEL, "Update read res failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    return serviceRes;
}

int32_t SecCompClient::UnregisterWriteToRawdata(int32_t scId, SecCompRawdata& rawData)
{
    MessageParcel dataParcel;

    if (!dataParcel.WriteInt32(scId)) {
        SC_LOG_ERROR(LABEL, "Unregister write scId failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (!SecCompEnhanceAdapter::EnhanceClientSerialize(dataParcel, rawData)) {
        SC_LOG_ERROR(LABEL, "Unregister serialize session info failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }
    return SC_OK;
}

int32_t SecCompClient::UnregisterSecurityComponent(int32_t scId)
{
    auto proxy = GetProxy(true);
    if (proxy == nullptr) {
        SC_LOG_ERROR(LABEL, "Proxy is null");
        return SC_SERVICE_ERROR_VALUE_INVALID;
    }

    std::lock_guard<std::mutex> lock(useIPCMutex_);
    SecCompRawdata rawData;
    int32_t res = UnregisterWriteToRawdata(scId, rawData);
    if (res != SC_OK) {
        return res;
    }

    SecCompRawdata rawReply;
    res = proxy->UnregisterSecurityComponent(rawData, rawReply);

    MessageParcel deserializedReply;
    if (!SecCompEnhanceAdapter::EnhanceClientDeserialize(rawReply, deserializedReply)) {
        SC_LOG_ERROR(LABEL, "Unregister deserialize session info failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (res != SC_OK) {
        SC_LOG_ERROR(LABEL, "Unregister request failed, result: %{public}d.", res);
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    int32_t serviceRes;
    if (!deserializedReply.ReadInt32(serviceRes)) {
        SC_LOG_ERROR(LABEL, "Unregister read res failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    return serviceRes;
}

int32_t SecCompClient::ReportWriteToRawdata(SecCompInfo& secCompInfo, SecCompRawdata& rawData, std::string& message)
{
    MessageParcel dataParcel;

    if (!dataParcel.WriteInt32(secCompInfo.scId)) {
        SC_LOG_ERROR(LABEL, "Report write scId failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (!dataParcel.WriteString(secCompInfo.componentInfo)) {
        SC_LOG_ERROR(LABEL, "Report write componentInfo failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (!dataParcel.WriteString(message)) {
        SC_LOG_ERROR(LABEL, "Report write message failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    sptr<SecCompClickEventParcel> parcel = new (std::nothrow) SecCompClickEventParcel();
    if (parcel == nullptr) {
        SC_LOG_ERROR(LABEL, "Report new click event parcel failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }
    parcel->clickInfoParams_ = secCompInfo.clickInfo;
    if (!dataParcel.WriteParcelable(parcel)) {
        SC_LOG_ERROR(LABEL, "Report write clickInfo failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (!SecCompEnhanceAdapter::EnhanceClientSerialize(dataParcel, rawData)) {
        SC_LOG_ERROR(LABEL, "Unregister serialize session info failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }
    return SC_OK;
}

int32_t SecCompClient::ReportSecurityComponentClickEvent(SecCompInfo& secCompInfo,
    sptr<IRemoteObject> callerToken, sptr<IRemoteObject> dialogCallback, std::string& message)
{
    auto proxy = GetProxy(true);
    if (proxy == nullptr) {
        SC_LOG_ERROR(LABEL, "Proxy is null");
        return SC_SERVICE_ERROR_VALUE_INVALID;
    }

    std::lock_guard<std::mutex> lock(useIPCMutex_);
    SecCompRawdata rawData;
    int32_t res = ReportWriteToRawdata(secCompInfo, rawData, message);
    if (res != SC_OK) {
        return res;
    }

    SecCompRawdata rawReply;
    res = proxy->ReportSecurityComponentClickEvent(callerToken, dialogCallback, rawData, rawReply);
    MessageParcel deserializedReply;
    if (!SecCompEnhanceAdapter::EnhanceClientDeserialize(rawReply, deserializedReply)) {
        SC_LOG_ERROR(LABEL, "Report deserialize session info failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (res != SC_OK) {
        SC_LOG_ERROR(LABEL, "Report request failed, result: %{public}d.", res);
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    int32_t serviceRes;
    if (!deserializedReply.ReadInt32(serviceRes)) {
        SC_LOG_ERROR(LABEL, "Report read res failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (!deserializedReply.ReadString(message)) {
        SC_LOG_ERROR(LABEL, "Report read error message failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }
    return serviceRes;
}

bool SecCompClient::VerifySavePermission(AccessToken::AccessTokenID tokenId)
{
    auto proxy = GetProxy(false);
    if (proxy == nullptr) {
        SC_LOG_ERROR(LABEL, "Proxy is null");
        return false;
    }

    bool isGranted;
    int32_t res = proxy->VerifySavePermission(tokenId, isGranted);
    if (res != SC_OK) {
        SC_LOG_ERROR(LABEL, "Verify save permission fail");
        return false;
    }
    return isGranted;
}

bool SecCompClient::IsSystemAppCalling()
{
    auto selfToken = IPCSkeleton::GetSelfTokenID();
    return Security::AccessToken::TokenIdKit::IsSystemAppByFullTokenID(selfToken);
}

int32_t SecCompClient::PreRegisterWriteToRawdata(SecCompRawdata& rawData)
{
    MessageParcel dataParcel;
    if (!SecCompEnhanceAdapter::EnhanceClientSerialize(dataParcel, rawData)) {
        SC_LOG_ERROR(LABEL, "PreRegister serialize session info failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    return SC_OK;
}

int32_t SecCompClient::PreRegisterSecCompProcess()
{
    auto proxy = GetProxy(true);
    if (proxy == nullptr) {
        SC_LOG_ERROR(LABEL, "Proxy is null");
        return SC_SERVICE_ERROR_VALUE_INVALID;
    }
    std::lock_guard<std::mutex> lock(useIPCMutex_);
    SecCompRawdata rawData;
    int32_t res = PreRegisterWriteToRawdata(rawData);
    if (res != SC_OK) {
        return res;
    }

    SecCompRawdata rawReply;
    res = proxy->PreRegisterSecCompProcess(rawData, rawReply);
    MessageParcel deserializedReply;
    if (!SecCompEnhanceAdapter::EnhanceClientDeserialize(rawReply, deserializedReply)) {
        SC_LOG_ERROR(LABEL, "PreRegister deserialize session info failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (res != SC_OK) {
        SC_LOG_ERROR(LABEL, "PreRegister request failed, result: %{public}d.", res);
        return res;
    }

    int32_t serviceRes;
    if (!deserializedReply.ReadInt32(serviceRes)) {
        SC_LOG_ERROR(LABEL, "PreRegister read serviceRes failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    return serviceRes;
}

bool SecCompClient::IsServiceExist()
{
    return GetProxy(false) != nullptr;
}

bool SecCompClient::LoadService()
{
    return GetProxy(true) != nullptr;
}

bool SecCompClient::StartLoadSecCompSa()
{
    {
        std::unique_lock<std::mutex> lock(cvLock_);
        readyFlag_ = false;
    }
    auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam == nullptr) {
        SC_LOG_ERROR(LABEL, "GetSystemAbilityManager return null");
        return false;
    }
    sptr<SecCompLoadCallback> ptrSecCompLoadCallback =
        new (std::nothrow) SecCompLoadCallback();
    if (ptrSecCompLoadCallback == nullptr) {
        SC_LOG_ERROR(LABEL, "New ptrSecCompLoadCallback fail.");
        return false;
    }

    int32_t result = sam->LoadSystemAbility(SA_ID_SECURITY_COMPONENT_SERVICE,
        ptrSecCompLoadCallback);
    if (result != SC_OK) {
        SC_LOG_ERROR(LABEL, "LoadSystemAbility %{public}d failed", SA_ID_SECURITY_COMPONENT_SERVICE);
        return false;
    }
    SC_LOG_INFO(LABEL, "Notify samgr load sa %{public}d, waiting for service start", SA_ID_SECURITY_COMPONENT_SERVICE);
    return true;
}

bool SecCompClient::TryToGetSecCompSa()
{
    auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam == nullptr) {
        SC_LOG_ERROR(LABEL, "GetSystemAbilityManager return null");
        return false;
    }

    auto secCompSa = sam->CheckSystemAbility(SA_ID_SECURITY_COMPONENT_SERVICE);
    if (secCompSa == nullptr) {
        SC_LOG_INFO(LABEL, "Service is not start.");
        return false;
    }
    GetProxyFromRemoteObject(secCompSa);
    return true;
}

void SecCompClient::WaitForSecCompSa()
{
    // wait_for release lock and block until time out(1s) or match the condition with notice
    std::unique_lock<std::mutex> lock(cvLock_);
    auto waitStatus = secComCon_.wait_for(
        lock, std::chrono::milliseconds(SA_LOAD_TIME_OUT), [this]() { return readyFlag_; });
    if (!waitStatus) {
        // time out or loadcallback fail
        SC_LOG_ERROR(LABEL, "security component load sa timeout");
        return;
    }
}

void SecCompClient::FinishStartSASuccess(const sptr<IRemoteObject>& remoteObject)
{
    GetProxyFromRemoteObject(remoteObject);
    // get lock which wait_for release and send a notice so that wait_for can out of block
    {
        std::unique_lock<std::mutex> lock(cvLock_);
        readyFlag_ = true;
        secComCon_.notify_one();
    }
    {
        std::unique_lock<std::mutex> lock(secCompSaMutex_);
        secCompSAFlag_ = false;
    }
}

void SecCompClient::FinishStartSAFail()
{
    SC_LOG_ERROR(LABEL, "get security component sa failed.");
    // get lock which wait_for release and send a notice
    std::unique_lock<std::mutex> lock(cvLock_);
    readyFlag_ = false;
    secComCon_.notify_one();
}

void SecCompClient::LoadSecCompSa()
{
    if (!StartLoadSecCompSa()) {
        return;
    }
    WaitForSecCompSa();
}

void SecCompClient::OnRemoteDiedHandle()
{
    SC_LOG_ERROR(LABEL, "Remote service died");
    std::unique_lock<std::mutex> lock(proxyMutex_);
    if (proxy_ != nullptr) {
        auto remoteObj = proxy_->AsObject();
        if ((remoteObj != nullptr) && (serviceDeathObserver_ != nullptr)) {
            remoteObj->RemoveDeathRecipient(serviceDeathObserver_);
        }
    }
    proxy_ = nullptr;
    serviceDeathObserver_ = nullptr;
    {
        std::unique_lock<std::mutex> lock1(cvLock_);
        readyFlag_ = false;
    }
    {
        std::unique_lock<std::mutex> lock1(secCompSaMutex_);
        secCompSAFlag_ = true;
        secCompSACon_.notify_one();
    }
}

void SecCompClient::GetProxyFromRemoteObject(const sptr<IRemoteObject>& remoteObject)
{
    if (remoteObject == nullptr) {
        return;
    }

    sptr<SecCompDeathRecipient> serviceDeathObserver = new (std::nothrow) SecCompDeathRecipient();
    if (serviceDeathObserver == nullptr) {
        SC_LOG_ERROR(LABEL, "Alloc service death observer fail");
        return;
    }

    if (!remoteObject->AddDeathRecipient(serviceDeathObserver)) {
        SC_LOG_ERROR(LABEL, "Add service death observer fail");
        return;
    }

    auto proxy = iface_cast<ISecCompService>(remoteObject);
    if (proxy == nullptr) {
        SC_LOG_ERROR(LABEL, "iface_cast get null");
        return;
    }
    proxy_ = proxy;
    serviceDeathObserver_ = serviceDeathObserver;
    SC_LOG_INFO(LABEL, "GetSystemAbility %{public}d success", SA_ID_SECURITY_COMPONENT_SERVICE);
    return;
}

sptr<ISecCompService> SecCompClient::GetProxy(bool doLoadSa)
{
    std::unique_lock<std::mutex> lock(proxyMutex_);
    if (proxy_ != nullptr) {
        return proxy_;
    }
    if (TryToGetSecCompSa() || !doLoadSa) {
        return proxy_;
    }

    LoadSecCompSa();
    return proxy_;
}
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
