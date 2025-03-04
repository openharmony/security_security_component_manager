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
#include "sec_comp_base.h"

#include "sec_comp_err.h"
#include "sec_comp_log.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "SecCompBase"};
}

const std::string JsonTagConstants::JSON_RECT = "rect";
const std::string JsonTagConstants::JSON_SC_TYPE = "type";
const std::string JsonTagConstants::JSON_NODE_ID = "nodeId";
const std::string JsonTagConstants::JSON_RECT_X = "x";
const std::string JsonTagConstants::JSON_RECT_Y = "y";
const std::string JsonTagConstants::JSON_RECT_WIDTH = "width";
const std::string JsonTagConstants::JSON_RECT_HEIGHT = "height";
const std::string JsonTagConstants::JSON_WINDOW_RECT = "windowRect";
const std::string JsonTagConstants::JSON_SIZE_TAG = "size";
const std::string JsonTagConstants::JSON_FONT_SIZE_TAG = "fontSize";
const std::string JsonTagConstants::JSON_ICON_SIZE_TAG = "iconSize";
const std::string JsonTagConstants::JSON_PADDING_SIZE_TAG = "paddingSize";
const std::string JsonTagConstants::JSON_PADDING_LEFT_TAG = "left";
const std::string JsonTagConstants::JSON_PADDING_TOP_TAG = "top";
const std::string JsonTagConstants::JSON_PADDING_RIGHT_TAG = "right";
const std::string JsonTagConstants::JSON_PADDING_BOTTOM_TAG = "bottom";
const std::string JsonTagConstants::JSON_TEXT_ICON_PADDING_TAG = "textIconSpace";
const std::string JsonTagConstants::JSON_RECT_WIDTH_TAG = "width";
const std::string JsonTagConstants::JSON_RECT_HEIGHT_TAG = "height";
const std::string JsonTagConstants::JSON_COLORS_TAG = "colors";
const std::string JsonTagConstants::JSON_FONT_COLOR_TAG = "fontColor";
const std::string JsonTagConstants::JSON_ICON_COLOR_TAG = "iconColor";
const std::string JsonTagConstants::JSON_BG_COLOR_TAG = "bgColor";
const std::string JsonTagConstants::JSON_BORDER_TAG = "border";
const std::string JsonTagConstants::JSON_BORDER_WIDTH_TAG = "borderWidth";
const std::string JsonTagConstants::JSON_PARENT_TAG = "parent";
const std::string JsonTagConstants::JSON_PARENT_EFFECT_TAG = "parentEffect";
const std::string JsonTagConstants::JSON_IS_CLIPPED_TAG = "isClipped";
const std::string JsonTagConstants::JSON_TOP_CLIP_TAG = "topClip";
const std::string JsonTagConstants::JSON_BOTTOM_CLIP_TAG = "bottomClip";
const std::string JsonTagConstants::JSON_LEFT_CLIP_TAG = "leftClip";
const std::string JsonTagConstants::JSON_RIGHT_CLIP_TAG = "rightClip";
const std::string JsonTagConstants::JSON_PARENT_TAG_TAG = "parentTag";
const std::string JsonTagConstants::JSON_STYLE_TAG = "style";
const std::string JsonTagConstants::JSON_TEXT_TAG = "text";
const std::string JsonTagConstants::JSON_ICON_TAG = "icon";
const std::string JsonTagConstants::JSON_BG_TAG = "bg";
const std::string JsonTagConstants::JSON_WINDOW_ID = "windowId";
const std::string JsonTagConstants::JSON_DISPLAY_ID = "displayId";
const std::string JsonTagConstants::JSON_CROSS_AXIS_STATE = "crossAxisState";

bool SecCompBase::ParseDimension(const nlohmann::json& json, const std::string& tag, DimensionT& res)
{
    if ((json.find(tag) == json.end()) || !json.at(tag).is_number_float()) {
        SC_LOG_ERROR(LABEL, "json: %{public}s tag invalid.", tag.c_str());
        return false;
    }

    res = json.at(tag).get<double>();
    return true;
}

bool SecCompBase::ParseColor(const nlohmann::json& json, const std::string& tag, SecCompColor& res)
{
    if ((json.find(tag) == json.end()) || !json.at(tag).is_number()) {
        SC_LOG_ERROR(LABEL, "json: %{public}s tag invalid.", tag.c_str());
        return false;
    }

    res.value = json.at(tag).get<uint32_t>();
    return true;
}

