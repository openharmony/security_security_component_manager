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
#include "sec_comp_proxy.h"

#include "sec_comp_click_event_parcel.h"
#include "sec_comp_enhance_adapter.h"
#include "sec_comp_err.h"
#include "sec_comp_log.h"
#include <mutex>

namespace OHOS {
namespace Security {
namespace SecurityComponent {
namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "SecCompProxy"};
}

SecCompProxy::SecCompProxy(const sptr<IRemoteObject>& impl) : IRemoteProxy<ISecCompService>(impl)
{}

SecCompProxy::~SecCompProxy()
{}

int32_t SecCompProxy::RegisterSecurityComponent(SecCompType type,
    const std::string& componentInfo, int32_t& scId)
{
    std::lock_guard<std::mutex> lock(useIPCMutex_);
    MessageParcel rawData;
    MessageParcel data;
    if (!data.WriteInterfaceToken(SecCompProxy::GetDescriptor())) {
        SC_LOG_ERROR(LABEL, "Register write descriptor failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (!rawData.WriteUint32(type)) {
        SC_LOG_ERROR(LABEL, "Register write type failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (!rawData.WriteString(componentInfo)) {
        SC_LOG_ERROR(LABEL, "Register write componentInfo failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (!SecCompEnhanceAdapter::EnhanceClientSerialize(rawData, data)) {
        SC_LOG_ERROR(LABEL, "Register serialize session info failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    MessageParcel reply;
    MessageParcel deserializedReply;
    MessageOption option(MessageOption::TF_SYNC);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        SC_LOG_ERROR(LABEL, "Register remote service is null.");
        return SC_SERVICE_ERROR_IPC_REQUEST_FAIL;
    }
    int32_t requestResult = remote->SendRequest(
        static_cast<uint32_t>(SecurityComponentServiceInterfaceCode::REGISTER_SECURITY_COMPONENT),
        data, reply, option);

    if (!SecCompEnhanceAdapter::EnhanceClientDeserialize(reply, deserializedReply)) {
        SC_LOG_ERROR(LABEL, "Register deserialize session info failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (requestResult != SC_OK) {
        SC_LOG_ERROR(LABEL, "Register request failed, result: %{public}d.", requestResult);
        return requestResult;
    }

    int32_t res;
    if (!deserializedReply.ReadInt32(res)) {
        SC_LOG_ERROR(LABEL, "Register read res failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (!deserializedReply.ReadInt32(scId)) {
        SC_LOG_ERROR(LABEL, "Register read scId failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }
    return res;
}

int32_t SecCompProxy::UpdateSecurityComponent(int32_t scId, const std::string& componentInfo)
{
    std::lock_guard<std::mutex> lock(useIPCMutex_);
    MessageParcel rawData;
    MessageParcel data;
    if (!data.WriteInterfaceToken(SecCompProxy::GetDescriptor())) {
        SC_LOG_ERROR(LABEL, "Update write descriptor failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (!rawData.WriteInt32(scId)) {
        SC_LOG_ERROR(LABEL, "Update write scId failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }
    if (!rawData.WriteString(componentInfo)) {
        SC_LOG_ERROR(LABEL, "Update write componentInfo failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (!SecCompEnhanceAdapter::EnhanceClientSerialize(rawData, data)) {
        SC_LOG_ERROR(LABEL, "Update serialize session info failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    MessageParcel reply;
    MessageParcel deserializedReply;
    MessageOption option(MessageOption::TF_SYNC);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        SC_LOG_ERROR(LABEL, "Update remote update service is null.");
        return SC_SERVICE_ERROR_IPC_REQUEST_FAIL;
    }
    int32_t requestResult = remote->SendRequest(
        static_cast<uint32_t>(SecurityComponentServiceInterfaceCode::UPDATE_SECURITY_COMPONENT), data, reply, option);

    if (!SecCompEnhanceAdapter::EnhanceClientDeserialize(reply, deserializedReply)) {
        SC_LOG_ERROR(LABEL, "Update deserialize session info failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (requestResult != SC_OK) {
        SC_LOG_ERROR(LABEL, "Update request failed, result: %{public}d.", requestResult);
        return requestResult;
    }

    int32_t res;
    if (!deserializedReply.ReadInt32(res)) {
        SC_LOG_ERROR(LABEL, "Update read res failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }
    return res;
}

int32_t SecCompProxy::UnregisterSecurityComponent(int32_t scId)
{
    std::lock_guard<std::mutex> lock(useIPCMutex_);
    MessageParcel rawData;
    MessageParcel data;
    if (!data.WriteInterfaceToken(SecCompProxy::GetDescriptor())) {
        SC_LOG_ERROR(LABEL, "Unregister write descriptor failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (!rawData.WriteInt32(scId)) {
        SC_LOG_ERROR(LABEL, "Unregister write scId failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (!SecCompEnhanceAdapter::EnhanceClientSerialize(rawData, data)) {
        SC_LOG_ERROR(LABEL, "Unregister serialize session info failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    MessageParcel reply;
    MessageParcel deserializedReply;
    MessageOption option(MessageOption::TF_SYNC);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        SC_LOG_ERROR(LABEL, "Unregister remote service is null.");
        return SC_SERVICE_ERROR_IPC_REQUEST_FAIL;
    }
    int32_t requestResult = remote->SendRequest(
        static_cast<uint32_t>(SecurityComponentServiceInterfaceCode::UNREGISTER_SECURITY_COMPONENT),
        data, reply, option);

    if (!SecCompEnhanceAdapter::EnhanceClientDeserialize(reply, deserializedReply)) {
        SC_LOG_ERROR(LABEL, "Unregister deserialize session info failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (requestResult != SC_OK) {
        SC_LOG_ERROR(LABEL, "Unregister request failed, result: %{public}d.", requestResult);
        return requestResult;
    }

    int32_t res;
    if (!deserializedReply.ReadInt32(res)) {
        SC_LOG_ERROR(LABEL, "Unregister read res failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }
    return res;
}

int32_t SecCompProxy::SendReportClickEventRequest(MessageParcel& data, std::string& message)
{
    MessageParcel reply;
    MessageParcel deserializedReply;
    MessageOption option(MessageOption::TF_SYNC);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        SC_LOG_ERROR(LABEL, "Report remote service is null.");
        return SC_SERVICE_ERROR_IPC_REQUEST_FAIL;
    }
    int32_t requestResult = remote->SendRequest(
        static_cast<uint32_t>(SecurityComponentServiceInterfaceCode::REPORT_SECURITY_COMPONENT_CLICK_EVENT),
        data, reply, option);

    if (!SecCompEnhanceAdapter::EnhanceClientDeserialize(reply, deserializedReply)) {
        SC_LOG_ERROR(LABEL, "Report deserialize session info failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (requestResult != SC_OK) {
        SC_LOG_ERROR(LABEL, "Report request failed, result: %{public}d.", requestResult);
        return requestResult;
    }

    int32_t res;
    if (!deserializedReply.ReadInt32(res)) {
        SC_LOG_ERROR(LABEL, "Report read res failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (!deserializedReply.ReadString(message)) {
        SC_LOG_ERROR(LABEL, "Report read error message failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }
    return res;
}

int32_t SecCompProxy::ReportSecurityComponentClickEvent(SecCompInfo& secCompInfo,
    sptr<IRemoteObject> callerToken, sptr<IRemoteObject> dialogCallback, std::string& message)
{
    std::lock_guard<std::mutex> lock(useIPCMutex_);
    MessageParcel rawData;
    MessageParcel data;
    if (!data.WriteInterfaceToken(SecCompProxy::GetDescriptor())) {
        SC_LOG_ERROR(LABEL, "Report write descriptor failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (!rawData.WriteInt32(secCompInfo.scId)) {
        SC_LOG_ERROR(LABEL, "Report write scId failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (!rawData.WriteString(secCompInfo.componentInfo)) {
        SC_LOG_ERROR(LABEL, "Report write componentInfo failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    sptr<SecCompClickEventParcel> parcel = new (std::nothrow) SecCompClickEventParcel();
    if (parcel == nullptr) {
        SC_LOG_ERROR(LABEL, "Report new click event parcel failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }
    parcel->clickInfoParams_ = secCompInfo.clickInfo;
    if (!rawData.WriteParcelable(parcel)) {
        SC_LOG_ERROR(LABEL, "Report write clickInfo failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if ((callerToken != nullptr) && !data.WriteRemoteObject(callerToken)) {
        SC_LOG_ERROR(LABEL, "Report write caller token failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if ((dialogCallback != nullptr) && !data.WriteRemoteObject(dialogCallback)) {
        SC_LOG_ERROR(LABEL, "Report write caller token failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (!SecCompEnhanceAdapter::EnhanceClientSerialize(rawData, data)) {
        SC_LOG_ERROR(LABEL, "Report serialize session info failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    return SendReportClickEventRequest(data, message);
}

bool SecCompProxy::VerifySavePermission(AccessToken::AccessTokenID tokenId)
{
    std::lock_guard<std::mutex> lock(useIPCMutex_);
    MessageParcel data;
    if (!data.WriteInterfaceToken(SecCompProxy::GetDescriptor())) {
        SC_LOG_ERROR(LABEL, "Verify write descriptor failed.");
        return false;
    }
    if (!data.WriteUint32(tokenId)) {
        SC_LOG_ERROR(LABEL, "Verify write tokenId failed.");
        return false;
    }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        SC_LOG_ERROR(LABEL, "Verify remote service is null.");
        return false;
    }
    int32_t requestResult = remote->SendRequest(
        static_cast<uint32_t>(SecurityComponentServiceInterfaceCode::VERIFY_TEMP_SAVE_PERMISSION),
        data, reply, option);
    if (requestResult != SC_OK) {
        SC_LOG_ERROR(LABEL, "Verify request failed, result: %{public}d.", requestResult);
        return false;
    }
    bool res;
    if (!reply.ReadBool(res)) {
        SC_LOG_ERROR(LABEL, "Verify read res failed.");
        return false;
    }
    return res;
}

sptr<IRemoteObject> SecCompProxy::GetEnhanceRemoteObject()
{
    std::lock_guard<std::mutex> lock(useIPCMutex_);
    MessageParcel rawData;
    MessageParcel data;
    if (!data.WriteInterfaceToken(SecCompProxy::GetDescriptor())) {
        SC_LOG_ERROR(LABEL, "Get enhance write descriptor failed.");
        return nullptr;
    }

    if (!SecCompEnhanceAdapter::EnhanceClientSerialize(rawData, data)) {
        SC_LOG_ERROR(LABEL, "Get enhance serialize session info failed.");
        return nullptr;
    }

    MessageParcel reply;
    MessageParcel deserializedReply;
    MessageOption option(MessageOption::TF_SYNC);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        SC_LOG_ERROR(LABEL, "Get enhance remote service is null.");
        return nullptr;
    }
    int32_t requestResult = remote->SendRequest(
        static_cast<uint32_t>(SecurityComponentServiceInterfaceCode::GET_SECURITY_COMPONENT_ENHANCE_OBJECT),
        data, reply, option);

    sptr<IRemoteObject> callback;
    if (requestResult == SC_OK) {
        callback = reply.ReadRemoteObject();
        if (callback == nullptr) {
            SC_LOG_ERROR(LABEL, "Get enhance read callback failed.");
        }
    } else {
        SC_LOG_ERROR(LABEL, "Get enhance request failed, result: %{public}d.", requestResult);
    }

    if (!SecCompEnhanceAdapter::EnhanceClientDeserialize(reply, deserializedReply)) {
        SC_LOG_ERROR(LABEL, "Get enhance deserialize session info failed.");
    }

    return callback;
}

int32_t SecCompProxy::PreRegisterSecCompProcess()
{
    std::lock_guard<std::mutex> lock(useIPCMutex_);
    MessageParcel rawData;
    MessageParcel data;
    if (!data.WriteInterfaceToken(SecCompProxy::GetDescriptor())) {
        SC_LOG_ERROR(LABEL, "PreRegister write descriptor failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (!SecCompEnhanceAdapter::EnhanceClientSerialize(rawData, data)) {
        SC_LOG_ERROR(LABEL, "PreRegister serialize session info failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    MessageParcel reply;
    MessageParcel deserializedReply;
    MessageOption option(MessageOption::TF_SYNC);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        SC_LOG_ERROR(LABEL, "PreRegister remote service is null.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }
    int32_t requestResult = remote->SendRequest(
        static_cast<uint32_t>(SecurityComponentServiceInterfaceCode::PRE_REGISTER_PROCESS),
        data, reply, option);

    if (!SecCompEnhanceAdapter::EnhanceClientDeserialize(reply, deserializedReply)) {
        SC_LOG_ERROR(LABEL, "PreRegister deserialize session info failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (requestResult != SC_OK) {
        SC_LOG_ERROR(LABEL, "PreRegister request failed, result: %{public}d.", requestResult);
        return requestResult;
    }

    int32_t res;
    if (!deserializedReply.ReadInt32(res)) {
        SC_LOG_ERROR(LABEL, "PreRegister read res failed.");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }
    return res;
}
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
