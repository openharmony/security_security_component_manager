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

#include "sec_comp_info_helper_test.h"

#include "display.h"
#include "display_info.h"
#include "display_manager.h"
#include "location_button.h"
#include "paste_button.h"
#include "save_button.h"
#include "sec_comp_log.h"
#include "sec_comp_err.h"
#include "service_test_common.h"
#include "window_manager.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Security::SecurityComponent;

namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
    LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "SecCompInfoHelperTest"};
static double g_curScreenWidth = 0.0F;
static double g_curScreenHeight = 0.0F;
static double g_testWidth = 0.0F;
static double g_testHeight = 0.0F;

static bool GetScreenSize()
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

    g_curScreenWidth = static_cast<double>(info->GetWidth());
    g_curScreenHeight = static_cast<double>(info->GetHeight());
    return true;
}
}

void SecCompInfoHelperTest::SetUpTestCase()
{
    ASSERT_TRUE(GetScreenSize());
    g_testWidth = (ServiceTestCommon::ZERO_OFFSET + g_curScreenWidth) / ServiceTestCommon::QUARTER;
    g_testHeight = (ServiceTestCommon::ZERO_OFFSET + g_curScreenHeight) / ServiceTestCommon::QUARTER;
}

void SecCompInfoHelperTest::TearDownTestCase()
{}

void SecCompInfoHelperTest::SetUp()
{
    SC_LOG_INFO(LABEL, "setup");
    Rosen::WindowManager::GetInstance().SetDefaultSecCompScene();
}

void SecCompInfoHelperTest::TearDown()
{}