bool SecCompBase::ParseBool(const nlohmann::json& json, const std::string& tag, bool& res)
{
    if ((json.find(tag) == json.end()) || !json.at(tag).is_boolean()) {
        SC_LOG_ERROR(LABEL, "json: %{public}s tag invalid.", tag.c_str());
        return false;
    }

    res = json.at(tag).get<bool>();
    return true;
}

bool SecCompBase::ParseString(const nlohmann::json& json, const std::string& tag, std::string& res)
{
    if ((json.find(tag) == json.end()) || !json.at(tag).is_string()) {
        SC_LOG_ERROR(LABEL, "json: %{public}s tag invalid.", tag.c_str());
        return false;
    }

    res = json.at(tag).get<std::string>();
    return true;
}

bool SecCompBase::ParsePadding(const nlohmann::json& json, const std::string& tag, PaddingSize& res)
{
    if ((json.find(tag) == json.end()) || !json.at(tag).is_object()) {
        SC_LOG_ERROR(LABEL, "json: %{public}s tag invalid.", tag.c_str());
        return false;
    }

    auto jsonPadding = json.at(tag);
    if (!ParseDimension(jsonPadding, JsonTagConstants::JSON_PADDING_TOP_TAG, res.top)) {
        return false;
    }
    if (!ParseDimension(jsonPadding, JsonTagConstants::JSON_PADDING_RIGHT_TAG, res.right)) {
        return false;
    }
    if (!ParseDimension(jsonPadding, JsonTagConstants::JSON_PADDING_BOTTOM_TAG, res.bottom)) {
        return false;
    }
    if (!ParseDimension(jsonPadding, JsonTagConstants::JSON_PADDING_LEFT_TAG, res.left)) {
        return false;
    }
    return true;
}

bool SecCompBase::ParseColors(const nlohmann::json& json, const std::string& tag)
{
    if ((json.find(tag) == json.end()) || !json.at(tag).is_object()) {
        SC_LOG_ERROR(LABEL, "json: %{public}s tag invalid.", tag.c_str());
        return false;
    }
    auto jsonColors = json.at(tag);
    if (!ParseColor(jsonColors, JsonTagConstants::JSON_FONT_COLOR_TAG, fontColor_)) {
        return false;
    }
    if (!ParseColor(jsonColors, JsonTagConstants::JSON_ICON_COLOR_TAG, iconColor_)) {
        return false;
    }
    if (!ParseColor(jsonColors, JsonTagConstants::JSON_BG_COLOR_TAG, bgColor_)) {
        return false;
    }
    return true;
}

bool SecCompBase::ParseBorders(const nlohmann::json& json, const std::string& tag)
{
    if ((json.find(tag) == json.end()) || !json.at(tag).is_object()) {
        SC_LOG_ERROR(LABEL, "json: %{public}s tag invalid.", tag.c_str());
        return false;
    }
    auto jsonBorder = json.at(tag);
    return ParseDimension(jsonBorder, JsonTagConstants::JSON_BORDER_WIDTH_TAG, borderWidth_);
}

bool SecCompBase::ParseSize(const nlohmann::json& json, const std::string& tag)
{
    if ((json.find(tag) == json.end()) || !json.at(tag).is_object()) {
        SC_LOG_ERROR(LABEL, "json: %{public}s tag invalid.", tag.c_str());
        return false;
    }

    auto jsonSize = json.at(tag);
    if (!ParseDimension(jsonSize, JsonTagConstants::JSON_FONT_SIZE_TAG, fontSize_)) {
        return false;
    }

    if (!ParseDimension(jsonSize, JsonTagConstants::JSON_ICON_SIZE_TAG, iconSize_)) {
        return false;
    }

    if (!ParseDimension(jsonSize, JsonTagConstants::JSON_TEXT_ICON_PADDING_TAG, textIconSpace_)) {
        return false;
    }

    if (!ParsePadding(jsonSize, JsonTagConstants::JSON_PADDING_SIZE_TAG, padding_)) {
        return false;
    }

    return true;
}

