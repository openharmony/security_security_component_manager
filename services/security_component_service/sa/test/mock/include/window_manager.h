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
#ifndef SECURITY_COMPONENT_MOCK_WINDOW_MANAGER_H
#define SECURITY_COMPONENT_MOCK_WINDOW_MANAGER_H
#include <cstdint>
#include <iremote_object.h>
#include <refbase.h>
#include <vector>

namespace OHOS {
namespace Rosen {
using DisplayId = uint64_t;
using WindowMode = uint32_t;
using WindowType = uint32_t;
enum class WMError : int32_t {
    WM_OK = 0,
};

struct Rect {
    int32_t posX_;
    int32_t posY_;
    uint32_t width_;
    uint32_t height_;
};

class AccessibilityWindowInfo : public Parcelable {
public:
    AccessibilityWindowInfo() = default;
    ~AccessibilityWindowInfo() = default;
    virtual bool Marshalling(Parcel& parcel) const override
    {
        return true;
    };

    int32_t wid_;
    int32_t innerWid_;
    int32_t uiNodeId_;
    Rect windowRect_;
    bool focused_ { false };
    bool isDecorEnable_ { false };
    DisplayId displayId_;
    uint32_t layer_;
    WindowMode mode_;
    WindowType type_;
    float scaleVal_;
};

class WindowManager {
public:
    static WindowManager& GetInstance()
    {
        static WindowManager instance;
        return instance;
    };

    WMError GetAccessibilityWindowInfo(std::vector<sptr<Rosen::AccessibilityWindowInfo>>& list)
    {
        list = list_;
        return result_;
    };

    WindowManager() {};

    void SetDefaultSecCompScene()
    {
        result_ = OHOS::Rosen::WMError::WM_OK;
        std::vector<sptr<AccessibilityWindowInfo>> list;
        sptr<AccessibilityWindowInfo> compWin = new AccessibilityWindowInfo();
        compWin->wid_ = 0;
        compWin->layer_ = 0;
        compWin->scaleVal_ = 0.0;
        list.emplace_back(compWin);
        list_ = list;
    };

    std::vector<sptr<Rosen::AccessibilityWindowInfo>> list_;
    WMError result_ = OHOS::Rosen::WMError::WM_OK;
private:
    ~WindowManager() {};
};
} // namespace Rosen
} // namespace OHOS
#endif // SECURITY_COMPONENT_MOCK_WINDOW_MANAGER_H
