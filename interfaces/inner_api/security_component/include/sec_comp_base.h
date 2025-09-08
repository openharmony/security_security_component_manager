/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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
#ifndef SECURITY_COMPONENT_BASE_H
#define SECURITY_COMPONENT_BASE_H

#include "nlohmann/json.hpp"
#include "sec_comp_info.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
constexpr int32_t UNKNOWN_TEXT = -2;
constexpr int32_t NO_TEXT = -1;
constexpr int32_t UNKNOWN_ICON = -2;
constexpr int32_t NO_ICON = -1;

enum class SecCompBackground {
    UNKNOWN_BG = -2,
    NO_BG_TYPE = -1,
    CAPSULE = 0,
    CIRCLE = 1,
    NORMAL = 2,
    ROUNDED_RECTANGLE = 8,
    MAX_BG_TYPE
};

class __attribute__((visibility("default"))) JsonTagConstants final {
public:
    static const std::string JSON_RECT;
    static const std::string JSON_SC_TYPE;
    static const std::string JSON_NODE_ID;
    static const std::string JSON_IS_WEARABLE;
    static const std::string JSON_RECT_X;
    static const std::string JSON_RECT_Y;
    static const std::string JSON_RECT_WIDTH;
    static const std::string JSON_RECT_HEIGHT;
    static const std::string JSON_WINDOW_RECT;

    static const std::string JSON_SIZE_TAG;
    static const std::string JSON_FONT_SIZE_TAG;
    static const std::string JSON_ICON_SIZE_TAG;
    static const std::string JSON_PADDING_SIZE_TAG;
    static const std::string JSON_LEFT_TAG;
    static const std::string JSON_TOP_TAG;
    static const std::string JSON_RIGHT_TAG;
    static const std::string JSON_BOTTOM_TAG;
    static const std::string JSON_BORDER_RADIUS_TAG;
    static const std::string JSON_LEFT_TOP_TAG;
    static const std::string JSON_LEFT_BOTTOM_TAG;
    static const std::string JSON_RIGHT_TOP_TAG;
    static const std::string JSON_RIGHT_BOTTOM_TAG;
    static const std::string JSON_TEXT_ICON_PADDING_TAG;
    static const std::string JSON_RECT_WIDTH_TAG;
    static const std::string JSON_RECT_HEIGHT_TAG;
    static const std::string JSON_COLORS_TAG;
    static const std::string JSON_FONT_COLOR_TAG;
    static const std::string JSON_ICON_COLOR_TAG;
    static const std::string JSON_BG_COLOR_TAG;

    static const std::string JSON_BORDER_TAG;
    static const std::string JSON_BORDER_WIDTH_TAG;
    static const std::string JSON_PARENT_TAG;
    static const std::string JSON_PARENT_EFFECT_TAG;
    static const std::string JSON_IS_CLIPPED_TAG;
    static const std::string JSON_TOP_CLIP_TAG;
    static const std::string JSON_BOTTOM_CLIP_TAG;
    static const std::string JSON_LEFT_CLIP_TAG;
    static const std::string JSON_RIGHT_CLIP_TAG;
    static const std::string JSON_PARENT_TAG_TAG;

    static const std::string JSON_STYLE_TAG;
    static const std::string JSON_TEXT_TAG;
    static const std::string JSON_ICON_TAG;
    static const std::string JSON_BG_TAG;
    static const std::string JSON_WINDOW_ID;
    static const std::string JSON_DISPLAY_ID;
    static const std::string JSON_CROSS_AXIS_STATE;
    static const std::string JSON_IS_ICON_EXCEEDED_TAG;
    static const std::string JSON_IS_BORDER_COVERED_TAG;
    static const std::string JSON_NON_COMPATIBLE_CHANGE_TAG;
    static const std::string JSON_LINEAR_GRADIENT_BLUR_RADIUS_TAG;
    static const std::string JSON_FOREGROUND_BLUR_RADIUS_TAG;
    static const std::string JSON_IS_OVERLAY_TEXT_SET_TAG;
    static const std::string JSON_IS_OVERLAY_NODE_SET_TAG;
    static const std::string JSON_IS_CUSTOMIZABLE;
    static const std::string JSON_TIP_POSITION;
};

