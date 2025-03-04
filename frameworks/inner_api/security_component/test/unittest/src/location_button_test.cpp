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
#include "location_button_test.h"

#include <string>
#include "sec_comp_log.h"
#include "sec_comp_err.h"
#include "test_common.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Security::SecurityComponent;

namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
    LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "LocationButtonTest"};
static const std::string WRONG_TYPE = "wrongType";
}

void LocationButtonTest::SetUpTestCase()
{}

void LocationButtonTest::TearDownTestCase()
{}

void LocationButtonTest::SetUp()
{
    SC_LOG_INFO(LABEL, "setup");
}

void LocationButtonTest::TearDown()
{}

/**
 * @tc.name: FromJson001
 * @tc.desc: Test LocationButton from json success
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(LocationButtonTest, FromJson001, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    TestCommon::BuildLocationComponentInfo(jsonComponent);
    LocationButton comp;
    
    ASSERT_TRUE(comp.FromJson(jsonComponent));
}

/**
 * @tc.name: FromJson002
 * @tc.desc: Test empty LocationButton from json
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(LocationButtonTest, FromJson002, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    LocationButton comp;
    ASSERT_FALSE(comp.FromJson(jsonComponent));
}

/**
 * @tc.name: FromJson003
 * @tc.desc: Test location button from wrong type json
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(LocationButtonTest, FromJson003, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    TestCommon::BuildLocationComponentInfo(jsonComponent);
    LocationButton comp;
    ASSERT_TRUE(comp.FromJson(jsonComponent));

    jsonComponent[JsonTagConstants::JSON_SC_TYPE] = WRONG_TYPE;
    ASSERT_FALSE(comp.FromJson(jsonComponent));

    jsonComponent[JsonTagConstants::JSON_SC_TYPE] = 0;
    ASSERT_FALSE(comp.FromJson(jsonComponent));
}

/**
 * @tc.name: FromJson004
 * @tc.desc: Test location button from wrong rect json
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(LocationButtonTest, FromJson004, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    LocationButton comp;
    jsonComponent[JsonTagConstants::JSON_SC_TYPE] = LOCATION_COMPONENT;
    nlohmann::json wrongJson = nlohmann::json::parse("{", nullptr, false);
    jsonComponent[JsonTagConstants::JSON_RECT] = wrongJson;
    ASSERT_FALSE(comp.FromJson(jsonComponent));

    TestCommon::BuildLocationComponentInfo(jsonComponent);
    ASSERT_TRUE(comp.FromJson(jsonComponent));

    auto& rectJson = jsonComponent[JsonTagConstants::JSON_RECT];
    rectJson[JsonTagConstants::JSON_RECT_X] = WRONG_TYPE;
    ASSERT_FALSE(comp.FromJson(jsonComponent));

    rectJson[JsonTagConstants::JSON_RECT_X] = TestCommon::TEST_COORDINATE;
    rectJson[JsonTagConstants::JSON_RECT_Y] = WRONG_TYPE;
    ASSERT_FALSE(comp.FromJson(jsonComponent));

    rectJson[JsonTagConstants::JSON_RECT_Y] = TestCommon::TEST_COORDINATE;
    rectJson[JsonTagConstants::JSON_RECT_WIDTH] = WRONG_TYPE;
    ASSERT_FALSE(comp.FromJson(jsonComponent));

    rectJson[JsonTagConstants::JSON_RECT_WIDTH] = TestCommon::TEST_COORDINATE;
    rectJson[JsonTagConstants::JSON_RECT_HEIGHT] = WRONG_TYPE;
    ASSERT_FALSE(comp.FromJson(jsonComponent));
}

/**
 * @tc.name: FromJson005
 * @tc.desc: Test location button from wrong rect json
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(LocationButtonTest, FromJson005, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    LocationButton comp;
    jsonComponent[JsonTagConstants::JSON_SC_TYPE] = LOCATION_COMPONENT;
    nlohmann::json wrongJson = nlohmann::json::parse("{", nullptr, false);
    jsonComponent[JsonTagConstants::JSON_WINDOW_RECT] = wrongJson;
    ASSERT_FALSE(comp.FromJson(jsonComponent));

    TestCommon::BuildLocationComponentInfo(jsonComponent);
    ASSERT_TRUE(comp.FromJson(jsonComponent));

    auto& rectJson = jsonComponent[JsonTagConstants::JSON_WINDOW_RECT];
    rectJson[JsonTagConstants::JSON_RECT_X] = WRONG_TYPE;
    ASSERT_FALSE(comp.FromJson(jsonComponent));

    rectJson[JsonTagConstants::JSON_RECT_X] = TestCommon::TEST_COORDINATE;
    rectJson[JsonTagConstants::JSON_RECT_Y] = WRONG_TYPE;
    ASSERT_FALSE(comp.FromJson(jsonComponent));

    rectJson[JsonTagConstants::JSON_RECT_Y] = TestCommon::TEST_COORDINATE;
    rectJson[JsonTagConstants::JSON_RECT_WIDTH] = WRONG_TYPE;
    ASSERT_FALSE(comp.FromJson(jsonComponent));

    rectJson[JsonTagConstants::JSON_RECT_WIDTH] = TestCommon::TEST_COORDINATE;
    rectJson[JsonTagConstants::JSON_RECT_HEIGHT] = WRONG_TYPE;
    ASSERT_FALSE(comp.FromJson(jsonComponent));
}

/**
 * @tc.name: FromJson006
 * @tc.desc: Test location button from wrong size json
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(LocationButtonTest, FromJson006, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    LocationButton comp;
    TestCommon::BuildLocationComponentInfo(jsonComponent);
    ASSERT_TRUE(comp.FromJson(jsonComponent));

    auto& sizeJson = jsonComponent[JsonTagConstants::JSON_SIZE_TAG];
    sizeJson[JsonTagConstants::JSON_FONT_SIZE_TAG] = WRONG_TYPE;
    ASSERT_FALSE(comp.FromJson(jsonComponent));

    sizeJson[JsonTagConstants::JSON_FONT_SIZE_TAG] = TestCommon::TEST_SIZE;
    sizeJson[JsonTagConstants::JSON_ICON_SIZE_TAG] = WRONG_TYPE;
    ASSERT_FALSE(comp.FromJson(jsonComponent));

    sizeJson[JsonTagConstants::JSON_ICON_SIZE_TAG] = TestCommon::TEST_SIZE;
    sizeJson[JsonTagConstants::JSON_TEXT_ICON_PADDING_TAG] = WRONG_TYPE;
    ASSERT_FALSE(comp.FromJson(jsonComponent));
}

/**
 * @tc.name: FromJson007
 * @tc.desc: Test location button from wrong size json
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(LocationButtonTest, FromJson007, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    LocationButton comp;
    TestCommon::BuildLocationComponentInfo(jsonComponent);
    ASSERT_TRUE(comp.FromJson(jsonComponent));

    auto& sizeJson = jsonComponent[JsonTagConstants::JSON_SIZE_TAG];
    auto& paddingJson = sizeJson[JsonTagConstants::JSON_PADDING_SIZE_TAG];
    paddingJson[JsonTagConstants::JSON_PADDING_TOP_TAG] = WRONG_TYPE;
    ASSERT_FALSE(comp.FromJson(jsonComponent));

    paddingJson[JsonTagConstants::JSON_PADDING_TOP_TAG] = TestCommon::TEST_DIMENSION;
    paddingJson[JsonTagConstants::JSON_PADDING_RIGHT_TAG] = WRONG_TYPE;
    ASSERT_FALSE(comp.FromJson(jsonComponent));
}

/**
 * @tc.name: FromJson008
 * @tc.desc: Test location button from wrong size json
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(LocationButtonTest, FromJson008, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    LocationButton comp;
    TestCommon::BuildLocationComponentInfo(jsonComponent);
    ASSERT_TRUE(comp.FromJson(jsonComponent));

    auto& sizeJson = jsonComponent[JsonTagConstants::JSON_SIZE_TAG];
    auto& paddingJson = sizeJson[JsonTagConstants::JSON_PADDING_SIZE_TAG];
    paddingJson[JsonTagConstants::JSON_PADDING_BOTTOM_TAG] = WRONG_TYPE;
    ASSERT_FALSE(comp.FromJson(jsonComponent));

    paddingJson[JsonTagConstants::JSON_PADDING_BOTTOM_TAG] = TestCommon::TEST_DIMENSION;
    paddingJson[JsonTagConstants::JSON_PADDING_LEFT_TAG] = WRONG_TYPE;
    ASSERT_FALSE(comp.FromJson(jsonComponent));
}

/**
 * @tc.name: FromJson009
 * @tc.desc: Test location button from wrong border and parent json
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(LocationButtonTest, FromJson009, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    LocationButton comp;
    TestCommon::BuildLocationComponentInfo(jsonComponent);
    ASSERT_TRUE(comp.FromJson(jsonComponent));

    jsonComponent[JsonTagConstants::JSON_BORDER_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_BORDER_WIDTH_TAG, WRONG_TYPE },
    };
    ASSERT_FALSE(comp.FromJson(jsonComponent));

    TestCommon::BuildLocationComponentInfo(jsonComponent);
    jsonComponent[JsonTagConstants::JSON_PARENT_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_PARENT_EFFECT_TAG, WRONG_TYPE },
    };
    ASSERT_FALSE(comp.FromJson(jsonComponent));
}

/**
 * @tc.name: FromJson010
 * @tc.desc: Test location button from wrong type params json
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(LocationButtonTest, FromJson010, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    TestCommon::BuildLocationComponentInfo(jsonComponent);
    LocationButton button;
    ASSERT_TRUE(button.FromJson(jsonComponent));

    auto& styleJson = jsonComponent[JsonTagConstants::JSON_STYLE_TAG];
    styleJson[JsonTagConstants::JSON_TEXT_TAG] = WRONG_TYPE;
    ASSERT_FALSE(button.FromJson(jsonComponent));

    styleJson[JsonTagConstants::JSON_TEXT_TAG] = LocationDesc::SELECT_LOCATION;
    styleJson[JsonTagConstants::JSON_ICON_TAG] = WRONG_TYPE;
    ASSERT_FALSE(button.FromJson(jsonComponent));

    styleJson[JsonTagConstants::JSON_ICON_TAG] = LocationIcon::LINE_ICON;
    styleJson[JsonTagConstants::JSON_BG_TAG] = WRONG_TYPE;
    ASSERT_FALSE(button.FromJson(jsonComponent));
}

/**
 * @tc.name: FromJson011
 * @tc.desc: Test location button from wrong type params json
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(LocationButtonTest, FromJson011, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    TestCommon::BuildLocationComponentInfo(jsonComponent);
    LocationButton button;
    ASSERT_TRUE(button.FromJson(jsonComponent));

    auto& colorJson = jsonComponent[JsonTagConstants::JSON_COLORS_TAG];
    colorJson[JsonTagConstants::JSON_FONT_COLOR_TAG] = WRONG_TYPE;
    ASSERT_FALSE(button.FromJson(jsonComponent));

    colorJson[JsonTagConstants::JSON_FONT_COLOR_TAG] = TestCommon::TEST_COLOR_RED;
    colorJson[JsonTagConstants::JSON_ICON_COLOR_TAG] = WRONG_TYPE;
    ASSERT_FALSE(button.FromJson(jsonComponent));

    colorJson[JsonTagConstants::JSON_ICON_COLOR_TAG] = TestCommon::TEST_COLOR_BLUE;
    colorJson[JsonTagConstants::JSON_BG_COLOR_TAG] = WRONG_TYPE;
    ASSERT_FALSE(button.FromJson(jsonComponent));
}

/**
 * @tc.name: FromJson012
 * @tc.desc: Test location button from wrong value params json
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(LocationButtonTest, FromJson012, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    TestCommon::BuildLocationComponentInfo(jsonComponent);
    LocationButton button;

    ASSERT_TRUE(button.FromJson(jsonComponent));

    auto& styleJson = jsonComponent[JsonTagConstants::JSON_STYLE_TAG];
    styleJson[JsonTagConstants::JSON_TEXT_TAG] = UNKNOWN_TEXT;
    ASSERT_FALSE(button.FromJson(jsonComponent));

    styleJson[JsonTagConstants::JSON_TEXT_TAG] = LocationDesc::SELECT_LOCATION;
    styleJson[JsonTagConstants::JSON_ICON_TAG] = UNKNOWN_ICON;
    ASSERT_FALSE(button.FromJson(jsonComponent));

    styleJson[JsonTagConstants::JSON_ICON_TAG] = LocationIcon::LINE_ICON;
    styleJson[JsonTagConstants::JSON_BG_TAG] = SecCompBackground::UNKNOWN_BG;
    ASSERT_FALSE(button.FromJson(jsonComponent));

    styleJson[JsonTagConstants::JSON_BG_TAG] = SecCompBackground::CIRCLE;
    styleJson[JsonTagConstants::JSON_TEXT_TAG] = LocationDesc::MAX_LABEL_TYPE;
    ASSERT_FALSE(button.FromJson(jsonComponent));

    styleJson[JsonTagConstants::JSON_TEXT_TAG] = LocationDesc::SELECT_LOCATION;
    styleJson[JsonTagConstants::JSON_ICON_TAG] = LocationIcon::MAX_ICON_TYPE;
    ASSERT_FALSE(button.FromJson(jsonComponent));

    styleJson[JsonTagConstants::JSON_ICON_TAG] = LocationIcon::LINE_ICON;
    styleJson[JsonTagConstants::JSON_BG_TAG] = SecCompBackground::MAX_BG_TYPE;
    ASSERT_FALSE(button.FromJson(jsonComponent));
}

/**
 * @tc.name: ToJsonStr001
 * @tc.desc: Test ToJsonStr normal branch
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(LocationButtonTest, ToJsonStr001, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    TestCommon::BuildLocationComponentInfo(jsonComponent);
    LocationButton button;

    ASSERT_TRUE(button.FromJson(jsonComponent));
    ASSERT_EQ(jsonComponent.dump(), button.ToJsonStr());
}

/**
 * @tc.name: CompareComponentBasicInfo001
 * @tc.desc: Test CompareComponentBasicInfo other is null
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(LocationButtonTest, CompareComponentBasicInfo001, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    TestCommon::BuildLocationComponentInfo(jsonComponent);
    LocationButton button;

    ASSERT_FALSE(button.CompareComponentBasicInfo(nullptr, true));
}

/**
 * @tc.name: CompareLocationButton001
 * @tc.desc: Test LocationButton compare
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(LocationButtonTest, CompareLocationButton001, TestSize.Level1)
{
    LocationButton button1;
    LocationButton button2;

    nlohmann::json jsonComponent;
    TestCommon::BuildLocationComponentInfo(jsonComponent);

    ASSERT_TRUE(button1.FromJson(jsonComponent));
    ASSERT_TRUE(button2.FromJson(jsonComponent));
    ASSERT_TRUE(button1.CompareComponentBasicInfo(&button2, true));

    button1.text_ = UNKNOWN_TEXT;
    ASSERT_FALSE(button1.CompareComponentBasicInfo(&button2, true));
    button1.text_ = static_cast<int32_t>(LocationDesc::SELECT_LOCATION);

    button1.icon_ = UNKNOWN_ICON;
    ASSERT_FALSE(button1.CompareComponentBasicInfo(&button2, true));
    button1.icon_ = static_cast<int32_t>(LocationIcon::LINE_ICON);

    button1.bg_ = SecCompBackground::UNKNOWN_BG;
    ASSERT_FALSE(button1.CompareComponentBasicInfo(&button2, true));
    button1.bg_ = SecCompBackground::CIRCLE;

    ASSERT_TRUE(button1.CompareComponentBasicInfo(&button2, true));
}

/**
 * @tc.name: CompareLocationButton002
 * @tc.desc: Test LocationButton compare
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(LocationButtonTest, CompareLocationButton002, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    LocationButton comp1;
    TestCommon::BuildLocationComponentInfo(jsonComponent);
    ASSERT_TRUE(comp1.FromJson(jsonComponent));
    LocationButton comp2 = comp1;

    comp1.type_ = SAVE_COMPONENT;
    ASSERT_FALSE(comp1.CompareComponentBasicInfo(&comp2, true));
    comp1.type_ = LOCATION_COMPONENT;

    comp1.fontSize_ = DEFAULT_DIMENSION;
    ASSERT_FALSE(comp1.CompareComponentBasicInfo(&comp2, true));
    comp1.fontSize_ = TestCommon::TEST_SIZE;

    comp1.iconSize_ = DEFAULT_DIMENSION;
    ASSERT_FALSE(comp1.CompareComponentBasicInfo(&comp2, true));
    comp1.iconSize_ = TestCommon::TEST_SIZE;

    comp1.padding_.top = DEFAULT_DIMENSION;
    ASSERT_FALSE(comp1.CompareComponentBasicInfo(&comp2, true));
    comp1.padding_.top = TestCommon::TEST_DIMENSION;

    comp1.padding_.right = DEFAULT_DIMENSION;
    ASSERT_FALSE(comp1.CompareComponentBasicInfo(&comp2, true));
    comp1.padding_.right = TestCommon::TEST_DIMENSION;

    comp1.padding_.bottom = DEFAULT_DIMENSION;
    ASSERT_FALSE(comp1.CompareComponentBasicInfo(&comp2, true));
    comp1.padding_.bottom = TestCommon::TEST_DIMENSION;

    comp1.padding_.left = DEFAULT_DIMENSION;
    ASSERT_FALSE(comp1.CompareComponentBasicInfo(&comp2, true));
    comp1.padding_.left = TestCommon::TEST_DIMENSION;

    comp1.textIconSpace_ = DEFAULT_DIMENSION;
    ASSERT_FALSE(comp1.CompareComponentBasicInfo(&comp2, true));
    comp1.textIconSpace_ = TestCommon::TEST_SIZE;

    comp1.borderWidth_ = DEFAULT_DIMENSION;
    ASSERT_FALSE(comp1.CompareComponentBasicInfo(&comp2, true));
    comp1.borderWidth_ = TestCommon::TEST_SIZE;

    comp1.fontColor_.value = TestCommon::TEST_DIFF_COLOR;
    ASSERT_FALSE(comp1.CompareComponentBasicInfo(&comp2, true));
    comp1.fontColor_.value = TestCommon::TEST_COLOR_RED;

    comp1.bgColor_.value = TestCommon::TEST_DIFF_COLOR;
    ASSERT_FALSE(comp1.CompareComponentBasicInfo(&comp2, true));
    comp1.bgColor_.value = TestCommon::TEST_COLOR_YELLOW;

    comp1.iconColor_.value = TestCommon::TEST_DIFF_COLOR;
    ASSERT_FALSE(comp1.CompareComponentBasicInfo(&comp2, true));
    comp1.iconColor_.value = TestCommon::TEST_COLOR_BLUE;

    ASSERT_TRUE(comp1.CompareComponentBasicInfo(&comp2, true));
}

/**
 * @tc.name: CompareLocationButton003
 * @tc.desc: Test LocationButton compare
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(LocationButtonTest, CompareLocationButton003, TestSize.Level1)
{
    LocationButton button1;
    SaveButton button2;
    nlohmann::json jsonComponent;
    TestCommon::BuildLocationComponentInfo(jsonComponent);

    ASSERT_TRUE(button1.FromJson(jsonComponent));
    ASSERT_FALSE(button1.CompareComponentBasicInfo(&button2, false));
}
