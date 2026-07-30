/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include "wm_mini_client_adapter.h"

#include <string>

#include "errors.h"
#include "ipc_types.h"
#include "message_option.h"
#include "message_parcel.h"
#include "sec_comp_log.h"
#include "wm_mini_client_environment.h"

namespace OHOS {
namespace Rosen {
using Security::SecurityComponent::MiniAccessibilityWindowInfo;
using Security::SecurityComponent::MiniUnreliableWindowInfo;
using Security::SecurityComponent::MiniWMError;

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "WMClientMini"};
constexpr char16_t WINDOW_MANAGER_DESCRIPTOR[] = u"OHOS.IWindowManager";
constexpr char16_t SESSION_MANAGER_SERVICE_DESCRIPTOR[] = u"OHOS.ISessionManagerService";
constexpr char16_t MOCK_SESSION_MANAGER_DESCRIPTOR[] = u"OHOS.IMockSessionManager";
constexpr char16_t SCENE_SESSION_MANAGER_DESCRIPTOR[] = u"OHOS.ISceneSessionManager";
constexpr int32_t INVALID_USER_ID = -1;

enum class WmBackend {
    LEGACY_WMS,
    SCENE_BOARD,
    UNKNOWN,
};

enum class MockSessionManagerMessageMini : uint32_t {
    TRANS_ID_GET_SESSION_MANAGER_SERVICE = 0,
    TRANS_ID_GET_SESSION_MANAGER_SERVICE_BY_USER_ID,
};

enum class SessionManagerServiceMessageMini : uint32_t {
    TRANS_ID_GET_SCENE_SESSION_MANAGER = 0,
};

enum class SceneSessionManagerMessageMini : uint32_t {
    // Keep aligned with ISceneSessionManager::SceneSessionManagerMessage.
    TRANS_ID_GET_WINDOW_INFO = 12,
    TRANS_ID_GET_UNRELIABLE_WINDOW_INFO = 80,
};

enum class WindowManagerMessageMini : uint32_t {
    // Keep aligned with IWindowManager::WindowManagerMessage.
    TRANS_ID_GET_WINDOW_INFO_LEGACY = 20,
    TRANS_ID_GET_UNRELIABLE_WINDOW_INFO = 53,
};

WmBackend GetWmBackend(const sptr<IRemoteObject>& remote)
{
    if (remote == nullptr) {
        return WmBackend::UNKNOWN;
    }
    const std::u16string descriptor = remote->GetInterfaceDescriptor();
    if (descriptor == WINDOW_MANAGER_DESCRIPTOR) {
        return WmBackend::LEGACY_WMS;
    }
    if (descriptor == MOCK_SESSION_MANAGER_DESCRIPTOR) {
        return WmBackend::SCENE_BOARD;
    }
    return WmBackend::UNKNOWN;
}

template<typename T>
bool UnmarshalParcelableVector(MessageParcel& parcel, std::vector<sptr<T>>& infos)
{
    int32_t len = 0;
    if (!parcel.ReadInt32(len) || len < 0) {
        return false;
    }

    const size_t size = static_cast<size_t>(len);
    if (size > parcel.GetReadableBytes() ||
        !WmMiniClientEnvironment::IsParcelableVectorSizeValid(size, infos.max_size())) {
        return false;
    }
    if (!WmMiniClientEnvironment::ResizeParcelableVector(infos, size)) {
        return false;
    }

    for (size_t i = 0; i < size; ++i) {
        if (parcel.GetReadableBytes() < sizeof(int32_t)) {
            return false;
        }
        infos[i] = parcel.ReadParcelable<T>();
        if (infos[i] == nullptr) {
            return false;
        }
    }
    return true;
}

MiniWMError ReadReplyErrCode(MessageParcel& reply)
{
    int32_t ret = ERR_NONE;
    if (!reply.ReadInt32(ret)) {
        SC_LOG_ERROR(LABEL, "Read reply err code failed");
        return MiniWMError::WM_ERROR_IPC_FAILED;
    }
    return static_cast<MiniWMError>(ret);
}

template<typename T>
MiniWMError ReadWindowInfoReply(MessageParcel& reply, std::vector<sptr<T>>& infos)
{
    if (!UnmarshalParcelableVector(reply, infos)) {
        SC_LOG_ERROR(LABEL, "Read window info failed");
        return MiniWMError::WM_ERROR_IPC_FAILED;
    }
    return ReadReplyErrCode(reply);
}

sptr<IRemoteObject> ReadMockSessionManagerRemote(MessageParcel& reply)
{
    int32_t errCode = ERR_NONE;
    if (!reply.ReadInt32(errCode)) {
        SC_LOG_ERROR(LABEL, "Read mock session manager err code failed");
        return nullptr;
    }
    if (errCode != ERR_NONE) {
        SC_LOG_ERROR(LABEL, "Mock session manager request failed, errCode=%{public}d", errCode);
        return nullptr;
    }

    sptr<IRemoteObject> remote = reply.ReadRemoteObject();
    if (remote == nullptr) {
        SC_LOG_ERROR(LABEL, "Read mock session manager remote failed");
        return nullptr;
    }
    return remote;
}

sptr<IRemoteObject> GetSessionManagerServiceRemote(const sptr<IRemoteObject>& wmsRemote, int32_t userId)
{
    if (wmsRemote == nullptr) {
        SC_LOG_ERROR(LABEL, "Window manager service remote is null");
        return nullptr;
    }

    MessageParcel wmsData;
    MessageParcel wmsReply;
    MessageOption wmsOption(MessageOption::TF_SYNC);
    if (!WmMiniClientEnvironment::WriteInterfaceToken(wmsData, MOCK_SESSION_MANAGER_DESCRIPTOR,
        WmMiniParcelField::MOCK_INTERFACE_TOKEN)) {
        SC_LOG_ERROR(LABEL, "Write mock session manager descriptor failed");
        return nullptr;
    }
    auto requestCode = MockSessionManagerMessageMini::TRANS_ID_GET_SESSION_MANAGER_SERVICE;
    const bool useUserInstance =
        userId > INVALID_USER_ID && WmMiniClientEnvironment::IsMultiInstanceEnabled();
    if (useUserInstance) {
        requestCode = MockSessionManagerMessageMini::TRANS_ID_GET_SESSION_MANAGER_SERVICE_BY_USER_ID;
        if (!WmMiniClientEnvironment::WriteInt32(wmsData, userId, WmMiniParcelField::MOCK_USER_ID)) {
            SC_LOG_ERROR(LABEL, "Write user id failed");
            return nullptr;
        }
    }
    int32_t ret = wmsRemote->SendRequest(
        static_cast<uint32_t>(requestCode), wmsData, wmsReply, wmsOption);
    if (ret != ERR_NONE) {
        SC_LOG_ERROR(LABEL, "Get session manager service remote request failed, ret=%{public}d", ret);
        return nullptr;
    }
    return ReadMockSessionManagerRemote(wmsReply);
}

sptr<IRemoteObject> GetSceneSessionManagerRemote(const sptr<IRemoteObject>& wmsRemote, int32_t userId)
{
    sptr<IRemoteObject> sessionManagerServiceRemote = GetSessionManagerServiceRemote(wmsRemote, userId);
    if (sessionManagerServiceRemote == nullptr) {
        SC_LOG_ERROR(LABEL, "Read session manager service remote failed");
        return nullptr;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!WmMiniClientEnvironment::WriteInterfaceToken(data, SESSION_MANAGER_SERVICE_DESCRIPTOR,
        WmMiniParcelField::SESSION_INTERFACE_TOKEN)) {
        SC_LOG_ERROR(LABEL, "Write session manager service descriptor failed");
        return nullptr;
    }
    int32_t ret = sessionManagerServiceRemote->SendRequest(
        static_cast<uint32_t>(SessionManagerServiceMessageMini::TRANS_ID_GET_SCENE_SESSION_MANAGER), data, reply,
        option);
    if (ret != ERR_NONE) {
        SC_LOG_ERROR(LABEL, "Get scene session manager remote request failed, ret=%{public}d", ret);
        return nullptr;
    }
    return reply.ReadRemoteObject();
}

MiniWMError GetWindowInfoByRemote(const sptr<IRemoteObject>& remote,
    const std::u16string& descriptor, uint32_t requestCode,
    std::vector<sptr<MiniAccessibilityWindowInfo>>& infos)
{
    if (remote == nullptr) {
        return MiniWMError::WM_ERROR_SAMGR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WmMiniClientEnvironment::WriteInterfaceToken(data, descriptor,
        WmMiniParcelField::WINDOW_INFO_INTERFACE_TOKEN)) {
        SC_LOG_ERROR(LABEL, "Write window manager descriptor failed");
        return MiniWMError::WM_ERROR_IPC_FAILED;
    }
    int32_t ret = remote->SendRequest(requestCode, data, reply, option);
    if (ret != ERR_NONE) {
        SC_LOG_ERROR(LABEL, "Get window info request failed");
        return MiniWMError::WM_ERROR_IPC_FAILED;
    }
    return ReadWindowInfoReply(reply, infos);
}

MiniWMError GetUnreliableWindowInfoByRemote(const sptr<IRemoteObject>& remote,
    const std::u16string& descriptor, uint32_t requestCode, int32_t windowId,
    std::vector<sptr<MiniUnreliableWindowInfo>>& infos)
{
    if (remote == nullptr) {
        return MiniWMError::WM_ERROR_SAMGR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WmMiniClientEnvironment::WriteInterfaceToken(data, descriptor,
        WmMiniParcelField::UNRELIABLE_INTERFACE_TOKEN)) {
        SC_LOG_ERROR(LABEL, "Write window manager descriptor failed");
        return MiniWMError::WM_ERROR_IPC_FAILED;
    }
    if (!WmMiniClientEnvironment::WriteInt32(data, windowId, WmMiniParcelField::UNRELIABLE_WINDOW_ID)) {
        SC_LOG_ERROR(LABEL, "Write unreliable window id failed");
        return MiniWMError::WM_ERROR_IPC_FAILED;
    }
    int32_t ret = remote->SendRequest(requestCode, data, reply, option);
    if (ret != ERR_NONE) {
        SC_LOG_ERROR(LABEL, "Get unreliable window info request failed");
        return MiniWMError::WM_ERROR_IPC_FAILED;
    }
    return ReadWindowInfoReply(reply, infos);
}
}

