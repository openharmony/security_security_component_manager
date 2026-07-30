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
#ifndef I_SECURITY_COMPONENT_WM_MINI_CLIENT_H
#define I_SECURITY_COMPONENT_WM_MINI_CLIENT_H

#include <cstdint>
#include <string>
#include <vector>

#include <parcel.h>
#include <refbase.h>

namespace OHOS {
namespace Security {
namespace SecurityComponent {
using DisplayId = uint64_t;
using WindowMode = uint32_t;
using WindowType = uint32_t;

enum class MiniWMError : int32_t {
    WM_OK = 0,
    WM_DO_NOTHING,
    WM_ERROR_NO_MEM,
    WM_ERROR_DESTROYED_OBJECT,
    WM_ERROR_INVALID_WINDOW,
    WM_ERROR_INVALID_WINDOW_MODE_OR_SIZE,
    WM_ERROR_INVALID_OPERATION,
    WM_ERROR_INVALID_PERMISSION,
    WM_ERROR_NOT_SYSTEM_APP,
    WM_ERROR_NO_REMOTE_ANIMATION,
    WM_ERROR_INVALID_DISPLAY,
    WM_ERROR_INVALID_PARENT,
    WM_ERROR_INVALID_OP_IN_CUR_STATUS,
    WM_ERROR_REPEAT_OPERATION,
    WM_ERROR_INVALID_SESSION,
    WM_ERROR_INVALID_CALLING,
    WM_ERROR_SYSTEM_ABNORMALLY,
    WM_ERROR_DEVICE_NOT_SUPPORT = 801,
    WM_ERROR_NEED_REPORT_BASE = 1000,
    WM_ERROR_NULLPTR,
    WM_ERROR_INVALID_TYPE,
    WM_ERROR_INVALID_PARAM,
    WM_ERROR_SAMGR,
    WM_ERROR_IPC_FAILED,
};

class MiniRect {
public:
    int32_t posX_ = 0;
    int32_t posY_ = 0;
    uint32_t width_ = 0;
    uint32_t height_ = 0;
};

class MiniAccessibilityWindowInfo : public Parcelable {
public:
    MiniAccessibilityWindowInfo() = default;
    ~MiniAccessibilityWindowInfo() override = default;
    bool Marshalling(Parcel& parcel) const override;
    static MiniAccessibilityWindowInfo* Unmarshalling(Parcel& parcel);

    int32_t wid_ = 0;
    int32_t innerWid_ = 0;
    int32_t uiNodeId_ = 0;
    MiniRect windowRect_;
    bool focused_ { false };
    bool isDecorEnable_ { false };
    DisplayId displayId_ = 0;
    uint32_t layer_ = 0;
    WindowMode mode_ = 0;
    WindowType type_ = 0;
    float scaleVal_ = 0.0f;
    float scaleX_ = 0.0f;
    float scaleY_ = 0.0f;
    bool isCompatScaleMode_ { false };
    MiniRect scaleRect_;
    std::string bundleName_;
    std::vector<MiniRect> touchHotAreas_;
};

class MiniUnreliableWindowInfo : public Parcelable {
public:
    MiniUnreliableWindowInfo() = default;
    ~MiniUnreliableWindowInfo() override = default;
    bool Marshalling(Parcel& parcel) const override;
    static MiniUnreliableWindowInfo* Unmarshalling(Parcel& parcel);

    int32_t windowId_ { 0 };
    MiniRect windowRect_;
    uint32_t zOrder_ { 0 };
    float floatingScale_ { 1.0f };
    float scaleX_ { 1.0f };
    float scaleY_ { 1.0f };
};
}  // namespace SecurityComponent
}  // namespace Security

namespace Rosen {
class WMClientMini {
public:
    static Security::SecurityComponent::MiniWMError GetAccessibilityWindowInfo(int32_t userId,
        std::vector<sptr<Security::SecurityComponent::MiniAccessibilityWindowInfo>>& infos);
    static Security::SecurityComponent::MiniWMError GetUnreliableWindowInfo(int32_t windowId, int32_t userId,
        std::vector<sptr<Security::SecurityComponent::MiniUnreliableWindowInfo>>& infos);
};
}  // namespace Rosen
}  // namespace OHOS

#endif  // I_SECURITY_COMPONENT_WM_MINI_CLIENT_H
