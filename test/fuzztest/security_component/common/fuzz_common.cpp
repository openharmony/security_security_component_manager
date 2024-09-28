/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "fuzz_common.h"

#include <algorithm>
#include <cmath>
#include <display.h>
#include <display_info.h>
#include "display_manager.h"
#include "location_button.h"
#include "paste_button.h"
#include "save_button.h"
#include "sec_comp_base.h"
#include "sec_comp_info.h"
#include "securec.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
namespace {
static const int32_t DEFAULT_BUTTON_SIZE = 20;
static const int32_t DEFAULT_SCREEN_SIZE = 128;
static const int32_t BUTTON_FLOAT_SIZE = 10;
};

uint32_t CompoRandomGenerator::GetScType()
{
    // generate a number in range
    return GetData<uint32_t>() % (SecCompType::MAX_SC_TYPE - 1) + 1;
}

std::string CompoRandomGenerator::GenerateRandomCompoStr(uint32_t type)
{
    switch (type) {
        case LOCATION_COMPONENT:
            return ConstructLocationJson();
        case PASTE_COMPONENT:
            return ConstructPasteJson();
        case SAVE_COMPONENT:
            return ConstructSaveJson();
        default:
            return std::string();
    }
    return std::string();
}

// To generate a json, some objs are in default, otherwise these may not pass value check.
std::string CompoRandomGenerator::ConstructLocationJson()
{
    nlohmann::json jsonComponent;
    jsonComponent[JsonTagConstants::JSON_SC_TYPE] = LOCATION_COMPONENT;
    jsonComponent[JsonTagConstants::JSON_NODE_ID] = 0;
    SecCompRect window;
    PaddingSize padding;
    SecCompRect buttonRect;
    ConstructWindowJson(jsonComponent, window, padding, buttonRect);

    jsonComponent[JsonTagConstants::JSON_COLORS_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_FONT_COLOR_TAG, GetData<uint32_t>() },
        { JsonTagConstants::JSON_ICON_COLOR_TAG, GetData<uint32_t>() },
        { JsonTagConstants::JSON_BG_COLOR_TAG, GetData<uint32_t>() }
    };

    jsonComponent[JsonTagConstants::JSON_BORDER_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_BORDER_WIDTH_TAG, GetData<float>() },
    };
    // set default val
    jsonComponent[JsonTagConstants::JSON_PARENT_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_PARENT_EFFECT_TAG, GetData<bool>() },
        { JsonTagConstants::JSON_IS_CLIPPED_TAG, GetData<bool>() },
        { JsonTagConstants::JSON_TOP_CLIP_TAG, GetData<double>() },
        { JsonTagConstants::JSON_BOTTOM_CLIP_TAG, GetData<double>() },
        { JsonTagConstants::JSON_LEFT_CLIP_TAG, GetData<double>() },
        { JsonTagConstants::JSON_RIGHT_CLIP_TAG, GetData<double>() },
        { JsonTagConstants::JSON_PARENT_TAG_TAG, "" },
    };
    jsonComponent[JsonTagConstants::JSON_STYLE_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_TEXT_TAG,
            GetData<int32_t>() % static_cast<int32_t>(LocationDesc::MAX_LABEL_TYPE) },
        { JsonTagConstants::JSON_ICON_TAG,
            GetData<int32_t>() % static_cast<int32_t>(LocationIcon::MAX_ICON_TYPE) },
        { JsonTagConstants::JSON_BG_TAG,
            GetData<int32_t>() % static_cast<int32_t>(SecCompBackground::MAX_BG_TYPE) },
    };
    jsonComponent[JsonTagConstants::JSON_WINDOW_ID] = 0;
    compoJson_ = jsonComponent;
    return compoJson_.dump();
}

