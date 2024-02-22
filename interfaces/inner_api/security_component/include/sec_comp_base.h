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
    MAX_BG_TYPE
};

class __attribute__((visibility("default"))) JsonTagConstants final {
public:
    static const std::string JSON_RECT;
    static const std::string JSON_SC_TYPE;
    static const std::string JSON_NODE_ID;
    static const std::string JSON_RECT_X;
    static const std::string JSON_RECT_Y;
    static const std::string JSON_RECT_WIDTH;
    static const std::string JSON_RECT_HEIGHT;
    static const std::string JSON_WINDOW_RECT;

    static const std::string JSON_SIZE_TAG;
    static const std::string JSON_FONT_SIZE_TAG;
    static const std::string JSON_ICON_SIZE_TAG;
    static const std::string JSON_PADDING_SIZE_TAG;
    static const std::string JSON_PADDING_LEFT_TAG;
    static const std::string JSON_PADDING_TOP_TAG;
    static const std::string JSON_PADDING_RIGHT_TAG;
    static const std::string JSON_PADDING_BOTTOM_TAG;
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

    static const std::string JSON_STYLE_TAG;
    static const std::string JSON_TEXT_TAG;
    static const std::string JSON_ICON_TAG;
    static const std::string JSON_BG_TAG;
    static const std::string JSON_WINDOW_ID;
};

class __attribute__((visibility("default"))) SecCompBase {
public:
    SecCompBase() = default;
    virtual ~SecCompBase() = default;
    bool FromJson(const nlohmann::json& jsonSrc);
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
    DimensionT textIconSpace_ = DEFAULT_DIMENSION;

    // color
    SecCompColor fontColor_;
    SecCompColor iconColor_;
    SecCompColor bgColor_;

    // border
    DimensionT borderWidth_ = DEFAULT_DIMENSION;

    // parent effect
    bool parentEffect_ = false;

    SecCompType type_ = UNKNOWN_SC_TYPE;
    SecCompRect rect_;
    SecCompRect windowRect_;
    bool isValid_ = false;

    int32_t text_ = UNKNOWN_TEXT;
    int32_t icon_ = UNKNOWN_ICON;
    SecCompBackground bg_ = SecCompBackground::UNKNOWN_BG;

    int32_t windowId_ = 0;
    int32_t nodeId_ = 0;
protected:
    virtual bool IsTextIconTypeValid() = 0;
    virtual bool IsCorrespondenceType() = 0;
private:
    bool ParseDimension(const nlohmann::json& json, const std::string& tag, DimensionT& res);
    bool ParseColor(const nlohmann::json& json, const std::string& tag, SecCompColor& res);
    bool ParseBool(const nlohmann::json& json, const std::string& tag, bool& res);
    bool ParsePadding(const nlohmann::json& json, const std::string& tag, PaddingSize& res);
    bool ParseColors(const nlohmann::json& json, const std::string& tag);
    bool ParseBorders(const nlohmann::json& json, const std::string& tag);
    bool ParseSize(const nlohmann::json& json, const std::string& tag);
    bool ParseParent(const nlohmann::json& json, const std::string& tag);
    bool ParseRect(const nlohmann::json& json, const std::string& tag, SecCompRect& rect);
    bool ParseStyle(const nlohmann::json& json, const std::string& tag);
};
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
#endif  // SECURITY_COMPONENT_BASE_H
