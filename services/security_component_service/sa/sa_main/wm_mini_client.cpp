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
#include "wm_mini_client.h"

#ifndef TDD_ENABLE
#include <new>

#include "iservice_registry.h"
#include "message_option.h"
#include "message_parcel.h"
#include "system_ability_definition.h"
#endif

#include "sec_comp_log.h"

namespace OHOS {
namespace Rosen {
using Security::SecurityComponent::MiniAccessibilityWindowInfo;
using Security::SecurityComponent::MiniRect;
using Security::SecurityComponent::MiniUnreliableWindowInfo;
using Security::SecurityComponent::MiniWMError;

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "WMClientMini"};

#ifndef TDD_ENABLE
constexpr char16_t SESSION_MANAGER_SERVICE_DESCRIPTOR[] = u"OHOS.ISessionManagerService";
constexpr char16_t MOCK_SESSION_MANAGER_DESCRIPTOR[] = u"OHOS.IMockSessionManager";
constexpr char16_t SCENE_SESSION_MANAGER_DESCRIPTOR[] = u"OHOS.ISceneSessionManager";

enum class MockSessionManagerMessageMini : uint32_t {
    TRANS_ID_GET_SESSION_MANAGER_SERVICE = 0,
};

enum class SessionManagerServiceMessageMini : uint32_t {
    TRANS_ID_GET_SCENE_SESSION_MANAGER = 0,
    TRANS_ID_GET_SCENE_SESSION_MANAGER_LITE = 1,
};

enum class SceneSessionManagerMessageMini : uint32_t {
    TRANS_ID_CREATE_AND_CONNECT_SPECIFIC_SESSION = 0,
    TRANS_ID_DESTROY_AND_DISCONNECT_SPECIFIC_SESSION,
    TRANS_ID_UPDATE_PROPERTY,
    TRANS_ID_REQUEST_FOCUS,
    TRANS_ID_REGISTER_WINDOW_MANAGER_AGENT,
    TRANS_ID_UNREGISTER_WINDOW_MANAGER_AGENT,
    TRANS_ID_BIND_DIALOG_TARGET,
    TRANS_ID_GET_FOCUS_SESSION_INFO,
    TRANS_ID_SET_SESSION_LABEL,
    TRANS_ID_SET_SESSION_ICON,
    TRANS_ID_IS_VALID_SESSION_IDS,
    TRANS_ID_SET_GESTURE_NAVIGATION_ENABLED,
    TRANS_ID_GET_WINDOW_INFO,
    TRANS_ID_PENDING_SESSION_TO_FOREGROUND,
    TRANS_ID_PENDING_SESSION_TO_BACKGROUND_FOR_DELEGATOR,
    TRANS_ID_GET_FOCUS_SESSION_TOKEN,
    TRANS_ID_GET_FOCUS_SESSION_ELEMENT,
    TRANS_ID_CHECK_WINDOW_ID,
    TRANS_ID_REGISTER_SESSION_LISTENER,
    TRANS_ID_UNREGISTER_SESSION_LISTENER,
    TRANS_ID_GET_MISSION_INFOS,
    TRANS_ID_GET_MISSION_INFO_BY_ID,
    TRANS_ID_GET_SESSION_INFO_WITH_DISPLAY,
    TRANS_ID_GET_SESSION_INFO_BY_CONTINUE_SESSION_ID,
    TRANS_ID_DUMP_SESSION_ALL,
    TRANS_ID_DUMP_SESSION_WITH_ID,
    TRANS_ID_TERMINATE_SESSION_NEW,
    TRANS_ID_GET_SESSION_DUMP_INFO,
    TRANS_ID_UPDATE_AVOIDAREA_LISTENER,
    TRANS_ID_GET_SESSION_SNAPSHOT,
    TRANS_ID_GET_SESSION_SNAPSHOT_BY_ID,
    TRANS_ID_SET_SESSION_CONTINUE_STATE,
    TRANS_ID_NOTIFY_DUMP_INFO_RESULT,
    TRANS_ID_CLEAR_SESSION,
    TRANS_ID_CLEAR_ALL_SESSIONS,
    TRANS_ID_LOCK_SESSION,
    TRANS_ID_UNLOCK_SESSION,
    TRANS_ID_MOVE_MISSIONS_TO_FOREGROUND,
    TRANS_ID_MOVE_MISSIONS_TO_BACKGROUND,
    TRANS_ID_REGISTER_COLLABORATOR,
    TRANS_ID_UNREGISTER_COLLABORATOR,
    TRANS_ID_UPDATE_TOUCHOUTSIDE_LISTENER,
    TRANS_ID_RAISE_WINDOW_TO_TOP,
    TRANS_ID_NOTIFY_WINDOW_EXTENSION_VISIBILITY_CHANGE,
    TRANS_ID_RECOVER_AND_RECONNECT_SCENE_SESSION,
    TRANS_ID_RECOVER_AND_CONNECT_SPECIFIC_SESSION,
    TRANS_ID_GET_TOP_WINDOW_ID,
    TRANS_ID_GET_PARENT_MAIN_WINDOW_ID,
    TRANS_ID_GET_UI_CONTENT_REMOTE_OBJ,
    TRANS_ID_GET_ROOT_UI_CONTENT_REMOTE_OBJ,
    TRANS_ID_UPDATE_WINDOW_VISIBILITY_LISTENER,
    TRANS_ID_UPDATE_SESSION_SCREENSHOT_LISTENER,
    TRANS_ID_UPDATE_SESSION_OCCLUSION_STATE_LISTENER,
    TRANS_ID_GET_WINDOW_STATE_SNAPSHOT,
    TRANS_ID_NOTIFY_SURFACE_NODE_ALPHA_UPDATE,
    TRANS_ID_SHIFT_APP_WINDOW_FOCUS,
    TRANS_ID_LIST_WINDOW_INFO,
    TRANS_ID_GET_WINDOW_LAYOUT_INFO,
    TRANS_ID_GET_ALL_MAIN_WINDOW_INFO,
    TRANS_ID_GET_MAIN_WINDOW_SNAPSHOT,
    TRANS_ID_SET_WINDOW_SNAPSHOT_SKIP,
    TRANS_ID_GET_GLOBAL_WINDOW_MODE,
    TRANS_ID_GET_TOP_NAV_DEST_NAME,
    TRANS_ID_SET_SCREEN_WATERMARK_IMAGE,
    TRANS_ID_CLEAN_SCREEN_WATERMARK_IMAGE,
    TRANS_ID_RECOVER_SCREEN_WATERMARK_IMAGE,
    TRANS_ID_SET_APP_WATERMARK_IMAGE,
    TRANS_ID_RECOVER_APP_WATERMARK_IMAGE,
    TRANS_ID_GET_VISIBILITY_WINDOW_INFO_ID,
    TRANS_ID_ADD_EXTENSION_WINDOW_STAGE_TO_SCB,
    TRANS_ID_REMOVE_EXTENSION_WINDOW_STAGE_FROM_SCB,
    TRANS_ID_UPDATE_MODALEXTENSION_RECT_TO_SCB,
    TRANS_ID_PROCESS_MODALEXTENSION_POINTDOWN_TO_SCB,
    TRANS_ID_ADD_OR_REMOVE_SECURE_SESSION,
    TRANS_ID_UPDATE_EXTENSION_WINDOW_FLAGS,
    TRANS_ID_GET_HOST_WINDOW_RECT,
    TRANS_ID_DESTROY_AND_DISCONNECT_SPECIFIC_SESSION_WITH_DETACH_CALLBACK,
    TRANS_ID_GET_WINDOW_STATUS,
    TRANS_ID_GET_WINDOW_RECT,
    TRANS_ID_GET_WINDOW_MODE_TYPE,
    TRANS_ID_GET_UNRELIABLE_WINDOW_INFO,
};

template<typename T>
bool UnmarshalParcelableVector(MessageParcel& parcel, std::vector<sptr<T>>& infos)
{
    int32_t len = parcel.ReadInt32();
    if (len < 0) {
        return false;
    }

    const size_t size = static_cast<size_t>(len);
    if (size > parcel.GetReadableBytes() || size > infos.max_size()) {
        return false;
    }
    infos.resize(size);
    if (infos.size() < size) {
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

bool ReadReplyErrCode(MessageParcel& reply, MiniWMError& errCode)
{
    int32_t ret = ERR_NONE;
    if (!reply.ReadInt32(ret)) {
        SC_LOG_ERROR(LABEL, "Read reply err code failed");
        errCode = MiniWMError::WM_ERROR_IPC_FAILED;
        return false;
    }
    errCode = static_cast<MiniWMError>(ret);
    return true;
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

sptr<IRemoteObject> GetSceneSessionManagerRemote()
{
    auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityManager == nullptr) {
        SC_LOG_ERROR(LABEL, "Get system ability manager failed");
        return nullptr;
    }

    sptr<IRemoteObject> wmsRemote = systemAbilityManager->GetSystemAbility(WINDOW_MANAGER_SERVICE_ID);
    if (wmsRemote == nullptr) {
        SC_LOG_ERROR(LABEL, "Get window manager service remote failed");
        return nullptr;
    }

    MessageParcel mockData;
    MessageParcel mockReply;
    MessageOption mockOption(MessageOption::TF_SYNC);
    if (!mockData.WriteInterfaceToken(std::u16string(MOCK_SESSION_MANAGER_DESCRIPTOR))) {
        SC_LOG_ERROR(LABEL, "Write mock session manager descriptor failed");
        return nullptr;
    }
    int32_t ret = wmsRemote->SendRequest(
        static_cast<uint32_t>(MockSessionManagerMessageMini::TRANS_ID_GET_SESSION_MANAGER_SERVICE), mockData,
        mockReply, mockOption);
    if (ret != ERR_NONE) {
        SC_LOG_ERROR(LABEL, "Get session manager service remote request failed, ret=%{public}d", ret);
        return nullptr;
    }
    sptr<IRemoteObject> sessionManagerServiceRemote = ReadMockSessionManagerRemote(mockReply);
    if (sessionManagerServiceRemote == nullptr) {
        SC_LOG_ERROR(LABEL, "Read session manager service remote failed");
        return nullptr;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(std::u16string(SESSION_MANAGER_SERVICE_DESCRIPTOR))) {
        SC_LOG_ERROR(LABEL, "Write session manager service descriptor failed");
        return nullptr;
    }
    ret = sessionManagerServiceRemote->SendRequest(
        static_cast<uint32_t>(SessionManagerServiceMessageMini::TRANS_ID_GET_SCENE_SESSION_MANAGER), data, reply,
        option);
    if (ret != ERR_NONE) {
        SC_LOG_ERROR(LABEL, "Get scene session manager remote request failed, ret=%{public}d", ret);
        return nullptr;
    }
    return reply.ReadRemoteObject();
}
#endif

#ifdef TDD_ENABLE
MiniWMError GetAccessibilityWindowInfoInner(std::vector<sptr<MiniAccessibilityWindowInfo>>& infos)
{
    std::vector<sptr<OHOS::Rosen::AccessibilityWindowInfo>> srcInfos;
    auto ret = WindowManager::GetInstance().GetAccessibilityWindowInfo(srcInfos);
    infos.assign(srcInfos.begin(), srcInfos.end());
    return static_cast<MiniWMError>(ret);
}

MiniWMError GetUnreliableWindowInfoInner(int32_t windowId, std::vector<sptr<MiniUnreliableWindowInfo>>& infos)
{
    std::vector<sptr<OHOS::Rosen::UnreliableWindowInfo>> srcInfos;
    auto ret = WindowManager::GetInstance().GetUnreliableWindowInfo(windowId, srcInfos);
    infos.assign(srcInfos.begin(), srcInfos.end());
    return static_cast<MiniWMError>(ret);
}
#else
MiniWMError GetAccessibilityWindowInfoInner(std::vector<sptr<MiniAccessibilityWindowInfo>>& infos)
{
    sptr<IRemoteObject> remote = GetSceneSessionManagerRemote();
    if (remote == nullptr) {
        return MiniWMError::WM_ERROR_SAMGR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(std::u16string(SCENE_SESSION_MANAGER_DESCRIPTOR))) {
        SC_LOG_ERROR(LABEL, "Write scene session manager descriptor failed");
        return MiniWMError::WM_ERROR_IPC_FAILED;
    }
    int32_t ret = remote->SendRequest(
        static_cast<uint32_t>(SceneSessionManagerMessageMini::TRANS_ID_GET_WINDOW_INFO), data, reply, option);
    if (ret != ERR_NONE) {
        SC_LOG_ERROR(LABEL, "Get accessibility window info request failed");
        return MiniWMError::WM_ERROR_IPC_FAILED;
    }
    if (!UnmarshalParcelableVector(reply, infos)) {
        SC_LOG_ERROR(LABEL, "Read accessibility window info failed");
        return MiniWMError::WM_ERROR_IPC_FAILED;
    }
    MiniWMError errCode = MiniWMError::WM_ERROR_IPC_FAILED;
    if (!ReadReplyErrCode(reply, errCode)) {
        return MiniWMError::WM_ERROR_IPC_FAILED;
    }
    return errCode;
}

MiniWMError GetUnreliableWindowInfoInner(int32_t windowId, std::vector<sptr<MiniUnreliableWindowInfo>>& infos)
{
    sptr<IRemoteObject> remote = GetSceneSessionManagerRemote();
    if (remote == nullptr) {
        return MiniWMError::WM_ERROR_SAMGR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(std::u16string(SCENE_SESSION_MANAGER_DESCRIPTOR))) {
        SC_LOG_ERROR(LABEL, "Write scene session manager descriptor failed");
        return MiniWMError::WM_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(windowId)) {
        SC_LOG_ERROR(LABEL, "Write unreliable window id failed");
        return MiniWMError::WM_ERROR_IPC_FAILED;
    }
    int32_t ret = remote->SendRequest(
        static_cast<uint32_t>(SceneSessionManagerMessageMini::TRANS_ID_GET_UNRELIABLE_WINDOW_INFO),
        data, reply, option);
    if (ret != ERR_NONE) {
        SC_LOG_ERROR(LABEL, "Get unreliable window info request failed");
        return MiniWMError::WM_ERROR_IPC_FAILED;
    }
    if (!UnmarshalParcelableVector(reply, infos)) {
        SC_LOG_ERROR(LABEL, "Read unreliable window info failed");
        return MiniWMError::WM_ERROR_IPC_FAILED;
    }
    MiniWMError errCode = MiniWMError::WM_ERROR_IPC_FAILED;
    if (!ReadReplyErrCode(reply, errCode)) {
        return MiniWMError::WM_ERROR_IPC_FAILED;
    }
    return errCode;
}
#endif
}

MiniWMError WMClientMini::GetAccessibilityWindowInfo(std::vector<sptr<MiniAccessibilityWindowInfo>>& infos)
{
    return GetAccessibilityWindowInfoInner(infos);
}

MiniWMError WMClientMini::GetUnreliableWindowInfo(int32_t windowId, std::vector<sptr<MiniUnreliableWindowInfo>>& infos)
{
    return GetUnreliableWindowInfoInner(windowId, infos);
}
}  // namespace Rosen

#ifndef TDD_ENABLE
namespace Security {
namespace SecurityComponent {
bool MiniAccessibilityWindowInfo::Marshalling(Parcel& parcel) const
{
    (void)parcel;
    return false;
}

MiniAccessibilityWindowInfo* MiniAccessibilityWindowInfo::Unmarshalling(Parcel& parcel)
{
    constexpr size_t MAX_TOUCH_HOT_AREAS_LOCAL = 10000;
    auto info = new (std::nothrow) MiniAccessibilityWindowInfo();
    if (info == nullptr) {
        return nullptr;
    }

    bool res = parcel.ReadInt32(info->wid_) && parcel.ReadInt32(info->innerWid_) && parcel.ReadInt32(info->uiNodeId_) &&
        parcel.ReadUint32(info->windowRect_.width_) && parcel.ReadUint32(info->windowRect_.height_) &&
        parcel.ReadInt32(info->windowRect_.posX_) && parcel.ReadInt32(info->windowRect_.posY_) &&
        parcel.ReadBool(info->focused_) && parcel.ReadBool(info->isDecorEnable_) &&
        parcel.ReadUint64(info->displayId_) && parcel.ReadUint32(info->layer_) &&
        parcel.ReadFloat(info->scaleVal_) && parcel.ReadFloat(info->scaleX_) &&
        parcel.ReadFloat(info->scaleY_) && parcel.ReadBool(info->isCompatScaleMode_) &&
        parcel.ReadUint32(info->scaleRect_.width_) && parcel.ReadUint32(info->scaleRect_.height_) &&
        parcel.ReadInt32(info->scaleRect_.posX_) && parcel.ReadInt32(info->scaleRect_.posY_);
    if (!res) {
        delete info;
        return nullptr;
    }

    if (!parcel.ReadUint32(info->mode_) || !parcel.ReadUint32(info->type_) ||
        !parcel.ReadString(info->bundleName_)) {
        delete info;
        return nullptr;
    }
    uint32_t touchHotAreasCnt = 0;
    if (!parcel.ReadUint32(touchHotAreasCnt)) {
        delete info;
        return nullptr;
    }
    if (touchHotAreasCnt > MAX_TOUCH_HOT_AREAS_LOCAL) {
        delete info;
        return nullptr;
    }
    info->touchHotAreas_.reserve(touchHotAreasCnt);
    for (size_t i = 0; i < touchHotAreasCnt; ++i) {
        MiniRect rect;
        if (!parcel.ReadInt32(rect.posX_) || !parcel.ReadInt32(rect.posY_) ||
            !parcel.ReadUint32(rect.width_) || !parcel.ReadUint32(rect.height_)) {
            delete info;
            return nullptr;
        }
        info->touchHotAreas_.emplace_back(rect);
    }
    return info;
}

bool MiniUnreliableWindowInfo::Marshalling(Parcel& parcel) const
{
    (void)parcel;
    return false;
}

MiniUnreliableWindowInfo* MiniUnreliableWindowInfo::Unmarshalling(Parcel& parcel)
{
    auto info = new (std::nothrow) MiniUnreliableWindowInfo();
    if (info == nullptr) {
        return nullptr;
    }

    bool res = parcel.ReadInt32(info->windowId_) && parcel.ReadUint32(info->windowRect_.width_) &&
        parcel.ReadUint32(info->windowRect_.height_) && parcel.ReadInt32(info->windowRect_.posX_) &&
        parcel.ReadInt32(info->windowRect_.posY_) && parcel.ReadUint32(info->zOrder_) &&
        parcel.ReadFloat(info->floatingScale_) && parcel.ReadFloat(info->scaleX_) &&
        parcel.ReadFloat(info->scaleY_);
    if (!res) {
        delete info;
        return nullptr;
    }
    return info;
}
}  // namespace SecurityComponent
}  // namespace Security
#endif
}  // namespace OHOS
