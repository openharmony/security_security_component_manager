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
#include "sec_comp_err.h"
#include "sec_comp_log.h"

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
    MessageParcel data;
    if (!data.WriteInterfaceToken(SecCompProxy::GetDescriptor())) {
        SC_LOG_ERROR(LABEL, "Register write descriptor fail");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (!data.WriteUint32(type)) {
        SC_LOG_ERROR(LABEL, "Register write Uint32 fail");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }
    if (!data.WriteString(componentInfo)) {
        SC_LOG_ERROR(LABEL, "Register write string fail");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        SC_LOG_ERROR(LABEL, "Register remote service is null");
        return SC_SERVICE_ERROR_IPC_REQUEST_FAIL;
    }
    int32_t requestResult = remote->SendRequest(
        static_cast<uint32_t>(SecurityComponentServiceInterfaceCode::REGISTER_SECURITY_COMPONENT),
        data, reply, option);
    if (requestResult != SC_OK) {
        SC_LOG_ERROR(LABEL, "Register request fail, result: %{public}d", requestResult);
        return requestResult;
    }
    int32_t res;
    if (!reply.ReadInt32(res)) {
        SC_LOG_ERROR(LABEL, "Register read result int32 fail");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (!reply.ReadInt32(scId)) {
        SC_LOG_ERROR(LABEL, "Register read scId int32 fail");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }
    return res;
}

int32_t SecCompProxy::UpdateSecurityComponent(int32_t scId, const std::string& componentInfo)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(SecCompProxy::GetDescriptor())) {
        SC_LOG_ERROR(LABEL, "Update write descriptor fail");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (!data.WriteInt32(scId)) {
        SC_LOG_ERROR(LABEL, "Update write Int32 fail");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }
    if (!data.WriteString(componentInfo)) {
        SC_LOG_ERROR(LABEL, "Update write string fail");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        SC_LOG_ERROR(LABEL, "Update remote update service is null");
        return SC_SERVICE_ERROR_IPC_REQUEST_FAIL;
    }
    int32_t requestResult = remote->SendRequest(
        static_cast<uint32_t>(SecurityComponentServiceInterfaceCode::UPDATE_SECURITY_COMPONENT), data, reply, option);
    if (requestResult != SC_OK) {
        SC_LOG_ERROR(LABEL, "Update request fail, result: %{public}d", requestResult);
        return requestResult;
    }
    int32_t res;
    if (!reply.ReadInt32(res)) {
        SC_LOG_ERROR(LABEL, "Update read result int32 fail");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }
    return res;
}

int32_t SecCompProxy::UnregisterSecurityComponent(int32_t scId)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(SecCompProxy::GetDescriptor())) {
        SC_LOG_ERROR(LABEL, "Unregister write descriptor fail");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (!data.WriteInt32(scId)) {
        SC_LOG_ERROR(LABEL, "Unregister write Int32 fail");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        SC_LOG_ERROR(LABEL, "Unregister remote service is null");
        return SC_SERVICE_ERROR_IPC_REQUEST_FAIL;
    }
    int32_t requestResult = remote->SendRequest(
        static_cast<uint32_t>(SecurityComponentServiceInterfaceCode::UNREGISTER_SECURITY_COMPONENT),
        data, reply, option);
    if (requestResult != SC_OK) {
        SC_LOG_ERROR(LABEL, "Unregister request fail, result: %{public}d", requestResult);
        return requestResult;
    }
    int32_t res;
    if (!reply.ReadInt32(res)) {
        SC_LOG_ERROR(LABEL, "Unregister read int32 fail");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }
    return res;
}

int32_t SecCompProxy::ReportSecurityComponentClickEvent(int32_t scId,
    const std::string& componentInfo, const SecCompClickEvent& touchInfo, sptr<IRemoteObject> callerToken)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(SecCompProxy::GetDescriptor())) {
        SC_LOG_ERROR(LABEL, "Report write descriptor fail");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (!data.WriteInt32(scId)) {
        SC_LOG_ERROR(LABEL, "Report write Uint32 fail");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (!data.WriteString(componentInfo)) {
        SC_LOG_ERROR(LABEL, "Report write string fail");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    sptr<SecCompClickEventParcel> parcel = new (std::nothrow) SecCompClickEventParcel();
    if (parcel == nullptr) {
        SC_LOG_ERROR(LABEL, "Report new click event parcel failed");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }
    parcel->touchInfoParams_ = touchInfo;
    if (!data.WriteParcelable(parcel)) {
        SC_LOG_ERROR(LABEL, "Report write touchInfo fail");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if ((callerToken != nullptr) && !data.WriteRemoteObject(callerToken)) {
        SC_LOG_ERROR(LABEL, "Report write caller token fail");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        SC_LOG_ERROR(LABEL, "Report remote service is null");
        return SC_SERVICE_ERROR_IPC_REQUEST_FAIL;
    }
    int32_t requestResult = remote->SendRequest(
        static_cast<uint32_t>(SecurityComponentServiceInterfaceCode::REPORT_SECURITY_COMPONENT_CLICK_EVENT),
        data, reply, option);
    if (requestResult != SC_OK) {
        SC_LOG_ERROR(LABEL, "Report request fail, result: %{public}d", requestResult);
        return requestResult;
    }
    int32_t res;
    if (!reply.ReadInt32(res)) {
        SC_LOG_ERROR(LABEL, "Report read int32 fail");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }
    return res;
}

bool SecCompProxy::ReduceAfterVerifySavePermission(AccessToken::AccessTokenID tokenId)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(SecCompProxy::GetDescriptor())) {
        SC_LOG_ERROR(LABEL, "Verify write descriptor fail");
        return false;
    }

    if (!data.WriteUint32(tokenId)) {
        SC_LOG_ERROR(LABEL, "Verify write Uint32 fail");
        return false;
    }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        SC_LOG_ERROR(LABEL, "Verify remote service is null");
        return false;
    }
    int32_t requestResult = remote->SendRequest(
        static_cast<uint32_t>(SecurityComponentServiceInterfaceCode::VERIFY_TEMP_SAVE_PERMISSION),
        data, reply, option);
    if (requestResult != SC_OK) {
        SC_LOG_ERROR(LABEL, "Verify request fail, result: %{public}d", requestResult);
        return false;
    }
    bool res;
    if (!reply.ReadBool(res)) {
        SC_LOG_ERROR(LABEL, "Verify read bool fail");
        return false;
    }
    return res;
}

sptr<IRemoteObject> SecCompProxy::GetEnhanceRemoteObject()
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(SecCompProxy::GetDescriptor())) {
        SC_LOG_ERROR(LABEL, "Get enhance write descriptor fail");
        return nullptr;
    }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        SC_LOG_ERROR(LABEL, "Get enhance remote service is null");
        return nullptr;
    }
    int32_t requestResult = remote->SendRequest(
        static_cast<uint32_t>(SecurityComponentServiceInterfaceCode::GET_SECURITY_COMPONENT_ENHANCE_OBJECT),
        data, reply, option);
    if (requestResult != SC_OK) {
        SC_LOG_ERROR(LABEL, "Get enhance request fail, result: %{public}d", requestResult);
        return nullptr;
    }
    sptr<IRemoteObject> callback = reply.ReadRemoteObject();
    if (callback == nullptr) {
        SC_LOG_ERROR(LABEL, "Get enhance read remote object fail");
    }
    return callback;
}
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