bool SecCompBase::ParseParent(const nlohmann::json& json, const std::string& tag)
{
    if ((json.find(tag) == json.end()) || !json.at(tag).is_object()) {
        SC_LOG_ERROR(LABEL, "json: %{public}s tag invalid.", tag.c_str());
        return false;
    }
    auto jsonParent = json.at(tag);
    if (!ParseBool(jsonParent, JsonTagConstants::JSON_PARENT_EFFECT_TAG, parentEffect_)) {
        return false;
    }
    if (!ParseBool(jsonParent, JsonTagConstants::JSON_IS_CLIPPED_TAG, isClipped_)) {
        return false;
    }
    if (!ParseDimension(jsonParent, JsonTagConstants::JSON_TOP_CLIP_TAG, topClip_)) {
        return false;
    }
    if (!ParseDimension(jsonParent, JsonTagConstants::JSON_BOTTOM_CLIP_TAG, bottomClip_)) {
        return false;
    }
    if (!ParseDimension(jsonParent, JsonTagConstants::JSON_LEFT_CLIP_TAG, leftClip_)) {
        return false;
    }
    if (!ParseDimension(jsonParent, JsonTagConstants::JSON_RIGHT_CLIP_TAG, rightClip_)) {
        return false;
    }
    if (!ParseString(jsonParent, JsonTagConstants::JSON_PARENT_TAG_TAG, parentTag_)) {
        return false;
    }
    return true;
}

bool SecCompBase::ParseRect(const nlohmann::json& json, const std::string& tag, SecCompRect& rect)
{
    if ((json.find(tag) == json.end()) || !json.at(tag).is_object()) {
        SC_LOG_ERROR(LABEL, "json: %{public}s tag invalid.", tag.c_str());
        return false;
    }

    auto jsonSize = json.at(tag);
    if (!ParseDimension(jsonSize, JsonTagConstants::JSON_RECT_X, rect.x_)) {
        return false;
    }

    if (!ParseDimension(jsonSize, JsonTagConstants::JSON_RECT_Y, rect.y_)) {
        return false;
    }

    if (!ParseDimension(jsonSize, JsonTagConstants::JSON_RECT_WIDTH, rect.width_)) {
        return false;
    }

    if (!ParseDimension(jsonSize, JsonTagConstants::JSON_RECT_HEIGHT, rect.height_)) {
        return false;
    }

    return true;
}

bool SecCompBase::ParseType(const nlohmann::json& json, const std::string& tag)
{
    if ((json.find(tag) == json.end()) || !json.at(tag).is_number()) {
        SC_LOG_ERROR(LABEL, "json: %{public}s tag invalid.", tag.c_str());
        return false;
    }
    int32_t value = json.at(tag).get<int32_t>();
    if ((value <= static_cast<int32_t>(SecCompType::UNKNOWN_SC_TYPE)) ||
        (value >= static_cast<int32_t>(SecCompType::MAX_SC_TYPE))) {
        SC_LOG_ERROR(LABEL, "scType value is invalid.");
        return false;
    }
    type_ = static_cast<SecCompType>(value);

    return true;
}

bool SecCompBase::ParseValue(const nlohmann::json& json, const std::string& tag, int32_t& value)
{
    if ((json.find(tag) == json.end()) || !json.at(tag).is_number()) {
        SC_LOG_ERROR(LABEL, "json: %{public}s tag invalid.", tag.c_str());
        return false;
    }
    value = json.at(tag).get<int32_t>();

    return true;
}

bool SecCompBase::ParseDisplayId(const nlohmann::json& json, const std::string& tag)
{
    if ((json.find(tag) == json.end()) || !json.at(tag).is_number()) {
        SC_LOG_ERROR(LABEL, "json: %{public}s tag invalid.", tag.c_str());
        return false;
    }
    displayId_ = json.at(tag).get<uint64_t>();

    return true;
}

bool SecCompBase::ParseCrossAxisState(const nlohmann::json& json, const std::string& tag)
{
    if ((json.find(tag) == json.end()) || !json.at(tag).is_number()) {
        SC_LOG_ERROR(LABEL, "json: %{public}s tag invalid.", tag.c_str());
        return false;
    }
    int32_t value = json.at(tag).get<int32_t>();
    if ((value < static_cast<int32_t>(CrossAxisState::STATE_INVALID)) ||
        (value > static_cast<int32_t>(CrossAxisState::STATE_NO_CROSS))) {
        SC_LOG_ERROR(LABEL, "Cross axis state: %{public}d is invalid.", value);
        return false;
    }
    crossAxisState_ = static_cast<CrossAxisState>(value);

    return true;
}

