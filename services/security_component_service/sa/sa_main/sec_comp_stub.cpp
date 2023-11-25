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
#include "sec_comp_stub.h"

#include "accesstoken_kit.h"
#include "ipc_skeleton.h"
#include "sec_comp_click_event_parcel.h"
#include "sec_comp_err.h"
#include "sec_comp_log.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "SecCompStub"};
static constexpr int32_t ROOT_UID = 0;
static constexpr int32_t BASE_USER_RANGE = 200000;
}  // namespace

int32_t SecCompStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    std::u16string descripter = SecCompStub::GetDescriptor();
    std::u16string remoteDescripter = data.ReadInterfaceToken();
    if (descripter != remoteDescripter) {
        SC_LOG_ERROR(LABEL, "Deal remote request fail, descriptor is not matched");
        return SC_SERVICE_ERROR_IPC_REQUEST_FAIL;
    }

    auto funcIter = requestFuncMap_.find(code);
    if (funcIter != requestFuncMap_.end()) {
        auto func = funcIter->second;
        if (func != nullptr) {
            return (this->*func)(data, reply);
        }
    }
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int32_t SecCompStub::RegisterSecurityComponentInner(MessageParcel& data, MessageParcel& reply)
{
    uint32_t type;
    if (!data.ReadUint32(type)) {
        SC_LOG_ERROR(LABEL, "Register read component type fail");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (type <= UNKNOWN_SC_TYPE || type >= MAX_SC_TYPE) {
        SC_LOG_ERROR(LABEL, "Register security component type invalid");
        return SC_SERVICE_ERROR_VALUE_INVALID;
    }

    std::string componentInfo;
    if (!data.ReadString(componentInfo)) {
        SC_LOG_ERROR(LABEL, "Register read component info fail");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    int32_t scId = INVALID_SC_ID;
    int32_t res = this->RegisterSecurityComponent(static_cast<SecCompType>(type), componentInfo, scId);
    if (!reply.WriteInt32(res)) {
        SC_LOG_ERROR(LABEL, "Register security component result fail");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (!reply.WriteInt32(scId)) {
        SC_LOG_ERROR(LABEL, "Register security component result fail");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    return SC_OK;
}

int32_t SecCompStub::UpdateSecurityComponentInner(MessageParcel& data, MessageParcel& reply)
{
    int32_t scId;
    if (!data.ReadInt32(scId)) {
        SC_LOG_ERROR(LABEL, "Update read component id fail");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (scId < 0) {
        SC_LOG_ERROR(LABEL, "Update security component id invalid");
        return SC_SERVICE_ERROR_VALUE_INVALID;
    }

    std::string componentInfo;
    if (!data.ReadString(componentInfo)) {
        SC_LOG_ERROR(LABEL, "Update read component info fail");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    int32_t res = this->UpdateSecurityComponent(scId, componentInfo);
    if (!reply.WriteInt32(res)) {
        SC_LOG_ERROR(LABEL, "Update security component result fail");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    return res;
}

int32_t SecCompStub::UnregisterSecurityComponentInner(MessageParcel& data, MessageParcel& reply)
{
    int32_t scId;
    if (!data.ReadInt32(scId)) {
        SC_LOG_ERROR(LABEL, "Unreigster read component id fail");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (scId < 0) {
        SC_LOG_ERROR(LABEL, "Unreigster security component id invalid");
        return SC_SERVICE_ERROR_VALUE_INVALID;
    }

    int32_t res = this->UnregisterSecurityComponent(scId);
    if (!reply.WriteInt32(res)) {
        SC_LOG_ERROR(LABEL, "Unregister security component result fail");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }
    return SC_OK;
}

int32_t SecCompStub::ReportSecurityComponentClickEventInner(MessageParcel& data, MessageParcel& reply)
{
    int32_t scId;
    if (!data.ReadInt32(scId)) {
        SC_LOG_ERROR(LABEL, "Report read component id fail");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (scId < 0) {
        SC_LOG_ERROR(LABEL, "Report security component id invalid");
        return SC_SERVICE_ERROR_VALUE_INVALID;
    }

    std::string componentInfo;
    if (!data.ReadString(componentInfo)) {
        SC_LOG_ERROR(LABEL, "Report read component info fail");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }
    sptr<SecCompClickEventParcel> clickInfoParcel = data.ReadParcelable<SecCompClickEventParcel>();
    if (clickInfoParcel == nullptr) {
        SC_LOG_ERROR(LABEL, "Report read clickInfo info fail");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    sptr<IRemoteObject> callerToken = data.ReadRemoteObject();
    if (callerToken == nullptr) {
        SC_LOG_ERROR(LABEL, "callerToken is nullptr");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }
    int32_t res =
        this->ReportSecurityComponentClickEvent(scId, componentInfo, clickInfoParcel->clickInfoParams_, callerToken);
    if (!reply.WriteInt32(res)) {
        SC_LOG_ERROR(LABEL, "Report security component result fail");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }
    return SC_OK;
}

int32_t SecCompStub::VerifySavePermissionInner(MessageParcel& data, MessageParcel& reply)
{
    if (!IsMediaLibraryCalling()) {
        SC_LOG_ERROR(LABEL, "Not medialibrary called");
        return SC_SERVICE_ERROR_CALLER_INVALID;
    }
    uint32_t tokenId;
    if (!data.ReadUint32(tokenId)) {
        SC_LOG_ERROR(LABEL, "Verify read component id fail");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }

    if (tokenId == 0) {
        SC_LOG_ERROR(LABEL, "Verify AccessTokenId invalid");
        return SC_SERVICE_ERROR_VALUE_INVALID;
    }

    bool res = this->VerifySavePermission(tokenId);
    if (!reply.WriteBool(res)) {
        SC_LOG_ERROR(LABEL, "Verify temp save permission result fail");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }
    return SC_OK;
}

int32_t SecCompStub::GetEnhanceRemoteObjectInner(MessageParcel& data, MessageParcel& reply)
{
    auto res = this->GetEnhanceRemoteObject();
    if (!reply.WriteRemoteObject(res)) {
        SC_LOG_ERROR(LABEL, "Security component enhance remote object fail");
        return SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL;
    }
    return SC_OK;
}

bool SecCompStub::IsMediaLibraryCalling()
{
    int32_t uid = IPCSkeleton::GetCallingUid();
    if (uid == ROOT_UID) {
        return true;
    }
    int32_t userId = uid / BASE_USER_RANGE;
    uint32_t tokenCaller = IPCSkeleton::GetCallingTokenID();
    if (mediaLibraryTokenId_ == 0) {
        mediaLibraryTokenId_ = AccessToken::AccessTokenKit::GetHapTokenID(
            userId, "com.ohos.medialibrary.medialibrarydata", 0);
    }
    return tokenCaller == mediaLibraryTokenId_;
}

SecCompStub::SecCompStub()
{
    requestFuncMap_[static_cast<uint32_t>(SecurityComponentServiceInterfaceCode::REGISTER_SECURITY_COMPONENT)] =
        &SecCompStub::RegisterSecurityComponentInner;
    requestFuncMap_[static_cast<uint32_t>(SecurityComponentServiceInterfaceCode::UPDATE_SECURITY_COMPONENT)] =
        &SecCompStub::UpdateSecurityComponentInner;
    requestFuncMap_[static_cast<uint32_t>(SecurityComponentServiceInterfaceCode::UNREGISTER_SECURITY_COMPONENT)] =
        &SecCompStub::UnregisterSecurityComponentInner;
    requestFuncMap_[static_cast<uint32_t>(
        SecurityComponentServiceInterfaceCode::REPORT_SECURITY_COMPONENT_CLICK_EVENT)] =
        &SecCompStub::ReportSecurityComponentClickEventInner;
    requestFuncMap_[static_cast<uint32_t>(SecurityComponentServiceInterfaceCode::VERIFY_TEMP_SAVE_PERMISSION)] =
        &SecCompStub::VerifySavePermissionInner;
    requestFuncMap_[static_cast<uint32_t>(
        SecurityComponentServiceInterfaceCode::GET_SECURITY_COMPONENT_ENHANCE_OBJECT)] =
        &SecCompStub::GetEnhanceRemoteObjectInner;
}

SecCompStub::~SecCompStub()
{
    SC_LOG_ERROR(LABEL, "~SecCompStub");
    requestFuncMap_.clear();
    SC_LOG_ERROR(LABEL, "~SecCompStub end");
}
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
