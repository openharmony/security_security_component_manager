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

#include "sec_comp_log.h"
#include "wm_mini_client_adapter.h"

namespace OHOS {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "WMClientMini"};
// Keep aligned with the WMS limit and prevent excessive allocation from malformed IPC data.
constexpr size_t MAX_TOUCH_HOT_AREAS = 10000;
}

namespace Rosen {
using Security::SecurityComponent::MiniAccessibilityWindowInfo;
using Security::SecurityComponent::MiniUnreliableWindowInfo;
using Security::SecurityComponent::MiniWMError;

MiniWMError WMClientMini::GetAccessibilityWindowInfo(int32_t userId,
    std::vector<sptr<MiniAccessibilityWindowInfo>>& infos)
{
    return WmMiniClientAdapter::GetWindowInfo(userId, infos);
}

MiniWMError WMClientMini::GetUnreliableWindowInfo(int32_t windowId, int32_t userId,
    std::vector<sptr<MiniUnreliableWindowInfo>>& infos)
{
    return WmMiniClientAdapter::GetUnreliableWindowInfo(windowId, userId, infos);
}
}  // namespace Rosen

namespace Security {
namespace SecurityComponent {
namespace {
bool UnmarshalWindowInfoFixedFields(Parcel& parcel, MiniAccessibilityWindowInfo& info)
{
    bool res = parcel.ReadInt32(info.wid_) && parcel.ReadInt32(info.innerWid_) && parcel.ReadInt32(info.uiNodeId_) &&
        parcel.ReadUint32(info.windowRect_.width_) && parcel.ReadUint32(info.windowRect_.height_) &&
        parcel.ReadInt32(info.windowRect_.posX_) && parcel.ReadInt32(info.windowRect_.posY_) &&
        parcel.ReadBool(info.focused_) && parcel.ReadBool(info.isDecorEnable_) &&
        parcel.ReadUint64(info.displayId_) && parcel.ReadUint32(info.layer_) &&
        parcel.ReadFloat(info.scaleVal_) && parcel.ReadFloat(info.scaleX_) &&
        parcel.ReadFloat(info.scaleY_) && parcel.ReadBool(info.isCompatScaleMode_) &&
        parcel.ReadUint32(info.scaleRect_.width_) && parcel.ReadUint32(info.scaleRect_.height_) &&
        parcel.ReadInt32(info.scaleRect_.posX_) && parcel.ReadInt32(info.scaleRect_.posY_);
    if (!res) {
        SC_LOG_ERROR(LABEL, "Unmarshal window info fixed fields failed");
    }
    return res;
}

bool UnmarshalWindowInfoTailFields(Parcel& parcel, MiniAccessibilityWindowInfo& info)
{
    if (!parcel.ReadUint32(info.mode_) || !parcel.ReadUint32(info.type_) ||
        !parcel.ReadString(info.bundleName_)) {
        SC_LOG_ERROR(LABEL, "Unmarshal window info tail fields failed");
        return false;
    }
    uint32_t touchHotAreasCnt = 0;
    if (!parcel.ReadUint32(touchHotAreasCnt)) {
        SC_LOG_ERROR(LABEL, "Read touch hot area count failed");
        return false;
    }
    if (touchHotAreasCnt > MAX_TOUCH_HOT_AREAS) {
        SC_LOG_ERROR(LABEL, "Invalid touch hot area count=%{public}u", touchHotAreasCnt);
        return false;
    }
    info.touchHotAreas_.reserve(touchHotAreasCnt);
    for (size_t i = 0; i < touchHotAreasCnt; ++i) {
        MiniRect rect;
        if (!parcel.ReadInt32(rect.posX_) || !parcel.ReadInt32(rect.posY_) ||
            !parcel.ReadUint32(rect.width_) || !parcel.ReadUint32(rect.height_)) {
            SC_LOG_ERROR(LABEL, "Unmarshal touch hot area failed");
            return false;
        }
        info.touchHotAreas_.emplace_back(rect);
    }
    return true;
}
}

bool MiniAccessibilityWindowInfo::Marshalling(Parcel& parcel) const
{
    (void)parcel;
    return false;
}

MiniAccessibilityWindowInfo* MiniAccessibilityWindowInfo::Unmarshalling(Parcel& parcel)
{
    auto info = Rosen::WmMiniClientAdapter::CreateWindowInfo();
    if (info == nullptr) {
        SC_LOG_ERROR(LABEL, "Create window info failed");
        return nullptr;
    }
    if (!UnmarshalWindowInfoFixedFields(parcel, *info) || !UnmarshalWindowInfoTailFields(parcel, *info)) {
        delete info;
        info = nullptr;
        return nullptr;
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
    auto info = Rosen::WmMiniClientAdapter::CreateUnreliableWindowInfo();
    if (info == nullptr) {
        SC_LOG_ERROR(LABEL, "Create unreliable window info failed");
        return nullptr;
    }

    bool res = parcel.ReadInt32(info->windowId_) && parcel.ReadUint32(info->windowRect_.width_) &&
        parcel.ReadUint32(info->windowRect_.height_) && parcel.ReadInt32(info->windowRect_.posX_) &&
        parcel.ReadInt32(info->windowRect_.posY_) && parcel.ReadUint32(info->zOrder_) &&
        parcel.ReadFloat(info->floatingScale_) && parcel.ReadFloat(info->scaleX_) &&
        parcel.ReadFloat(info->scaleY_);
    if (!res) {
        SC_LOG_ERROR(LABEL, "Unmarshal unreliable window info failed");
        delete info;
        info = nullptr;
        return nullptr;
    }
    return info;
}
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