bool SecCompBase::FromJson(const nlohmann::json& jsonSrc)
{
    SC_LOG_DEBUG(LABEL, "Button info %{public}s.", jsonSrc.dump().c_str());
    if (!ParseType(jsonSrc, JsonTagConstants::JSON_SC_TYPE)) {
        return false;
    }
    if (!ParseValue(jsonSrc, JsonTagConstants::JSON_NODE_ID, nodeId_)) {
        return false;
    }
    if (!ParseRect(jsonSrc, JsonTagConstants::JSON_RECT, rect_)) {
        return false;
    }
    if (!ParseRect(jsonSrc, JsonTagConstants::JSON_WINDOW_RECT, windowRect_)) {
        return false;
    }
    if (!ParseSize(jsonSrc, JsonTagConstants::JSON_SIZE_TAG)) {
        return false;
    }
    if (!ParseColors(jsonSrc, JsonTagConstants::JSON_COLORS_TAG)) {
        return false;
    }
    if (!ParseBorders(jsonSrc, JsonTagConstants::JSON_BORDER_TAG)) {
        return false;
    }
    if (!ParseParent(jsonSrc, JsonTagConstants::JSON_PARENT_TAG)) {
        return false;
    }
    if (!ParseStyle(jsonSrc, JsonTagConstants::JSON_STYLE_TAG)) {
        return false;
    }
    if (!ParseValue(jsonSrc, JsonTagConstants::JSON_WINDOW_ID, windowId_)) {
        return false;
    }
    if (!ParseDisplayId(jsonSrc, JsonTagConstants::JSON_DISPLAY_ID)) {
        return false;
    }
    if (!ParseCrossAxisState(jsonSrc, JsonTagConstants::JSON_CROSS_AXIS_STATE)) {
        return false;
    }

    return true;
}

void SecCompBase::ToJson(nlohmann::json& jsonRes) const
{
    jsonRes[JsonTagConstants::JSON_SC_TYPE] = type_;
    jsonRes[JsonTagConstants::JSON_NODE_ID] = nodeId_;
    jsonRes[JsonTagConstants::JSON_RECT] = nlohmann::json {
        {JsonTagConstants::JSON_RECT_X, rect_.x_},
        {JsonTagConstants::JSON_RECT_Y, rect_.y_},
        {JsonTagConstants::JSON_RECT_WIDTH, rect_.width_},
        {JsonTagConstants::JSON_RECT_HEIGHT, rect_.height_}
    };
    jsonRes[JsonTagConstants::JSON_WINDOW_RECT] = nlohmann::json {
        {JsonTagConstants::JSON_RECT_X, windowRect_.x_},
        {JsonTagConstants::JSON_RECT_Y, windowRect_.y_},
        {JsonTagConstants::JSON_RECT_WIDTH, windowRect_.width_},
        {JsonTagConstants::JSON_RECT_HEIGHT, windowRect_.height_}
    };
    nlohmann::json jsonPadding = nlohmann::json {
        { JsonTagConstants::JSON_PADDING_TOP_TAG, padding_.top },
        { JsonTagConstants::JSON_PADDING_RIGHT_TAG, padding_.right },
        { JsonTagConstants::JSON_PADDING_BOTTOM_TAG, padding_.bottom },
        { JsonTagConstants::JSON_PADDING_LEFT_TAG, padding_.left },
    };

    jsonRes[JsonTagConstants::JSON_SIZE_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_FONT_SIZE_TAG, fontSize_ },
        { JsonTagConstants::JSON_ICON_SIZE_TAG, iconSize_ },
        { JsonTagConstants::JSON_TEXT_ICON_PADDING_TAG, textIconSpace_ },
        { JsonTagConstants::JSON_PADDING_SIZE_TAG, jsonPadding },
    };

    jsonRes[JsonTagConstants::JSON_COLORS_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_FONT_COLOR_TAG, fontColor_.value },
        { JsonTagConstants::JSON_ICON_COLOR_TAG, iconColor_.value },
        { JsonTagConstants::JSON_BG_COLOR_TAG, bgColor_.value }
    };

    jsonRes[JsonTagConstants::JSON_BORDER_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_BORDER_WIDTH_TAG, borderWidth_ },
    };
    jsonRes[JsonTagConstants::JSON_PARENT_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_PARENT_EFFECT_TAG, parentEffect_ },
        { JsonTagConstants::JSON_IS_CLIPPED_TAG, isClipped_ },
        { JsonTagConstants::JSON_TOP_CLIP_TAG, topClip_ },
        { JsonTagConstants::JSON_BOTTOM_CLIP_TAG, bottomClip_ },
        { JsonTagConstants::JSON_LEFT_CLIP_TAG, leftClip_ },
        { JsonTagConstants::JSON_RIGHT_CLIP_TAG, rightClip_ },
        { JsonTagConstants::JSON_PARENT_TAG_TAG, parentTag_ },
    };

    jsonRes[JsonTagConstants::JSON_STYLE_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_TEXT_TAG, text_ },
        { JsonTagConstants::JSON_ICON_TAG, icon_ },
        { JsonTagConstants::JSON_BG_TAG, bg_ },
    };
    jsonRes[JsonTagConstants::JSON_WINDOW_ID] = windowId_;
    jsonRes[JsonTagConstants::JSON_DISPLAY_ID] = displayId_;
    jsonRes[JsonTagConstants::JSON_CROSS_AXIS_STATE] = crossAxisState_;
}