MiniWMError WmMiniClientAdapter::GetWindowInfo(int32_t userId,
    std::vector<sptr<MiniAccessibilityWindowInfo>>& infos)
{
    sptr<IRemoteObject> wmsRemote = WmMiniClientEnvironment::GetWindowManagerServiceRemote();
    if (wmsRemote == nullptr) {
        return MiniWMError::WM_ERROR_SAMGR;
    }
    WmBackend backend = GetWmBackend(wmsRemote);
    if (backend == WmBackend::LEGACY_WMS) {
        return GetWindowInfoByRemote(wmsRemote, WINDOW_MANAGER_DESCRIPTOR,
            static_cast<uint32_t>(WindowManagerMessageMini::TRANS_ID_GET_WINDOW_INFO_LEGACY), infos);
    }
    if (backend == WmBackend::SCENE_BOARD) {
        sptr<IRemoteObject> sceneRemote = GetSceneSessionManagerRemote(wmsRemote, userId);
        return GetWindowInfoByRemote(sceneRemote, SCENE_SESSION_MANAGER_DESCRIPTOR,
            static_cast<uint32_t>(SceneSessionManagerMessageMini::TRANS_ID_GET_WINDOW_INFO), infos);
    }
    SC_LOG_ERROR(LABEL, "Unsupported window manager backend");
    return MiniWMError::WM_ERROR_IPC_FAILED;
}