class __attribute__((visibility("default"))) SecCompBase {
public:
    SecCompBase() = default;
    virtual ~SecCompBase() = default;
    bool FromJson(const nlohmann::json& jsonSrc, std::string& message, bool isClicked);
    void ToJson(nlohmann::json& jsonRes) const;
    std::string ToJsonStr(void) const;
    virtual bool CompareComponentBasicInfo(SecCompBase *other, bool isRectCheck) const;
    void SetValid(bool valid)
    {
        isValid_ = valid;
    };

    bool GetValid()
    {
        return isValid_;
    };

    // size
    DimensionT fontSize_ = DEFAULT_DIMENSION;
    DimensionT iconSize_ = DEFAULT_DIMENSION;
    PaddingSize padding_;
    BorderRadius borderRadius_;
    DimensionT textIconSpace_ = DEFAULT_DIMENSION;

    // color
    SecCompColor fontColor_;
    SecCompColor iconColor_;
    SecCompColor bgColor_;

    // border
    DimensionT borderWidth_ = DEFAULT_DIMENSION;

    // parent effect
    bool parentEffect_ = false;
    bool isClipped_ = false;
    DimensionT topClip_;
    DimensionT bottomClip_;
    DimensionT leftClip_;
    DimensionT rightClip_;
    std::string parentTag_;

    SecCompType type_ = UNKNOWN_SC_TYPE;
    SecCompRect rect_;
    SecCompRect windowRect_;
    bool isValid_ = false;

    int32_t text_ = UNKNOWN_TEXT;
    int32_t icon_ = UNKNOWN_ICON;
    SecCompBackground bg_ = SecCompBackground::UNKNOWN_BG;

    bool hasNonCompatibleChange_ = false;
    double blurRadius_ = 0.0;
    double foregroundBlurRadius_ = 0.0;
    bool isOverlayTextSet_ = false;
    bool isOverlayNodeCovered_ = false;
    int32_t windowId_ = 0;
    uint64_t displayId_ = 0;
    int32_t nodeId_ = 0;
    CrossAxisState crossAxisState_ = CrossAxisState::STATE_INVALID;
    bool isIconExceeded_ = false;
    bool isBorderCovered_ = false;
    bool isWearableDevice_ = false;
    TipPosition tipPosition_ = TipPosition::ABOVE_BOTTOM;
    bool isCustomizable_ = false;
    float scale_ = 1.0f;
protected:
    virtual bool IsTextIconTypeValid(std::string& message, bool isClicked) = 0;
    virtual bool IsCorrespondenceType() = 0;
private:
    bool ParseNonCompatibleChange(const nlohmann::json& json);
    bool ParseDimension(const nlohmann::json& json, const std::string& tag, DimensionT& res);
    bool ParseColor(const nlohmann::json& json, const std::string& tag, SecCompColor& res);
    bool ParseBool(const nlohmann::json& json, const std::string& tag, bool& res);
    bool ParseString(const nlohmann::json& json, const std::string& tag, std::string& res);
    bool ParsePadding(const nlohmann::json& json, const std::string& tag, PaddingSize& res);
    bool ParseBorderRadius(const nlohmann::json& json, const std::string& tag, BorderRadius& res);
    bool ParseColors(const nlohmann::json& json, const std::string& tag);
    bool ParseBorders(const nlohmann::json& json, const std::string& tag);
    bool ParseSize(const nlohmann::json& json, const std::string& tag);
    bool ParseParent(const nlohmann::json& json, const std::string& tag);
    bool ParseRect(const nlohmann::json& json, const std::string& tag, SecCompRect& rect);
    bool ParseStyle(const nlohmann::json& json, const std::string& tag, std::string& message, bool isClicked);
    bool ParseType(const nlohmann::json& json, const std::string& tag);
    bool ParseValue(const nlohmann::json& json, const std::string& tag, int32_t& value);
    bool ParseDisplayId(const nlohmann::json& json, const std::string& tag);
    bool ParseCrossAxisState(const nlohmann::json& json, const std::string& tag);
    bool ParseTipPosition(const nlohmann::json& json, const std::string& tag);
    bool ParseWearable(const nlohmann::json& json, const std::string& tag);
    void ToJsonRect(nlohmann::json& jsonRes) const;
    void ToJsonSize(nlohmann::json& jsonRes) const;
    bool ParseComponentInfo(const nlohmann::json& json, std::string& message, bool isClicked);
    bool ParseWindowInfo(const nlohmann::json& json);
    bool ParseDisplayInfo(const nlohmann::json& json);
    bool ParseCustomInfo(const nlohmann::json& json);
};
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
#endif  // SECURITY_COMPONENT_BASE_H
