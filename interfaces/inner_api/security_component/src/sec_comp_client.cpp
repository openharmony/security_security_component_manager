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

#include "iservice_registry.h"
#include "sec_comp_load_callback.h"
#include "sec_comp_log.h"
#include "sec_comp_proxy.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "SecCompClient"};
}  // namespace

SecCompClient& SecCompClient::GetInstance()
{
    static SecCompClient instance;
    return instance;
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

int32_t SecCompClient::RegisterSecurityComponent(SecCompType type,
    const std::string& componentInfo, int32_t& scId)
{
    auto proxy = GetProxy(true);
    if (proxy == nullptr) {
        SC_LOG_ERROR(LABEL, "Proxy is null");
        return SC_SERVICE_ERROR_VALUE_INVALID;
    }

    return proxy->RegisterSecurityComponent(type, componentInfo, scId);
}

int32_t SecCompClient::UpdateSecurityComponent(int32_t scId, const std::string& componentInfo)
{
    auto proxy = GetProxy(true);
    if (proxy == nullptr) {
        SC_LOG_ERROR(LABEL, "Proxy is null");
        return SC_SERVICE_ERROR_VALUE_INVALID;
    }

    return proxy->UpdateSecurityComponent(scId, componentInfo);
}

int32_t SecCompClient::UnregisterSecurityComponent(int32_t scId)
{
    auto proxy = GetProxy(true);
    if (proxy == nullptr) {
        SC_LOG_ERROR(LABEL, "Proxy is null");
        return SC_SERVICE_ERROR_VALUE_INVALID;
    }

    return proxy->UnregisterSecurityComponent(scId);
}

int32_t SecCompClient::ReportSecurityComponentClickEvent(int32_t scId,
    const std::string& componentInfo, const SecCompClickEvent& clickInfo,
    sptr<IRemoteObject> callerToken, sptr<IRemoteObject> dialogCallback)
{
    auto proxy = GetProxy(true);
    if (proxy == nullptr) {
        SC_LOG_ERROR(LABEL, "Proxy is null");
        return SC_SERVICE_ERROR_VALUE_INVALID;
    }

    return proxy->ReportSecurityComponentClickEvent(scId, componentInfo, clickInfo, callerToken, dialogCallback);
}

bool SecCompClient::VerifySavePermission(AccessToken::AccessTokenID tokenId)
{
    auto proxy = GetProxy(false);
    if (proxy == nullptr) {
        SC_LOG_ERROR(LABEL, "Proxy is null");
        return false;
    }

    return proxy->VerifySavePermission(tokenId);
}

sptr<IRemoteObject> SecCompClient::GetEnhanceRemoteObject(bool doLoadSa)
{
    auto proxy = GetProxy(doLoadSa);
    if (proxy == nullptr) {
        return nullptr;
    }

    return proxy->GetEnhanceRemoteObject();
}

int32_t SecCompClient::PreRegisterSecCompProcess()
{
    auto proxy = GetProxy(true);
    if (proxy == nullptr) {
        SC_LOG_ERROR(LABEL, "Proxy is null");
        return SC_SERVICE_ERROR_VALUE_INVALID;
    }

    return proxy->PreRegisterSecCompProcess();
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
        lock, std::chrono::milliseconds(SA_ID_SECURITY_COMPONENT_SERVICE), [this]() { return readyFlag_; });
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
    std::unique_lock<std::mutex> lock(cvLock_);
    readyFlag_ = true;
    secComCon_.notify_one();
}

void SecCompClient::FinishStartSAFail()
{
    SC_LOG_ERROR(LABEL, "get security component sa failed.");
    // get lock which wait_for release and send a notice
    std::unique_lock<std::mutex> lock(cvLock_);
    readyFlag_ = true;
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
    proxy_ = nullptr;
    serviceDeathObserver_ = nullptr;
    {
        std::unique_lock<std::mutex> lock1(cvLock_);
        readyFlag_ = false;
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
