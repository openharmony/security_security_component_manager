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
#include "test_common.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
void TestCommon::BuildLocationComponentInfo(nlohmann::json& jsonComponent)
{
    jsonComponent[JsonTagConstants::JSON_SC_TYPE] = LOCATION_COMPONENT;
    jsonComponent[JsonTagConstants::JSON_RECT] = nlohmann::json {
        {JsonTagConstants::JSON_RECT_X, TestCommon::TEST_COORDINATE },
        {JsonTagConstants::JSON_RECT_Y, TestCommon::TEST_COORDINATE },
        {JsonTagConstants::JSON_RECT_WIDTH, TestCommon::TEST_COORDINATE },
        {JsonTagConstants::JSON_RECT_HEIGHT, TestCommon::TEST_COORDINATE }
    };
    jsonComponent[JsonTagConstants::JSON_NODE_ID] = 0;
    jsonComponent[JsonTagConstants::JSON_WINDOW_RECT] = nlohmann::json {
        {JsonTagConstants::JSON_RECT_X, TestCommon::TEST_COORDINATE },
        {JsonTagConstants::JSON_RECT_Y, TestCommon::TEST_COORDINATE },
        {JsonTagConstants::JSON_RECT_WIDTH, TestCommon::TEST_COORDINATE },
        {JsonTagConstants::JSON_RECT_HEIGHT, TestCommon::TEST_COORDINATE }
    };
    nlohmann::json jsonPadding = nlohmann::json {
        { JsonTagConstants::JSON_PADDING_TOP_TAG, TestCommon::TEST_DIMENSION },
        { JsonTagConstants::JSON_PADDING_RIGHT_TAG, TestCommon::TEST_DIMENSION },
        { JsonTagConstants::JSON_PADDING_BOTTOM_TAG, TestCommon::TEST_DIMENSION },
        { JsonTagConstants::JSON_PADDING_LEFT_TAG, TestCommon::TEST_DIMENSION },
    };

    jsonComponent[JsonTagConstants::JSON_SIZE_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_FONT_SIZE_TAG, TestCommon::TEST_SIZE },
        { JsonTagConstants::JSON_ICON_SIZE_TAG, TestCommon::TEST_SIZE },
        { JsonTagConstants::JSON_TEXT_ICON_PADDING_TAG, TestCommon::TEST_SIZE },
        { JsonTagConstants::JSON_PADDING_SIZE_TAG, jsonPadding },
    };

    jsonComponent[JsonTagConstants::JSON_COLORS_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_FONT_COLOR_TAG, TestCommon::TEST_COLOR_RED },
        { JsonTagConstants::JSON_ICON_COLOR_TAG, TestCommon::TEST_COLOR_BLUE },
        { JsonTagConstants::JSON_BG_COLOR_TAG, TestCommon::TEST_COLOR_YELLOW }
    };

    jsonComponent[JsonTagConstants::JSON_BORDER_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_BORDER_WIDTH_TAG, TestCommon::TEST_SIZE },
    };
    jsonComponent[JsonTagConstants::JSON_PARENT_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_PARENT_EFFECT_TAG, false },
    };
    jsonComponent[JsonTagConstants::JSON_STYLE_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_TEXT_TAG, LocationDesc::SELECT_LOCATION },
        { JsonTagConstants::JSON_ICON_TAG, LocationIcon::LINE_ICON },
        { JsonTagConstants::JSON_BG_TAG, SecCompBackground::CIRCLE },
    };
    jsonComponent[JsonTagConstants::JSON_WINDOW_ID] = 0;
}

