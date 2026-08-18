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
#include <string>
#include <vector>

namespace OHOS {
namespace Rosen {
using DisplayId = uint64_t;
enum class WindowMode : uint32_t {
    WINDOW_MODE_UNDEFINED = 0,
};
enum class WindowType : uint32_t {
    WINDOW_TYPE_APP_MAIN_WINDOW = 1,
};
enum class WMError : int32_t {
    WM_OK = 0,
};

struct Rect {
    int32_t posX_ = 0;
    int32_t posY_ = 0;
    uint32_t width_ = 0;
    uint32_t height_ = 0;
};

class AccessibilityWindowInfo : public Parcelable {
public:
    AccessibilityWindowInfo() = default;
    ~AccessibilityWindowInfo() = default;
    virtual bool Marshalling(Parcel& parcel) const override
    {
        return true;
    };

    int32_t wid_ = 0;
    int32_t innerWid_ = 0;
    int32_t uiNodeId_ = 0;
    Rect windowRect_;
    bool focused_ { false };
    bool isDecorEnable_ { false };
    DisplayId displayId_ = 0;
    uint32_t layer_ = 0;
    WindowMode mode_ = WindowMode::WINDOW_MODE_UNDEFINED;
    WindowType type_ {};
    float scaleVal_ = 0.0f;
    float scaleX_ = 0.0f;
    float scaleY_ = 0.0f;
    bool isCompatScaleMode_ { false };
    Rect scaleRect_;
    std::string bundleName_;
    std::vector<Rect> touchHotAreas_;
};

class UnreliableWindowInfo : public Parcelable {
public:
    UnreliableWindowInfo() = default;
    ~UnreliableWindowInfo() = default;
    virtual bool Marshalling(Parcel& parcel) const override
    {
        return true;
    };
    int32_t windowId_ { 0 };
    Rect windowRect_;
    uint32_t zOrder_ { 0 };
    float floatingScale_ { 1.0f };
    float scaleX_ { 1.0f };
    float scaleY_ { 1.0f };
};

#ifndef FUZZ_ENABLE
class WindowManager {
public:
    static WindowManager& GetInstance()
    {
        static WindowManager instance;
        return instance;
    };

    static WindowManager& GetInstance(int32_t userId)
    {
        WindowManager& instance = GetInstance();
        instance.lastUserId_ = userId;
        return instance;
    };

    WMError GetAccessibilityWindowInfo(std::vector<sptr<Rosen::AccessibilityWindowInfo>>& list) const
    {
        list = list_;
        return result_;
    };
    WMError GetUnreliableWindowInfo(int32_t windowId, std::vector<sptr<UnreliableWindowInfo>>& infos) const
    {
        lastWindowId_ = windowId;
        infos = info_;
        return result_;
    }

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

        std::vector<sptr<UnreliableWindowInfo>> info;
        sptr<UnreliableWindowInfo> unreliableWinInfo = new UnreliableWindowInfo();
        unreliableWinInfo->windowId_ = 0;
        unreliableWinInfo->zOrder_ = 0;
        unreliableWinInfo->floatingScale_ = 0.0;
        info.emplace_back(unreliableWinInfo);
        info_ = info;
    };

    std::vector<sptr<Rosen::AccessibilityWindowInfo>> list_;
    std::vector<sptr<Rosen::UnreliableWindowInfo>> info_;
    WMError result_ = OHOS::Rosen::WMError::WM_OK;
    int32_t lastUserId_ = -1;
    mutable int32_t lastWindowId_ = -1;
private:
    ~WindowManager() {};
};
#else
class WindowManager {
public:
    static WindowManager& GetInstance()
    {
        static WindowManager instance;
        return instance;
    };

    static WindowManager& GetInstance(int32_t userId)
    {
        WindowManager& instance = GetInstance();
        instance.lastUserId_ = userId;
        return instance;
    };

    WMError GetAccessibilityWindowInfo(std::vector<sptr<Rosen::AccessibilityWindowInfo>>& list) const
    {
        sptr<AccessibilityWindowInfo> compWin = sptr<AccessibilityWindowInfo>::MakeSptr();
        compWin->wid_ = 0;
        compWin->layer_ = 0;
        compWin->scaleVal_ = 1.0;
        list.emplace_back(compWin);
        return OHOS::Rosen::WMError::WM_OK;
    };
    
    WMError GetUnreliableWindowInfo(int32_t windowId, std::vector<sptr<UnreliableWindowInfo>>& infos) const
    {
        sptr<UnreliableWindowInfo> compoLayer = sptr<UnreliableWindowInfo>::MakeSptr();
        compoLayer->windowId_ = 0;
        compoLayer->zOrder_ = 0;
        compoLayer->floatingScale_ = 1.0;
        infos.emplace_back(compoLayer);
        sptr<UnreliableWindowInfo> coverLayer = sptr<UnreliableWindowInfo>::MakeSptr();
        // window is 128*128, cover window is 96*96
        coverLayer->windowRect_ = Rect{0, 0, 96, 96};
        coverLayer->windowId_ = 1;
        coverLayer->zOrder_ = 1;
        coverLayer->floatingScale_ = 1.0;
        infos.emplace_back(coverLayer);
        return OHOS::Rosen::WMError::WM_OK;
    }

    WindowManager() {};
    int32_t lastUserId_ = -1;
private:
    ~WindowManager() {};
};
#endif // FUZZ_ENABLE
} // namespace Rosen
} // namespace OHOS
#endif // SECURITY_COMPONENT_MOCK_WINDOW_MANAGER_H