std::string SecCompBase::ToJsonStr() const
{
    nlohmann::json json;
    ToJson(json);
    return json.dump();
}

bool SecCompBase::CompareComponentBasicInfo(SecCompBase *other, bool isRectCheck) const
{
    if (other == nullptr) {
        SC_LOG_ERROR(LABEL, "other is nullptr.");
        return false;
    }

    SecCompRect rect = other->rect_;
    SecCompRect windowRect = other->windowRect_;
    if (isRectCheck) {
        rect = rect_;
        windowRect = windowRect_;
    }

    auto leftValue = std::tie(type_, fontSize_, iconSize_, textIconSpace_, padding_.top, padding_.right,
        padding_.bottom, padding_.left, fontColor_.value, bgColor_.value, iconColor_.value, borderWidth_,
        rect, windowRect);
    auto rightValue = std::tie(other->type_, other->fontSize_, other->iconSize_, other->textIconSpace_,
        other->padding_.top, other->padding_.right, other->padding_.bottom, other->padding_.left,
        other->fontColor_.value, other->bgColor_.value, other->iconColor_.value, other->borderWidth_,
        other->rect_, other->windowRect_);

    return (leftValue == rightValue);
}

bool SecCompBase::ParseStyle(const nlohmann::json& json, const std::string& tag)
{
    if ((json.find(tag) == json.end()) || !json.at(tag).is_object()) {
        SC_LOG_ERROR(LABEL, "json: %{public}s tag invalid.", tag.c_str());
        return false;
    }
    auto jsonStyle = json.at(tag);
    if (jsonStyle.find(JsonTagConstants::JSON_TEXT_TAG) == jsonStyle.end() ||
        !jsonStyle.at(JsonTagConstants::JSON_TEXT_TAG).is_number()) {
        SC_LOG_ERROR(LABEL, "Json=%{public}s tag is invalid.", tag.c_str());
        return false;
    }
    if (jsonStyle.find(JsonTagConstants::JSON_ICON_TAG) == jsonStyle.end() ||
        !jsonStyle.at(JsonTagConstants::JSON_ICON_TAG).is_number()) {
        SC_LOG_ERROR(LABEL, "Json=%{public}s tag is invalid.", tag.c_str());
        return false;
    }
    if (jsonStyle.find(JsonTagConstants::JSON_BG_TAG) == jsonStyle.end() ||
        !jsonStyle.at(JsonTagConstants::JSON_BG_TAG).is_number()) {
        SC_LOG_ERROR(LABEL, "Json=%{public}s tag is invalid.", tag.c_str());
        return false;
    }
    text_ = jsonStyle.at(JsonTagConstants::JSON_TEXT_TAG).get<int32_t>();
    icon_ = jsonStyle.at(JsonTagConstants::JSON_ICON_TAG).get<int32_t>();
    if (!IsTextIconTypeValid()) {
        SC_LOG_ERROR(LABEL, "text or icon is invalid.");
        return false;
    }
    bg_ = static_cast<SecCompBackground>(jsonStyle.at(JsonTagConstants::JSON_BG_TAG).get<int32_t>());
    if ((bg_ <= SecCompBackground::UNKNOWN_BG) || (bg_ >= SecCompBackground::MAX_BG_TYPE)) {
        SC_LOG_ERROR(LABEL, "bg is invalid.");
        return false;
    }
    return true;
}
}  // namespace base
}  // namespace Security
}  // namespace OHOS
