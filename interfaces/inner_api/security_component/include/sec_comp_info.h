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

namespace OHOS {
namespace Security {
namespace SecurityComponent {
static constexpr int32_t INVALID_SC_ID = -1;
using DimensionT = double; // unit is vp
static constexpr DimensionT DEFAULT_DIMENSION = 0.0f;
static constexpr DimensionT MIN_FONT_SIZE = 12.0f;
static constexpr DimensionT MIN_ICON_SIZE = 12.0f;
static constexpr DimensionT MIN_PADDING_SIZE = 0.0f;
static constexpr DimensionT MIN_PADDING_WITHOUT_BG = 4.0f;
static constexpr uint32_t MAX_EXTRA_SIZE = 0x1000;

struct PaddingSize {
    DimensionT top = DEFAULT_DIMENSION;
    DimensionT right = DEFAULT_DIMENSION;
    DimensionT bottom = DEFAULT_DIMENSION;
    DimensionT left = DEFAULT_DIMENSION;
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

inline bool IsComponentTypeValid(int32_t type)
{
    if (!(type > UNKNOWN_SC_TYPE) || !(type < MAX_SC_TYPE)) {
        return false;
    }
    return true;
}

inline bool GreatOrEqual(double left, double right)
{
    constexpr double epsilon = -0.001f;
    return (left - right) > epsilon;
}

inline bool GreatNotEqual(double left, double right)
{
    constexpr double epsilon = 0.001f;
    return (left - right) > epsilon;
}

inline bool IsEqual(double left, double right)
{
    constexpr double epsilon = 0.001f;
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
        return (GreatOrEqual(x, x_) && GreatOrEqual((x_ + width_), x) &&
            GreatOrEqual(y, y_) && GreatOrEqual((y_ + height_), y));
    };

    DimensionT x_ = 0.0F;
    DimensionT y_ = 0.0F;
    DimensionT width_ = 0.0F;
    DimensionT height_ = 0.0F;
};

struct ExtraInfo {
    uint32_t dataSize;
    uint8_t* data;
};

struct SecCompClickEvent {
    double touchX;
    double touchY;
    uint64_t timestamp;
    ExtraInfo extraInfo;
};
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
#endif  // SECURITY_COMPONENT_INFO_H
