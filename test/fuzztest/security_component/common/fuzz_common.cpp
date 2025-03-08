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
bool GetScreenSize(double& width, double& height)
{
    sptr<OHOS::Rosen::Display> display =
        OHOS::Rosen::DisplayManager::GetInstance().GetDefaultDisplaySync();
    if (display == nullptr) {
        return false;
    }

    auto info = display->GetDisplayInfo();
    if (info == nullptr) {
        return false;
    }

    width = static_cast<double>(info->GetWidth());
    height = static_cast<double>(info->GetHeight());
    return true;
}
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
    jsonComponent[JsonTagConstants::JSON_DISPLAY_ID] = 0;
    jsonComponent[JsonTagConstants::JSON_CROSS_AXIS_STATE] = 0;
    return jsonComponent.dump();
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
    jsonComponent[JsonTagConstants::JSON_DISPLAY_ID] = 0;
    jsonComponent[JsonTagConstants::JSON_CROSS_AXIS_STATE] = 0;
    return jsonComponent.dump();
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
    jsonComponent[JsonTagConstants::JSON_DISPLAY_ID] = 0;
    jsonComponent[JsonTagConstants::JSON_CROSS_AXIS_STATE] = 0;
    return jsonComponent.dump();
}

void CompoRandomGenerator::ConstructRandomRect(SecCompRect &rect)
{
    // a window rect should val >= 0
    double width;
    double height;
    GetScreenSize(width, height);
    rect.x_ = std::fmod(std::fabs(GetData<double>()), width);
    rect.y_ = std::fmod(std::fabs(GetData<double>()), height);
    rect.width_ = std::fmod(std::fabs(GetData<double>()), width);
    rect.height_ = std::fmod(std::fabs(GetData<double>()), height);
}

void CompoRandomGenerator::ConstructButtonRect(
    SecCompRect &window, PaddingSize &padding, SecCompRect &buttonRect)
{
    ConstructRandomRect(window);
    double tmp1 = std::fmod(std::fabs(GetData<double>()), static_cast<double>(window.height_));
    double tmp2 = std::fmod(std::fabs(GetData<double>()), static_cast<double>(window.height_));
    double top = std::min(tmp1, tmp2);
    double bottom = std::max(tmp1, tmp2);
    buttonRect.y_ = window.y_ + top;
    buttonRect.height_ = bottom - top;
    padding.top = top;
    padding.bottom = window.height_ - bottom;

    double width;
    double height;
    double sizeLimitRate = 0.09;
    GetScreenSize(width, height);
    double sizeLimit = width * height * sizeLimitRate;
    double buttonMaxWidth = sizeLimit / buttonRect.height_;

    tmp1 = std::fmod(std::fabs(GetData<double>()), static_cast<double>(window.width_));
    tmp2 = std::fmod(std::fabs(GetData<double>()), static_cast<double>(window.width_));
    double left = std::min(tmp1, tmp2);
    double right = std::max(tmp1, tmp2);
    buttonRect.x_ = window.x_ + left;
    buttonRect.width_ = std::min({right - left, buttonMaxWidth, window.width_ - buttonRect.x_ - 1});
    padding.left = left;
    padding.right = window.width_ - buttonRect.width_ - left;
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
