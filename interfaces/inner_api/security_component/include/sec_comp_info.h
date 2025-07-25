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

#ifndef SECURITY_COMPONENT_INFO_H
#define SECURITY_COMPONENT_INFO_H

#include <cstdint>
#include <functional>
#include <string>

namespace OHOS {
namespace Security {
namespace SecurityComponent {
static constexpr int32_t INVALID_SC_ID = -1;
using DimensionT = double; // unit is vp
static constexpr DimensionT DEFAULT_DIMENSION = 0.0;
static constexpr DimensionT MIN_FONT_SIZE_WITHOUT_ICON = 12.0;
static constexpr DimensionT MIN_FONT_SIZE_WITH_ICON = 10.0;
static constexpr DimensionT MIN_ICON_SIZE = 12.0;
static constexpr DimensionT MIN_PADDING_SIZE = 0.0;
static constexpr DimensionT MIN_PADDING_WITHOUT_BG = 4.0;
static constexpr uint32_t MAX_EXTRA_SIZE = 0x1000;

static constexpr int32_t KEY_SPACE = 2050;
static constexpr int32_t KEY_ENTER = 2054;
static constexpr int32_t KEY_NUMPAD_ENTER = 2119;
using OnFirstUseDialogCloseFunc = std::function<void(int32_t)>;

struct PaddingSize {
    DimensionT top = DEFAULT_DIMENSION;
    DimensionT right = DEFAULT_DIMENSION;
    DimensionT bottom = DEFAULT_DIMENSION;
    DimensionT left = DEFAULT_DIMENSION;
};

struct BorderRadius {
    DimensionT leftTop = DEFAULT_DIMENSION;
    DimensionT rightTop = DEFAULT_DIMENSION;
    DimensionT leftBottom = DEFAULT_DIMENSION;
    DimensionT rightBottom = DEFAULT_DIMENSION;
};

enum SecCompType {
    UNKNOWN_SC_TYPE = 0,
    LOCATION_COMPONENT,
    PASTE_COMPONENT,
    SAVE_COMPONENT,
    MAX_SC_TYPE
};

union SecCompColor {
    struct {
        uint8_t blue;
        uint8_t green;
        uint8_t red;
        uint8_t alpha;
    } argb;
    uint32_t value;
};

enum CrossAxisState {
    STATE_INVALID = 0,
    STATE_CROSS,
    STATE_NO_CROSS,
};

enum TipPosition {
    ABOVE_BOTTOM = 0,
    BELOW_TOP,
};

enum NotifyType {
    DIALOG = 0,
    TOAST,
};

inline bool IsComponentTypeValid(int32_t type)
{
    return (type > UNKNOWN_SC_TYPE && type < MAX_SC_TYPE);
}

inline bool GreatOrEqual(double left, double right)
{
    constexpr double epsilon = -0.001;
    return (left - right) > epsilon;
}

inline bool GreatNotEqual(double left, double right)
{
    constexpr double epsilon = 0.001;
    return (left - right) > epsilon;
}

inline bool IsEqual(double left, double right)
{
    constexpr double epsilon = 0.001;
    if (left > right) {
        return (left - right) < epsilon;
    } else if (right > left) {
        return (right - left) < epsilon;
    } else {
        return true;
    }
}

class SecCompRect {
public:
    SecCompRect() = default;
    ~SecCompRect() = default;

    bool IsInRect(double x, double y) const
    {
        return (GreatOrEqual(x, x_ - 1.0) && GreatOrEqual((x_ + width_), x - 1.0) &&
            GreatOrEqual(y, y_ - 1.0) && GreatOrEqual((y_ + height_), y - 1.0));
    };

    bool operator==(const SecCompRect& other) const
    {
        return (IsEqual(x_, other.x_)) && (IsEqual(y_, other.y_)) &&
            (IsEqual(width_, other.width_)) && (IsEqual(height_, other.height_));
    }

    DimensionT x_ = 0.0;
    DimensionT y_ = 0.0;
    DimensionT width_ = 0.0;
    DimensionT height_ = 0.0;
    BorderRadius borderRadius_;
};

struct ExtraInfo {
    uint32_t dataSize;
    uint8_t* data;
};

enum class ClickEventType : int32_t {
    UNKNOWN_EVENT_TYPE,
    POINT_EVENT_TYPE,
    KEY_EVENT_TYPE,
    ACCESSIBILITY_EVENT_TYPE
};

struct SecCompPointEvent {
    double touchX;
    double touchY;
    uint64_t timestamp;
};

struct SecCompKeyEvent {
    uint64_t timestamp;
    int32_t keyCode;
};

struct SecCompAccessibilityEvent {
    int64_t timestamp = 0;
    int64_t componentId = 0;
};

struct SecCompClickEvent {
    ClickEventType type;
    union {
        SecCompPointEvent point;
        SecCompKeyEvent key;
        SecCompAccessibilityEvent accessibility;
    };
    ExtraInfo extraInfo;
};

struct SecCompInfo {
    int32_t scId;
    std::string componentInfo;
    SecCompClickEvent clickInfo;
};
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
#endif  // SECURITY_COMPONENT_INFO_H