std::string CompoRandomGenerator::ConstructSaveJson()
{
    nlohmann::json jsonComponent;
    jsonComponent[JsonTagConstants::JSON_SC_TYPE] = SAVE_COMPONENT;
    jsonComponent[JsonTagConstants::JSON_NODE_ID] = 0;
    SecCompRect window;
    PaddingSize padding;
    SecCompRect buttonRect;
    ConstructWindowJson(jsonComponent, window, padding, buttonRect);

    jsonComponent[JsonTagConstants::JSON_COLORS_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_FONT_COLOR_TAG, GetData<uint32_t>() },
        { JsonTagConstants::JSON_ICON_COLOR_TAG, GetData<uint32_t>() },
        { JsonTagConstants::JSON_BG_COLOR_TAG, GetData<uint32_t>() }
    };

    jsonComponent[JsonTagConstants::JSON_BORDER_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_BORDER_WIDTH_TAG, GetData<float>() },
    };
    // set default val
    jsonComponent[JsonTagConstants::JSON_PARENT_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_PARENT_EFFECT_TAG, GetData<bool>() },
        { JsonTagConstants::JSON_IS_CLIPPED_TAG, GetData<bool>() },
        { JsonTagConstants::JSON_TOP_CLIP_TAG, GetData<double>() },
        { JsonTagConstants::JSON_BOTTOM_CLIP_TAG, GetData<double>() },
        { JsonTagConstants::JSON_LEFT_CLIP_TAG, GetData<double>() },
        { JsonTagConstants::JSON_RIGHT_CLIP_TAG, GetData<double>() },
        { JsonTagConstants::JSON_PARENT_TAG_TAG, "" },
    };
    jsonComponent[JsonTagConstants::JSON_STYLE_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_TEXT_TAG,
            GetData<int32_t>() % static_cast<int32_t>(SaveDesc::MAX_LABEL_TYPE) },
        { JsonTagConstants::JSON_ICON_TAG,
            GetData<int32_t>() % static_cast<int32_t>(SaveIcon::MAX_ICON_TYPE) },
        { JsonTagConstants::JSON_BG_TAG,
            GetData<int32_t>() % static_cast<int32_t>(SecCompBackground::MAX_BG_TYPE) },
    };
    jsonComponent[JsonTagConstants::JSON_WINDOW_ID] = 0;
    compoJson_ = jsonComponent;
    return compoJson_.dump();
}

std::string CompoRandomGenerator::ConstructPasteJson()
{
    nlohmann::json jsonComponent;
    jsonComponent[JsonTagConstants::JSON_SC_TYPE] = PASTE_COMPONENT;
    jsonComponent[JsonTagConstants::JSON_NODE_ID] = 0;
    SecCompRect window;
    PaddingSize padding;
    SecCompRect buttonRect;
    ConstructWindowJson(jsonComponent, window, padding, buttonRect);

    jsonComponent[JsonTagConstants::JSON_COLORS_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_FONT_COLOR_TAG, GetData<uint32_t>() },
        { JsonTagConstants::JSON_ICON_COLOR_TAG, GetData<uint32_t>() },
        { JsonTagConstants::JSON_BG_COLOR_TAG, GetData<uint32_t>() }
    };

    jsonComponent[JsonTagConstants::JSON_BORDER_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_BORDER_WIDTH_TAG, GetData<float>() },
    };
    // set default val
    jsonComponent[JsonTagConstants::JSON_PARENT_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_PARENT_EFFECT_TAG, GetData<bool>() },
        { JsonTagConstants::JSON_IS_CLIPPED_TAG, GetData<bool>() },
        { JsonTagConstants::JSON_TOP_CLIP_TAG, GetData<double>() },
        { JsonTagConstants::JSON_BOTTOM_CLIP_TAG, GetData<double>() },
        { JsonTagConstants::JSON_LEFT_CLIP_TAG, GetData<double>() },
        { JsonTagConstants::JSON_RIGHT_CLIP_TAG, GetData<double>() },
        { JsonTagConstants::JSON_PARENT_TAG_TAG, "" },
    };
    jsonComponent[JsonTagConstants::JSON_STYLE_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_TEXT_TAG,
            GetData<int32_t>() % static_cast<int32_t>(PasteDesc::MAX_LABEL_TYPE) },
        { JsonTagConstants::JSON_ICON_TAG,
            GetData<int32_t>() % static_cast<int32_t>(PasteIcon::MAX_ICON_TYPE) },
        { JsonTagConstants::JSON_BG_TAG,
            GetData<int32_t>() % static_cast<int32_t>(SecCompBackground::MAX_BG_TYPE) },
    };
    jsonComponent[JsonTagConstants::JSON_WINDOW_ID] = 0;
    compoJson_ = jsonComponent;
    return compoJson_.dump();
}