void TestCommon::BuildSaveComponentInfo(nlohmann::json& jsonComponent)
{
    jsonComponent[JsonTagConstants::JSON_SC_TYPE] = SAVE_COMPONENT;
    jsonComponent[JsonTagConstants::JSON_NODE_ID] = 0;
    jsonComponent[JsonTagConstants::JSON_RECT] = nlohmann::json {
        {JsonTagConstants::JSON_RECT_X, TestCommon::TEST_COORDINATE },
        {JsonTagConstants::JSON_RECT_Y, TestCommon::TEST_COORDINATE },
        {JsonTagConstants::JSON_RECT_WIDTH, TestCommon::TEST_COORDINATE },
        {JsonTagConstants::JSON_RECT_HEIGHT, TestCommon::TEST_COORDINATE }
    };
    jsonComponent[JsonTagConstants::JSON_WINDOW_RECT] = nlohmann::json {
        {JsonTagConstants::JSON_RECT_X, TestCommon::TEST_COORDINATE },
        {JsonTagConstants::JSON_RECT_Y, TestCommon::TEST_COORDINATE },
        {JsonTagConstants::JSON_RECT_WIDTH, TestCommon::TEST_COORDINATE },
        {JsonTagConstants::JSON_RECT_HEIGHT, TestCommon::TEST_COORDINATE }
    };
    nlohmann::json jsonPadding = nlohmann::json {
        { JsonTagConstants::JSON_PADDING_TOP_TAG, TestCommon::TEST_DIMENSION },
        { JsonTagConstants::JSON_PADDING_RIGHT_TAG, TestCommon::TEST_DIMENSION },
        { JsonTagConstants::JSON_PADDING_BOTTOM_TAG, TestCommon::TEST_DIMENSION },
        { JsonTagConstants::JSON_PADDING_LEFT_TAG, TestCommon::TEST_DIMENSION },
    };

    jsonComponent[JsonTagConstants::JSON_SIZE_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_FONT_SIZE_TAG, TestCommon::TEST_SIZE },
        { JsonTagConstants::JSON_ICON_SIZE_TAG, TestCommon::TEST_SIZE },
        { JsonTagConstants::JSON_TEXT_ICON_PADDING_TAG, TestCommon::TEST_SIZE },
        { JsonTagConstants::JSON_PADDING_SIZE_TAG, jsonPadding },
    };

    jsonComponent[JsonTagConstants::JSON_COLORS_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_FONT_COLOR_TAG, TestCommon::TEST_COLOR_RED },
        { JsonTagConstants::JSON_ICON_COLOR_TAG, TestCommon::TEST_COLOR_BLUE },
        { JsonTagConstants::JSON_BG_COLOR_TAG, TestCommon::TEST_COLOR_YELLOW }
    };

    jsonComponent[JsonTagConstants::JSON_BORDER_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_BORDER_WIDTH_TAG, TestCommon::TEST_SIZE },
    };
    jsonComponent[JsonTagConstants::JSON_PARENT_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_PARENT_EFFECT_TAG, false },
    };
    jsonComponent[JsonTagConstants::JSON_STYLE_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_TEXT_TAG, SaveDesc::DOWNLOAD },
        { JsonTagConstants::JSON_ICON_TAG, SaveIcon::LINE_ICON },
        { JsonTagConstants::JSON_BG_TAG, SecCompBackground::CIRCLE },
    };
    jsonComponent[JsonTagConstants::JSON_WINDOW_ID] = 0;
}

void TestCommon::BuildPasteComponentInfo(nlohmann::json& jsonComponent)
{
    jsonComponent[JsonTagConstants::JSON_SC_TYPE] = PASTE_COMPONENT;
    jsonComponent[JsonTagConstants::JSON_NODE_ID] = 0;
    jsonComponent[JsonTagConstants::JSON_RECT] = nlohmann::json {
        {JsonTagConstants::JSON_RECT_X, TestCommon::TEST_COORDINATE },
        {JsonTagConstants::JSON_RECT_Y, TestCommon::TEST_COORDINATE },
        {JsonTagConstants::JSON_RECT_WIDTH, TestCommon::TEST_COORDINATE },
        {JsonTagConstants::JSON_RECT_HEIGHT, TestCommon::TEST_COORDINATE }
    };
    jsonComponent[JsonTagConstants::JSON_WINDOW_RECT] = nlohmann::json {
        {JsonTagConstants::JSON_RECT_X, TestCommon::TEST_COORDINATE },
        {JsonTagConstants::JSON_RECT_Y, TestCommon::TEST_COORDINATE },
        {JsonTagConstants::JSON_RECT_WIDTH, TestCommon::TEST_COORDINATE },
        {JsonTagConstants::JSON_RECT_HEIGHT, TestCommon::TEST_COORDINATE }
    };
    nlohmann::json jsonPadding = nlohmann::json {
        { JsonTagConstants::JSON_PADDING_TOP_TAG, TestCommon::TEST_DIMENSION },
        { JsonTagConstants::JSON_PADDING_RIGHT_TAG, TestCommon::TEST_DIMENSION },
        { JsonTagConstants::JSON_PADDING_BOTTOM_TAG, TestCommon::TEST_DIMENSION },
        { JsonTagConstants::JSON_PADDING_LEFT_TAG, TestCommon::TEST_DIMENSION },
    };

    jsonComponent[JsonTagConstants::JSON_SIZE_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_FONT_SIZE_TAG, TestCommon::TEST_SIZE },
        { JsonTagConstants::JSON_ICON_SIZE_TAG, TestCommon::TEST_SIZE },
        { JsonTagConstants::JSON_TEXT_ICON_PADDING_TAG, TestCommon::TEST_SIZE },
        { JsonTagConstants::JSON_PADDING_SIZE_TAG, jsonPadding },
    };

    jsonComponent[JsonTagConstants::JSON_COLORS_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_FONT_COLOR_TAG, TestCommon::TEST_COLOR_RED },
        { JsonTagConstants::JSON_ICON_COLOR_TAG, TestCommon::TEST_COLOR_BLUE },
        { JsonTagConstants::JSON_BG_COLOR_TAG, TestCommon::TEST_COLOR_YELLOW }
    };

    jsonComponent[JsonTagConstants::JSON_BORDER_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_BORDER_WIDTH_TAG, TestCommon::TEST_SIZE },
    };
    jsonComponent[JsonTagConstants::JSON_PARENT_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_PARENT_EFFECT_TAG, false },
    };
    jsonComponent[JsonTagConstants::JSON_STYLE_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_TEXT_TAG, PasteDesc::PASTE },
        { JsonTagConstants::JSON_ICON_TAG, PasteIcon::LINE_ICON },
        { JsonTagConstants::JSON_BG_TAG, SecCompBackground::CIRCLE },
    };
    jsonComponent[JsonTagConstants::JSON_WINDOW_ID] = 0;
}
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