/**
 * @tc.name: ParseComponent001
 * @tc.desc: Test parse component info success
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SecCompInfoHelperTest, ParseComponent001, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    ServiceTestCommon::BuildLocationComponentJson(jsonComponent);
    SecCompBase* comp = SecCompInfoHelper::ParseComponent(LOCATION_COMPONENT, jsonComponent);
    ASSERT_NE(nullptr, comp);
    ASSERT_TRUE(comp->GetValid());
}

/**
 * @tc.name: ParseComponent002
 * @tc.desc: Test parse component info with empty json
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SecCompInfoHelperTest, ParseComponent002, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    ASSERT_EQ(nullptr, SecCompInfoHelper::ParseComponent(UNKNOWN_SC_TYPE, jsonComponent));

    ASSERT_EQ(nullptr, SecCompInfoHelper::ParseComponent(LOCATION_COMPONENT, jsonComponent));
    ASSERT_EQ(nullptr, SecCompInfoHelper::ParseComponent(PASTE_COMPONENT, jsonComponent));
    ASSERT_EQ(nullptr, SecCompInfoHelper::ParseComponent(SAVE_COMPONENT, jsonComponent));
}

/**
 * @tc.name: ParseComponent003
 * @tc.desc: Test parse component info with invalid type
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SecCompInfoHelperTest, ParseComponent003, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    ServiceTestCommon::BuildLocationComponentJson(jsonComponent);

    SecCompBase* comp = SecCompInfoHelper::ParseComponent(LOCATION_COMPONENT, jsonComponent);
    ASSERT_TRUE(comp->GetValid());

    jsonComponent[JsonTagConstants::JSON_SC_TYPE] = UNKNOWN_SC_TYPE;
    comp = SecCompInfoHelper::ParseComponent(LOCATION_COMPONENT, jsonComponent);
    ASSERT_EQ(nullptr, comp);
}

static SecCompRect GetDefaultRect(void)
{
    SecCompRect rect = {
        .x_ = g_testWidth,
        .y_ = g_testHeight,
        .width_ = g_testWidth,
        .height_ = g_testHeight
    };
    return rect;
}

/**
 * @tc.name: ParseComponent004
 * @tc.desc: Test parse component info with invalid rect
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SecCompInfoHelperTest, ParseComponent004, TestSize.Level1)
{
    SecCompRect rect = GetDefaultRect();
    SecCompRect windowRect = GetDefaultRect();
    ASSERT_TRUE(SecCompInfoHelper::CheckRectValid(rect, windowRect));

    rect.x_ = ServiceTestCommon::TEST_INVALID_DIMENSION;
    ASSERT_FALSE(SecCompInfoHelper::CheckRectValid(rect, windowRect));
    rect.x_ = g_testWidth;

    rect.y_ = ServiceTestCommon::TEST_INVALID_DIMENSION;
    ASSERT_FALSE(SecCompInfoHelper::CheckRectValid(rect, windowRect));
    rect.y_ = g_testHeight;

    rect.x_ = g_curScreenWidth + 1;
    ASSERT_FALSE(SecCompInfoHelper::CheckRectValid(rect, windowRect));
    rect.x_ = g_testWidth;

    rect.y_ = g_curScreenHeight + 1;
    ASSERT_FALSE(SecCompInfoHelper::CheckRectValid(rect, windowRect));
    rect.y_ = g_testHeight;

    rect.width_ = g_curScreenWidth;
    rect.height_ = g_curScreenHeight;
    ASSERT_FALSE(SecCompInfoHelper::CheckRectValid(rect, windowRect));
    rect.width_ = g_testWidth;
    rect.height_ = g_testHeight;

    rect.x_ = g_curScreenWidth - g_testWidth;
    ASSERT_FALSE(SecCompInfoHelper::CheckRectValid(rect, windowRect));
    rect.x_ = g_testWidth;
    rect.y_ = g_curScreenHeight - g_testHeight;

    ASSERT_FALSE(SecCompInfoHelper::CheckRectValid(rect, windowRect));
    rect.y_ = g_testHeight;
}

/**
 * @tc.name: ParseComponent005
 * @tc.desc: Test parse component info with windowRect invalid
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SecCompInfoHelperTest, ParseComponent005, TestSize.Level1)
{
    SecCompRect rect = GetDefaultRect();
    SecCompRect windowRect = GetDefaultRect();
    ASSERT_TRUE(SecCompInfoHelper::CheckRectValid(rect, windowRect));

    windowRect.x_ = g_testWidth + 1;
    ASSERT_FALSE(SecCompInfoHelper::CheckRectValid(rect, windowRect));
    windowRect.x_ = g_testWidth;

    windowRect.y_ = g_testHeight + 1;
    ASSERT_FALSE(SecCompInfoHelper::CheckRectValid(rect, windowRect));
    windowRect.y_ = g_testHeight;

    windowRect.width_ = g_testWidth - 1;
    ASSERT_FALSE(SecCompInfoHelper::CheckRectValid(rect, windowRect));
    windowRect.width_ = g_testWidth;

    windowRect.height_ = g_testHeight - 1;
    ASSERT_FALSE(SecCompInfoHelper::CheckRectValid(rect, windowRect));
    windowRect.height_ = g_testHeight;

    windowRect.width_ = ServiceTestCommon::TEST_INVALID_DIMENSION;
    ASSERT_FALSE(SecCompInfoHelper::CheckRectValid(rect, windowRect));
    windowRect.width_ = g_testWidth;

    windowRect.height_ = ServiceTestCommon::TEST_INVALID_DIMENSION;
    ASSERT_FALSE(SecCompInfoHelper::CheckRectValid(rect, windowRect));
    windowRect.height_ = g_testHeight;
}

/**
 * @tc.name: ParseComponent006
 * @tc.desc: Test parse component info with parentEffect active
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SecCompInfoHelperTest, ParseComponent006, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    ServiceTestCommon::BuildLocationComponentJson(jsonComponent);

    SecCompBase* comp = SecCompInfoHelper::ParseComponent(LOCATION_COMPONENT, jsonComponent);
    ASSERT_TRUE(comp->GetValid());

    jsonComponent[JsonTagConstants::JSON_PARENT_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_PARENT_EFFECT_TAG, true },
    };
    comp = SecCompInfoHelper::ParseComponent(LOCATION_COMPONENT, jsonComponent);
    ASSERT_FALSE(comp->GetValid());
}

/**
 * @tc.name: ParseComponent007
 * @tc.desc: Test parse component info with invalid size
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SecCompInfoHelperTest, ParseComponent007, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    ServiceTestCommon::BuildLocationComponentJson(jsonComponent);

    SecCompBase* comp = SecCompInfoHelper::ParseComponent(LOCATION_COMPONENT, jsonComponent);
    ASSERT_TRUE(comp->GetValid());

    auto& sizeJson = jsonComponent[JsonTagConstants::JSON_SIZE_TAG];
    sizeJson[JsonTagConstants::JSON_FONT_SIZE_TAG] = ServiceTestCommon::TEST_INVALID_DIMENSION;
    comp = SecCompInfoHelper::ParseComponent(LOCATION_COMPONENT, jsonComponent);
    ASSERT_FALSE(comp->GetValid());

    sizeJson[JsonTagConstants::JSON_FONT_SIZE_TAG] = ServiceTestCommon::TEST_DIMENSION;
    sizeJson[JsonTagConstants::JSON_ICON_SIZE_TAG] = ServiceTestCommon::TEST_INVALID_DIMENSION;
    comp = SecCompInfoHelper::ParseComponent(LOCATION_COMPONENT, jsonComponent);
    ASSERT_FALSE(comp->GetValid());

    sizeJson[JsonTagConstants::JSON_ICON_SIZE_TAG] = ServiceTestCommon::TEST_DIMENSION;
    sizeJson[JsonTagConstants::JSON_TEXT_ICON_PADDING_TAG] = ServiceTestCommon::TEST_INVALID_DIMENSION;
    comp = SecCompInfoHelper::ParseComponent(LOCATION_COMPONENT, jsonComponent);
    ASSERT_FALSE(comp->GetValid());
}

/**
 * @tc.name: ParseComponent008
 * @tc.desc: Test parse component info with invalid size
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SecCompInfoHelperTest, ParseComponent008, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    ServiceTestCommon::BuildLocationComponentJson(jsonComponent);
    SecCompBase* comp = SecCompInfoHelper::ParseComponent(LOCATION_COMPONENT, jsonComponent);
    ASSERT_TRUE(comp->GetValid());

    auto& sizeJson = jsonComponent[JsonTagConstants::JSON_SIZE_TAG];
    auto& paddingJson = sizeJson[JsonTagConstants::JSON_PADDING_SIZE_TAG];
    paddingJson[JsonTagConstants::JSON_PADDING_TOP_TAG] = ServiceTestCommon::TEST_INVALID_DIMENSION;
    comp = SecCompInfoHelper::ParseComponent(LOCATION_COMPONENT, jsonComponent);
    ASSERT_FALSE(comp->GetValid());

    paddingJson[JsonTagConstants::JSON_PADDING_TOP_TAG] = ServiceTestCommon::TEST_DIMENSION;
    paddingJson[JsonTagConstants::JSON_PADDING_RIGHT_TAG] = ServiceTestCommon::TEST_INVALID_DIMENSION;
    comp = SecCompInfoHelper::ParseComponent(LOCATION_COMPONENT, jsonComponent);
    ASSERT_FALSE(comp->GetValid());
}

/**
 * @tc.name: ParseComponent009
 * @tc.desc: Test parse component info with invalid size
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SecCompInfoHelperTest, ParseComponent009, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    ServiceTestCommon::BuildLocationComponentJson(jsonComponent);
    SecCompBase* comp = SecCompInfoHelper::ParseComponent(LOCATION_COMPONENT, jsonComponent);
    ASSERT_TRUE(comp->GetValid());

    auto& sizesJson = jsonComponent[JsonTagConstants::JSON_SIZE_TAG];
    auto& paddingsJson = sizesJson[JsonTagConstants::JSON_PADDING_SIZE_TAG];
    paddingsJson[JsonTagConstants::JSON_PADDING_BOTTOM_TAG] = ServiceTestCommon::TEST_INVALID_DIMENSION;
    comp = SecCompInfoHelper::ParseComponent(LOCATION_COMPONENT, jsonComponent);
    ASSERT_FALSE(comp->GetValid());

    paddingsJson[JsonTagConstants::JSON_PADDING_BOTTOM_TAG] = ServiceTestCommon::TEST_DIMENSION;
    paddingsJson[JsonTagConstants::JSON_PADDING_LEFT_TAG] = ServiceTestCommon::TEST_INVALID_DIMENSION;
    comp = SecCompInfoHelper::ParseComponent(LOCATION_COMPONENT, jsonComponent);
    ASSERT_FALSE(comp->GetValid());
}

/**
 * @tc.name: ParseComponent010
 * @tc.desc: Test parse component info with invalid color
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SecCompInfoHelperTest, ParseComponent010, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    ServiceTestCommon::BuildLocationComponentJson(jsonComponent);
    SecCompBase* comp = SecCompInfoHelper::ParseComponent(LOCATION_COMPONENT, jsonComponent);
    ASSERT_TRUE(comp->GetValid());

    auto& colorJson = jsonComponent[JsonTagConstants::JSON_COLORS_TAG];
    colorJson[JsonTagConstants::JSON_FONT_COLOR_TAG] = ServiceTestCommon::TEST_COLOR_INVALID;
    comp = SecCompInfoHelper::ParseComponent(LOCATION_COMPONENT, jsonComponent);
    ASSERT_FALSE(comp->GetValid());

    colorJson[JsonTagConstants::JSON_FONT_COLOR_TAG] = ServiceTestCommon::TEST_COLOR_RED;
    colorJson[JsonTagConstants::JSON_ICON_COLOR_TAG] = ServiceTestCommon::TEST_COLOR_INVALID;
    comp = SecCompInfoHelper::ParseComponent(LOCATION_COMPONENT, jsonComponent);
    ASSERT_FALSE(comp->GetValid());
}

/**
 * @tc.name: ParseComponent011
 * @tc.desc: Test parse component info with invalid style
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SecCompInfoHelperTest, ParseComponent011, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    ServiceTestCommon::BuildLocationComponentJson(jsonComponent);
    SecCompBase* comp = SecCompInfoHelper::ParseComponent(LOCATION_COMPONENT, jsonComponent);
    ASSERT_TRUE(comp->GetValid());

    jsonComponent[JsonTagConstants::JSON_STYLE_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_TEXT_TAG, NO_TEXT },
        { JsonTagConstants::JSON_ICON_TAG, NO_ICON },
        { JsonTagConstants::JSON_BG_TAG, SecCompBackground::CIRCLE },
    };
    comp = SecCompInfoHelper::ParseComponent(LOCATION_COMPONENT, jsonComponent);
    ASSERT_FALSE(comp->GetValid());
}

/**
 * @tc.name: ParseComponent012
 * @tc.desc: Test parse component info with invalid style
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SecCompInfoHelperTest, ParseComponent012, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    ServiceTestCommon::BuildLocationComponentJson(jsonComponent);
    SecCompBase* comp = SecCompInfoHelper::ParseComponent(LOCATION_COMPONENT, jsonComponent);
    ASSERT_TRUE(comp->GetValid());

    auto& colorsJson = jsonComponent[JsonTagConstants::JSON_COLORS_TAG];
    colorsJson[JsonTagConstants::JSON_FONT_COLOR_TAG] = ServiceTestCommon::TEST_COLOR_YELLOW;
    comp = SecCompInfoHelper::ParseComponent(LOCATION_COMPONENT, jsonComponent);
    ASSERT_FALSE(comp->GetValid());

    colorsJson[JsonTagConstants::JSON_FONT_COLOR_TAG] = ServiceTestCommon::TEST_COLOR_RED;
    colorsJson[JsonTagConstants::JSON_ICON_COLOR_TAG] = ServiceTestCommon::TEST_COLOR_YELLOW;
    comp = SecCompInfoHelper::ParseComponent(LOCATION_COMPONENT, jsonComponent);
    ASSERT_FALSE(comp->GetValid());

    auto& styleJson = jsonComponent[JsonTagConstants::JSON_STYLE_TAG];
    styleJson[JsonTagConstants::JSON_BG_TAG] = SecCompBackground::NO_BG_TYPE;
    comp = SecCompInfoHelper::ParseComponent(LOCATION_COMPONENT, jsonComponent);
    ASSERT_FALSE(comp->GetValid());
}

/**
 * @tc.name: ParseComponent013
 * @tc.desc: Test parse component info with invalid style
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SecCompInfoHelperTest, ParseComponent013, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    ServiceTestCommon::BuildLocationComponentJson(jsonComponent);
    SecCompBase* comp = SecCompInfoHelper::ParseComponent(LOCATION_COMPONENT, jsonComponent);
    ASSERT_TRUE(comp->GetValid());

    auto& styleJson = jsonComponent[JsonTagConstants::JSON_STYLE_TAG];
    styleJson[JsonTagConstants::JSON_BG_TAG] = SecCompBackground::NO_BG_TYPE;
    auto& sizeJson = jsonComponent[JsonTagConstants::JSON_SIZE_TAG];
    auto& paddingJson = sizeJson[JsonTagConstants::JSON_PADDING_SIZE_TAG];
    paddingJson[JsonTagConstants::JSON_PADDING_RIGHT_TAG] = MIN_PADDING_WITHOUT_BG;
    paddingJson[JsonTagConstants::JSON_PADDING_BOTTOM_TAG] = MIN_PADDING_WITHOUT_BG;
    paddingJson[JsonTagConstants::JSON_PADDING_LEFT_TAG] = MIN_PADDING_WITHOUT_BG;
    comp = SecCompInfoHelper::ParseComponent(LOCATION_COMPONENT, jsonComponent);
    ASSERT_FALSE(comp->GetValid());

    paddingJson[JsonTagConstants::JSON_PADDING_TOP_TAG] = MIN_PADDING_WITHOUT_BG;
    paddingJson[JsonTagConstants::JSON_PADDING_RIGHT_TAG] = ServiceTestCommon::TEST_DIMENSION;
    paddingJson[JsonTagConstants::JSON_PADDING_BOTTOM_TAG] = MIN_PADDING_WITHOUT_BG;
    paddingJson[JsonTagConstants::JSON_PADDING_LEFT_TAG] = MIN_PADDING_WITHOUT_BG;
    comp = SecCompInfoHelper::ParseComponent(LOCATION_COMPONENT, jsonComponent);
    ASSERT_FALSE(comp->GetValid());
}

/**
 * @tc.name: ParseComponent014
 * @tc.desc: Test parse component info with invalid style
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SecCompInfoHelperTest, ParseComponent014, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    ServiceTestCommon::BuildLocationComponentJson(jsonComponent);
    SecCompBase* comp = SecCompInfoHelper::ParseComponent(LOCATION_COMPONENT, jsonComponent);
    ASSERT_TRUE(comp->GetValid());

    auto& styleJson = jsonComponent[JsonTagConstants::JSON_STYLE_TAG];
    styleJson[JsonTagConstants::JSON_BG_TAG] = SecCompBackground::NO_BG_TYPE;

    auto& sizeJson = jsonComponent[JsonTagConstants::JSON_SIZE_TAG];
    auto& paddingJson = sizeJson[JsonTagConstants::JSON_PADDING_SIZE_TAG];
    paddingJson[JsonTagConstants::JSON_PADDING_TOP_TAG] = MIN_PADDING_WITHOUT_BG;
    paddingJson[JsonTagConstants::JSON_PADDING_RIGHT_TAG] = MIN_PADDING_WITHOUT_BG;
    paddingJson[JsonTagConstants::JSON_PADDING_LEFT_TAG] = MIN_PADDING_WITHOUT_BG;
    comp = SecCompInfoHelper::ParseComponent(LOCATION_COMPONENT, jsonComponent);
    ASSERT_FALSE(comp->GetValid());

    paddingJson[JsonTagConstants::JSON_PADDING_TOP_TAG] = MIN_PADDING_WITHOUT_BG;
    paddingJson[JsonTagConstants::JSON_PADDING_RIGHT_TAG] = MIN_PADDING_WITHOUT_BG;
    paddingJson[JsonTagConstants::JSON_PADDING_BOTTOM_TAG] = MIN_PADDING_WITHOUT_BG;
    paddingJson[JsonTagConstants::JSON_PADDING_LEFT_TAG] = ServiceTestCommon::TEST_DIMENSION;
    comp = SecCompInfoHelper::ParseComponent(LOCATION_COMPONENT, jsonComponent);
    ASSERT_FALSE(comp->GetValid());
}


/**
 * @tc.name: ParseComponent015
 * @tc.desc: Test parse component info with similar color
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SecCompInfoHelperTest, ParseComponent015, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    ServiceTestCommon::BuildLocationComponentJson(jsonComponent);
    SecCompBase* comp = SecCompInfoHelper::ParseComponent(LOCATION_COMPONENT, jsonComponent);
    ASSERT_TRUE(comp->GetValid());

    jsonComponent[JsonTagConstants::JSON_STYLE_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_TEXT_TAG, LocationDesc::SELECT_LOCATION },
        { JsonTagConstants::JSON_ICON_TAG, LocationIcon::LINE_ICON },
        { JsonTagConstants::JSON_BG_TAG, SecCompBackground::CIRCLE }
    };
    jsonComponent[JsonTagConstants::JSON_COLORS_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_FONT_COLOR_TAG, ServiceTestCommon::TEST_COLOR_YELLOW },
        { JsonTagConstants::JSON_ICON_COLOR_TAG, ServiceTestCommon::TEST_COLOR_BLACK },
        { JsonTagConstants::JSON_BG_COLOR_TAG, ServiceTestCommon::TEST_COLOR_YELLOW }
    };
    comp = SecCompInfoHelper::ParseComponent(LOCATION_COMPONENT, jsonComponent);
    ASSERT_FALSE(comp->GetValid());
}

/**
 * @tc.name: CheckComponentValid001
 * @tc.desc: Test CheckComponentValid with invalid color
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SecCompInfoHelperTest, CheckComponentValid001, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    ServiceTestCommon::BuildLocationComponentJson(jsonComponent);
    SecCompBase* comp = SecCompInfoHelper::ParseComponent(LOCATION_COMPONENT, jsonComponent);
    ASSERT_TRUE(comp->GetValid());
    ASSERT_TRUE(SecCompInfoHelper::CheckComponentValid(comp));
    jsonComponent[JsonTagConstants::JSON_COLORS_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_FONT_COLOR_TAG, ServiceTestCommon::TEST_COLOR_RED },
        { JsonTagConstants::JSON_ICON_COLOR_TAG, ServiceTestCommon::TEST_COLOR_BLACK },
        { JsonTagConstants::JSON_BG_COLOR_TAG, ServiceTestCommon::TEST_COLOR_WHITE }
    };
    comp = SecCompInfoHelper::ParseComponent(LOCATION_COMPONENT, jsonComponent);
    ASSERT_TRUE(SecCompInfoHelper::CheckComponentValid(comp));
}

/**
 * @tc.name: CheckComponentValid002
 * @tc.desc: Test CheckComponentValid with invalid text or icon
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SecCompInfoHelperTest, CheckComponentValid002, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    ServiceTestCommon::BuildLocationComponentJson(jsonComponent);
    SecCompBase* comp = SecCompInfoHelper::ParseComponent(LOCATION_COMPONENT, jsonComponent);
    comp->text_ = UNKNOWN_TEXT;
    ASSERT_TRUE(SecCompInfoHelper::CheckComponentValid(comp));

    comp->text_ = static_cast<int32_t>(LocationDesc::SELECT_LOCATION);
    comp->icon_ = UNKNOWN_ICON;
    ASSERT_TRUE(SecCompInfoHelper::CheckComponentValid(comp));

    comp->text_ = UNKNOWN_TEXT;
    ASSERT_FALSE(SecCompInfoHelper::CheckComponentValid(comp));
}

/**
 * @tc.name: CheckComponentValid003
 * @tc.desc: Test CheckComponentValid with invalid type
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SecCompInfoHelperTest, CheckComponentValid003, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    ServiceTestCommon::BuildLocationComponentJson(jsonComponent);
    SecCompBase* comp = SecCompInfoHelper::ParseComponent(LOCATION_COMPONENT, jsonComponent);
    comp->type_ = SecCompType::UNKNOWN_SC_TYPE;
    ASSERT_FALSE(SecCompInfoHelper::CheckComponentValid(comp));

    comp->type_ = SecCompType::MAX_SC_TYPE;
    ASSERT_FALSE(SecCompInfoHelper::CheckComponentValid(comp));
}

/**
 * @tc.name: CheckComponentValid004
 * @tc.desc: Test CheckComponentValid with window scale changed
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompInfoHelperTest, CheckComponentValid004, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    ServiceTestCommon::BuildLocationComponentJson(jsonComponent);
    SecCompBase* comp = SecCompInfoHelper::ParseComponent(LOCATION_COMPONENT, jsonComponent);

    Rosen::WindowManager::GetInstance().result_ = Rosen::WMError::WM_OK;
    std::vector<sptr<Rosen::AccessibilityWindowInfo>> list;
    sptr<Rosen::AccessibilityWindowInfo> compWin = new Rosen::AccessibilityWindowInfo();
    compWin->wid_ = 0;
    compWin->layer_ = 1;
    compWin->scaleVal_ = 0.99; // change window scale to 0.99
    list.emplace_back(compWin);

    sptr<Rosen::AccessibilityWindowInfo> otherWin = new Rosen::AccessibilityWindowInfo();
    otherWin->wid_ = 1;
    otherWin->layer_ = 0;
    otherWin->scaleVal_ = 0.0;
    list.emplace_back(otherWin);
    Rosen::WindowManager::GetInstance().list_ = list;
    ASSERT_TRUE(SecCompInfoHelper::CheckComponentValid(comp));
}

/**
 * @tc.name: CheckComponentValid005
 * @tc.desc: Test CheckComponentValid with get GetWindowScale failed
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompInfoHelperTest, CheckComponentValid005, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    ServiceTestCommon::BuildLocationComponentJson(jsonComponent);
    SecCompBase* comp = SecCompInfoHelper::ParseComponent(LOCATION_COMPONENT, jsonComponent);

    Rosen::WindowManager::GetInstance().result_ = static_cast<OHOS::Rosen::WMError>(-1);
    ASSERT_TRUE(SecCompInfoHelper::CheckComponentValid(comp));
}