void CompoRandomGenerator::ConstructButtonRect(
    SecCompRect &window, PaddingSize &padding, SecCompRect &buttonRect)
{
    window.x_ = (DEFAULT_SCREEN_SIZE >> 1) - BUTTON_FLOAT_SIZE;
    window.y_ = (DEFAULT_SCREEN_SIZE >> 1) - BUTTON_FLOAT_SIZE;
    window.width_ = (DEFAULT_SCREEN_SIZE >> 1) + BUTTON_FLOAT_SIZE;
    window.height_ = (DEFAULT_SCREEN_SIZE >> 1) + BUTTON_FLOAT_SIZE;
    
    buttonRect.x_ = std::fmod(std::fabs(GetData<double>()), window.width_) + window.x_;
    buttonRect.y_ = std::fmod(std::fabs(GetData<double>()), window.height_) + window.y_;
    buttonRect.width_ = DEFAULT_BUTTON_SIZE;
    buttonRect.height_ = DEFAULT_BUTTON_SIZE;

    padding.bottom = window.y_ + window.height_ - buttonRect.y_ - buttonRect.height_;
    padding.top = window.y_ - buttonRect.y_;
    padding.left = buttonRect.x_ - window.x_;
    padding.right = window.x_ + window.width_ - buttonRect.x_ - buttonRect.width_;
}

void CompoRandomGenerator::ConstructWindowJson(
    nlohmann::json &jsonComponent, SecCompRect &window, PaddingSize &padding, SecCompRect &buttonRect)
{
    ConstructButtonRect(window, padding, buttonRect);
    jsonComponent[JsonTagConstants::JSON_RECT] =  nlohmann::json {
        {JsonTagConstants::JSON_RECT_X, buttonRect.x_ },
        {JsonTagConstants::JSON_RECT_Y, buttonRect.y_ },
        {JsonTagConstants::JSON_RECT_WIDTH, buttonRect.width_ },
        {JsonTagConstants::JSON_RECT_HEIGHT, buttonRect.height_ }
    };
    jsonComponent[JsonTagConstants::JSON_WINDOW_RECT] = nlohmann::json {
        { JsonTagConstants::JSON_RECT_X, window.x_ },
        { JsonTagConstants::JSON_RECT_Y, window.y_ },
        { JsonTagConstants::JSON_RECT_WIDTH, window.width_ },
        { JsonTagConstants::JSON_RECT_HEIGHT, window.height_ }
    };
    nlohmann::json jsonPadding = nlohmann::json {
        { JsonTagConstants::JSON_PADDING_TOP_TAG, padding.top },
        { JsonTagConstants::JSON_PADDING_RIGHT_TAG, padding.right },
        { JsonTagConstants::JSON_PADDING_BOTTOM_TAG, padding.bottom },
        { JsonTagConstants::JSON_PADDING_LEFT_TAG, padding.left },
    };

    jsonComponent[JsonTagConstants::JSON_SIZE_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_FONT_SIZE_TAG, std::fabs(GetData<float>()) },
        { JsonTagConstants::JSON_ICON_SIZE_TAG, std::fabs(GetData<float>()) },
        { JsonTagConstants::JSON_TEXT_ICON_PADDING_TAG, std::fabs(GetData<float>()) },
        { JsonTagConstants::JSON_PADDING_SIZE_TAG, jsonPadding },
    };
}
}
}
}