MiniWMError WmMiniClientAdapter::GetUnreliableWindowInfo(int32_t windowId, int32_t userId,
    std::vector<sptr<MiniUnreliableWindowInfo>>& infos)
{
    sptr<IRemoteObject> wmsRemote = WmMiniClientEnvironment::GetWindowManagerServiceRemote();
    if (wmsRemote == nullptr) {
        return MiniWMError::WM_ERROR_SAMGR;
    }
    WmBackend backend = GetWmBackend(wmsRemote);
    if (backend == WmBackend::LEGACY_WMS) {
        return GetUnreliableWindowInfoByRemote(wmsRemote, WINDOW_MANAGER_DESCRIPTOR,
            static_cast<uint32_t>(WindowManagerMessageMini::TRANS_ID_GET_UNRELIABLE_WINDOW_INFO), windowId, infos);
    }
    if (backend == WmBackend::SCENE_BOARD) {
        sptr<IRemoteObject> sceneRemote = GetSceneSessionManagerRemote(wmsRemote, userId);
        return GetUnreliableWindowInfoByRemote(sceneRemote, SCENE_SESSION_MANAGER_DESCRIPTOR,
            static_cast<uint32_t>(SceneSessionManagerMessageMini::TRANS_ID_GET_UNRELIABLE_WINDOW_INFO),
            windowId, infos);
    }
    SC_LOG_ERROR(LABEL, "Unsupported window manager backend");
    return MiniWMError::WM_ERROR_IPC_FAILED;
}

MiniAccessibilityWindowInfo* WmMiniClientAdapter::CreateWindowInfo()
{
    return WmMiniClientEnvironment::CreateWindowInfo();
}

MiniUnreliableWindowInfo* WmMiniClientAdapter::CreateUnreliableWindowInfo()
{
    return WmMiniClientEnvironment::CreateUnreliableWindowInfo();
}
}  // namespace Rosen
}  // namespace OHOS
