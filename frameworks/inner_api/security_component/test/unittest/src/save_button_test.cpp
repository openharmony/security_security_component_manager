/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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
#include "save_button_test.h"

#include <string>
#include "sec_comp_log.h"
#include "sec_comp_err.h"
#include "test_common.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Security::SecurityComponent;

namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
    LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "SaveButtonTest"};
static const std::string WRONG_TYPE = "wrongType";
}

void SaveButtonTest::SetUpTestCase()
{}

void SaveButtonTest::TearDownTestCase()
{}

void SaveButtonTest::SetUp()
{
    SC_LOG_INFO(LABEL, "setup");
}

void SaveButtonTest::TearDown()
{}

/**
 * @tc.name: IsParamValid001
 * @tc.desc: Test save button from wrong value params json
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaveButtonTest, IsParamValid001, TestSize.Level0)
{
    nlohmann::json jsonComponent;
    TestCommon::BuildSaveComponentInfo(jsonComponent);
    SaveButton button;
    std::string message;
    bool isClicked = true;

    ASSERT_TRUE(button.FromJson(jsonComponent, message, isClicked));

    auto& stylesJson = jsonComponent[JsonTagConstants::JSON_STYLE_TAG];
    stylesJson[JsonTagConstants::JSON_TEXT_TAG] = UNKNOWN_TEXT;
    ASSERT_FALSE(button.FromJson(jsonComponent, message, isClicked));

    stylesJson[JsonTagConstants::JSON_TEXT_TAG] = SaveDesc::DOWNLOAD;
    stylesJson[JsonTagConstants::JSON_ICON_TAG] = UNKNOWN_ICON;
    ASSERT_FALSE(button.FromJson(jsonComponent, message, isClicked));

    stylesJson[JsonTagConstants::JSON_ICON_TAG] = SaveIcon::LINE_ICON;
    stylesJson[JsonTagConstants::JSON_BG_TAG] = SecCompBackground::UNKNOWN_BG;
    ASSERT_FALSE(button.FromJson(jsonComponent, message, isClicked));

    stylesJson[JsonTagConstants::JSON_BG_TAG] = SecCompBackground::CIRCLE;
    stylesJson[JsonTagConstants::JSON_TEXT_TAG] = SaveDesc::MAX_LABEL_TYPE;
    ASSERT_FALSE(button.FromJson(jsonComponent, message, isClicked));

    stylesJson[JsonTagConstants::JSON_TEXT_TAG] = SaveDesc::DOWNLOAD;
    stylesJson[JsonTagConstants::JSON_ICON_TAG] = SaveIcon::MAX_ICON_TYPE;
    ASSERT_FALSE(button.FromJson(jsonComponent, message, isClicked));

    stylesJson[JsonTagConstants::JSON_ICON_TAG] = SaveIcon::LINE_ICON;
    stylesJson[JsonTagConstants::JSON_BG_TAG] = SecCompBackground::MAX_BG_TYPE;
    ASSERT_FALSE(button.FromJson(jsonComponent, message, isClicked));

    stylesJson[JsonTagConstants::JSON_ICON_TAG] = SaveIcon::PICTURE_ICON;
    stylesJson[JsonTagConstants::JSON_BG_TAG] = SecCompBackground::CIRCLE;
    ASSERT_FALSE(button.FromJson(jsonComponent, message, isClicked));
}

/**
 * @tc.name: CompareSaveButton001
 * @tc.desc: Test compare save button
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaveButtonTest, CompareSaveButton001, TestSize.Level0)
{
    SaveButton button1;
    SaveButton button2;

    nlohmann::json jsonComponent;
    std::string message;
    bool isClicked = true;
    TestCommon::BuildSaveComponentInfo(jsonComponent);

    ASSERT_TRUE(button1.FromJson(jsonComponent, message, isClicked));
    ASSERT_TRUE(button2.FromJson(jsonComponent, message, isClicked));
    ASSERT_TRUE(button1.CompareComponentBasicInfo(&button2, true));

    button1.text_ = UNKNOWN_TEXT;
    ASSERT_FALSE(button1.CompareComponentBasicInfo(&button2, true));
    button1.text_ = static_cast<int32_t>(SaveDesc::DOWNLOAD);

    button1.icon_ = UNKNOWN_ICON;
    ASSERT_FALSE(button1.CompareComponentBasicInfo(&button2, true));
    button1.icon_ = static_cast<int32_t>(SaveIcon::LINE_ICON);

    button1.bg_ = SecCompBackground::UNKNOWN_BG;
    ASSERT_FALSE(button1.CompareComponentBasicInfo(&button2, true));
    button1.bg_ = SecCompBackground::CIRCLE;

    ASSERT_TRUE(button1.CompareComponentBasicInfo(&button2, true));
}

/**
 * @tc.name: CompareSaveButton002
 * @tc.desc: Test SaveButton compare
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaveButtonTest, CompareSaveButton002, TestSize.Level0)
{
    nlohmann::json jsonComponent;
    SaveButton comp1;
    TestCommon::BuildSaveComponentInfo(jsonComponent);
    std::string message;
    bool isClicked = true;
    ASSERT_TRUE(comp1.FromJson(jsonComponent, message, isClicked));
    SaveButton comp2 = comp1;
    comp2.isClickEvent_ = true;

    comp1.type_ = PASTE_COMPONENT;
    ASSERT_FALSE(comp1.CompareComponentBasicInfo(&comp2, true));
    comp1.type_ = SAVE_COMPONENT;

    comp1.icon_ = static_cast<int32_t>(SaveIcon::FILLED_ICON);
    ASSERT_FALSE(comp1.CompareComponentBasicInfo(&comp2, true));
    comp1.icon_ = static_cast<int32_t>(SaveIcon::LINE_ICON);

    comp1.text_ = static_cast<int32_t>(SaveDesc::DOWNLOAD_AND_SHARE);
    ASSERT_FALSE(comp1.CompareComponentBasicInfo(&comp2, true));
    comp1.text_ = static_cast<int32_t>(SaveDesc::DOWNLOAD);

    comp1.bg_ = SecCompBackground::CAPSULE;
    ASSERT_FALSE(comp1.CompareComponentBasicInfo(&comp2, true));
    comp1.bg_ = SecCompBackground::CIRCLE;

    ASSERT_TRUE(comp1.CompareComponentBasicInfo(&comp2, true));
}

/**
 * @tc.name: CompareSaveButton003
 * @tc.desc: Test SaveButton compare
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaveButtonTest, CompareSaveButton003, TestSize.Level0)
{
    SaveButton button1;
    PasteButton button2;
    nlohmann::json jsonComponent;
    std::string message;
    bool isClicked = true;
    TestCommon::BuildSaveComponentInfo(jsonComponent);

    ASSERT_TRUE(button1.FromJson(jsonComponent, message, isClicked));
    ASSERT_FALSE(button1.CompareComponentBasicInfo(&button2, true));
}

/**
 * @tc.name: IsCorrespondenceType001
 * @tc.desc: Test IsCorrespondenceType
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaveButtonTest, IsCorrespondenceType001, TestSize.Level1)
{
    SaveButton button;
    button.type_ = SAVE_COMPONENT;
    EXPECT_TRUE(button.IsCorrespondenceType());
    button.type_ = PASTE_COMPONENT;
    EXPECT_FALSE(button.IsCorrespondenceType());
}

/**
 * @tc.name: FromJson001
 * @tc.desc: Test SaveButton from json success
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaveButtonTest, FromJson001, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    SaveButton comp;
    std::string message;
    bool isClicked = true;

    TestCommon::BuildSaveComponentInfo(jsonComponent);
    EXPECT_TRUE(comp.FromJson(jsonComponent, message, isClicked));
}

/**
 * @tc.name: FromJson002
 * @tc.desc: Test empty SaveButton from json
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaveButtonTest, FromJson002, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    SaveButton comp;
    std::string message;
    bool isClicked = true;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));
}

/**
 * @tc.name: FromJson003
 * @tc.desc: Test save button from wrong type json
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaveButtonTest, FromJson003, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    SaveButton comp;
    std::string message;
    bool isClicked = true;
    TestCommon::BuildSaveComponentInfo(jsonComponent);
    EXPECT_TRUE(comp.FromJson(jsonComponent, message, isClicked));

    jsonComponent[JsonTagConstants::JSON_SC_TYPE] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));

    jsonComponent[JsonTagConstants::JSON_SC_TYPE] = UNKNOWN_SC_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));

    jsonComponent[JsonTagConstants::JSON_SC_TYPE] = MAX_SC_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));
}

/**
 * @tc.name: FromJson004
 * @tc.desc: Test save button from wrong node id json
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaveButtonTest, FromJson004, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    SaveButton comp;
    std::string message;
    bool isClicked = true;
    TestCommon::BuildSaveComponentInfo(jsonComponent);
    EXPECT_TRUE(comp.FromJson(jsonComponent, message, isClicked));

    jsonComponent[JsonTagConstants::JSON_NODE_ID] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));
}

/**
 * @tc.name: FromJson005
 * @tc.desc: Test save button from wrong wearable info json
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaveButtonTest, FromJson005, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    SaveButton comp;
    std::string message;
    bool isClicked = true;
    TestCommon::BuildSaveComponentInfo(jsonComponent);
    EXPECT_TRUE(comp.FromJson(jsonComponent, message, isClicked));

    jsonComponent[JsonTagConstants::JSON_IS_WEARABLE] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));
}

/**
 * @tc.name: FromJson006
 * @tc.desc: Test save button from wrong parent json
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaveButtonTest, FromJson006, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    SaveButton comp;
    std::string message;
    bool isClicked = true;
    TestCommon::BuildSaveComponentInfo(jsonComponent);
    EXPECT_TRUE(comp.FromJson(jsonComponent, message, isClicked));

    jsonComponent[JsonTagConstants::JSON_PARENT_TAG] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));

    TestCommon::BuildSaveComponentInfo(jsonComponent);
    auto& parentJson = jsonComponent[JsonTagConstants::JSON_PARENT_TAG];
    parentJson[JsonTagConstants::JSON_PARENT_EFFECT_TAG] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));

    // Test the item of JSON_IS_CLIPPED_TAG is of wrong type
    parentJson[JsonTagConstants::JSON_PARENT_EFFECT_TAG] = false;
    parentJson[JsonTagConstants::JSON_IS_CLIPPED_TAG] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));

    // Test the item of JSON_TOP_CLIP_TAG is of wrong type
    parentJson[JsonTagConstants::JSON_IS_CLIPPED_TAG] = false;
    parentJson[JsonTagConstants::JSON_TOP_CLIP_TAG] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));

    // Test the item of JSON_BOTTOM_CLIP_TAG is of wrong type
    parentJson[JsonTagConstants::JSON_TOP_CLIP_TAG] = 0.0;
    parentJson[JsonTagConstants::JSON_BOTTOM_CLIP_TAG] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));

    // Test the item of JSON_LEFT_CLIP_TAG is of wrong type
    parentJson[JsonTagConstants::JSON_BOTTOM_CLIP_TAG] = 0.0;
    parentJson[JsonTagConstants::JSON_LEFT_CLIP_TAG] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));

    // Test the item of JSON_RIGHT_CLIP_TAG is of wrong type
    parentJson[JsonTagConstants::JSON_LEFT_CLIP_TAG] = 0.0;
    parentJson[JsonTagConstants::JSON_RIGHT_CLIP_TAG] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));

    // Test the item of JSON_PARENT_TAG_TAG is of wrong type
    int32_t tagIsIntType = 1;
    parentJson[JsonTagConstants::JSON_RIGHT_CLIP_TAG] = 0.0;
    parentJson[JsonTagConstants::JSON_PARENT_TAG_TAG] = tagIsIntType;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));
}

/**
 * @tc.name: FromJson007
 * @tc.desc: Test save button from wrong rect json
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaveButtonTest, FromJson007, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    SaveButton comp;
    std::string message;
    bool isClicked = true;
    TestCommon::BuildSaveComponentInfo(jsonComponent);
    EXPECT_TRUE(comp.FromJson(jsonComponent, message, isClicked));

    jsonComponent[JsonTagConstants::JSON_RECT] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));

    TestCommon::BuildSaveComponentInfo(jsonComponent);
    auto& rectJson = jsonComponent[JsonTagConstants::JSON_RECT];
    rectJson[JsonTagConstants::JSON_RECT_X] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));

    rectJson[JsonTagConstants::JSON_RECT_X] = TestCommon::TEST_COORDINATE;
    rectJson[JsonTagConstants::JSON_RECT_Y] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));

    rectJson[JsonTagConstants::JSON_RECT_Y] = TestCommon::TEST_COORDINATE;
    rectJson[JsonTagConstants::JSON_RECT_WIDTH] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));

    rectJson[JsonTagConstants::JSON_RECT_WIDTH] = TestCommon::TEST_COORDINATE;
    rectJson[JsonTagConstants::JSON_RECT_HEIGHT] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));
}

/**
 * @tc.name: FromJson008
 * @tc.desc: Test save button from wrong icon&font size json
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaveButtonTest, FromJson008, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    SaveButton comp;
    std::string message;
    bool isClicked = true;
    TestCommon::BuildSaveComponentInfo(jsonComponent);
    EXPECT_TRUE(comp.FromJson(jsonComponent, message, isClicked));

    jsonComponent[JsonTagConstants::JSON_SIZE_TAG] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));

    TestCommon::BuildSaveComponentInfo(jsonComponent);
    auto& sizeJson = jsonComponent[JsonTagConstants::JSON_SIZE_TAG];
    sizeJson[JsonTagConstants::JSON_FONT_SIZE_TAG] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));

    sizeJson[JsonTagConstants::JSON_FONT_SIZE_TAG] = TestCommon::TEST_SIZE;
    sizeJson[JsonTagConstants::JSON_ICON_SIZE_TAG] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));

    sizeJson[JsonTagConstants::JSON_ICON_SIZE_TAG] = TestCommon::TEST_SIZE;
    sizeJson[JsonTagConstants::JSON_TEXT_ICON_PADDING_TAG] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));
}

/**
 * @tc.name: FromJson009
 * @tc.desc: Test save button from wrong padding size json
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaveButtonTest, FromJson009, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    SaveButton comp;
    std::string message;
    bool isClicked = true;
    TestCommon::BuildSaveComponentInfo(jsonComponent);
    EXPECT_TRUE(comp.FromJson(jsonComponent, message, isClicked));

    auto& sizeJson = jsonComponent[JsonTagConstants::JSON_SIZE_TAG];
    sizeJson[JsonTagConstants::JSON_PADDING_SIZE_TAG] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));

    TestCommon::BuildSaveComponentInfo(jsonComponent);
    auto& paddingJson = sizeJson[JsonTagConstants::JSON_PADDING_SIZE_TAG];
    paddingJson[JsonTagConstants::JSON_TOP_TAG] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));

    paddingJson[JsonTagConstants::JSON_TOP_TAG] = TestCommon::TEST_DIMENSION;
    paddingJson[JsonTagConstants::JSON_RIGHT_TAG] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));

    paddingJson[JsonTagConstants::JSON_RIGHT_TAG] = TestCommon::TEST_DIMENSION;
    paddingJson[JsonTagConstants::JSON_BOTTOM_TAG] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));

    paddingJson[JsonTagConstants::JSON_BOTTOM_TAG] = TestCommon::TEST_DIMENSION;
    paddingJson[JsonTagConstants::JSON_LEFT_TAG] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));
}

/**
 * @tc.name: FromJson010
 * @tc.desc: Test save button from wrong border radius json
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaveButtonTest, FromJson010, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    SaveButton comp;
    std::string message;
    bool isClicked = true;
    TestCommon::BuildSaveComponentInfo(jsonComponent);
    EXPECT_TRUE(comp.FromJson(jsonComponent, message, isClicked));

    auto& sizeJson = jsonComponent[JsonTagConstants::JSON_SIZE_TAG];
    sizeJson[JsonTagConstants::JSON_BORDER_RADIUS_TAG] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));

    TestCommon::BuildSaveComponentInfo(jsonComponent);
    auto& borderRadiusJson = sizeJson[JsonTagConstants::JSON_BORDER_RADIUS_TAG];
    borderRadiusJson[JsonTagConstants::JSON_LEFT_TOP_TAG] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));

    borderRadiusJson[JsonTagConstants::JSON_LEFT_TOP_TAG] = TestCommon::TEST_DIMENSION;
    borderRadiusJson[JsonTagConstants::JSON_RIGHT_TOP_TAG] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));

    borderRadiusJson[JsonTagConstants::JSON_RIGHT_TOP_TAG] = TestCommon::TEST_DIMENSION;
    borderRadiusJson[JsonTagConstants::JSON_LEFT_BOTTOM_TAG] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));

    borderRadiusJson[JsonTagConstants::JSON_LEFT_BOTTOM_TAG] = TestCommon::TEST_DIMENSION;
    borderRadiusJson[JsonTagConstants::JSON_RIGHT_BOTTOM_TAG] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));
}

/**
 * @tc.name: FromJson011
 * @tc.desc: Test save button from wrong color params json
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaveButtonTest, FromJson011, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    SaveButton button;
    std::string message;
    bool isClicked = true;
    TestCommon::BuildSaveComponentInfo(jsonComponent);
    EXPECT_TRUE(button.FromJson(jsonComponent, message, isClicked));

    jsonComponent[JsonTagConstants::JSON_COLORS_TAG] = WRONG_TYPE;
    EXPECT_FALSE(button.FromJson(jsonComponent, message, isClicked));

    TestCommon::BuildSaveComponentInfo(jsonComponent);
    auto& colorJson = jsonComponent[JsonTagConstants::JSON_COLORS_TAG];
    colorJson[JsonTagConstants::JSON_FONT_COLOR_TAG] = WRONG_TYPE;
    EXPECT_FALSE(button.FromJson(jsonComponent, message, isClicked));

    colorJson[JsonTagConstants::JSON_FONT_COLOR_TAG] = TestCommon::TEST_COLOR_RED;
    colorJson[JsonTagConstants::JSON_ICON_COLOR_TAG] = WRONG_TYPE;
    EXPECT_FALSE(button.FromJson(jsonComponent, message, isClicked));

    colorJson[JsonTagConstants::JSON_ICON_COLOR_TAG] = TestCommon::TEST_COLOR_BLUE;
    colorJson[JsonTagConstants::JSON_BG_COLOR_TAG] = WRONG_TYPE;
    EXPECT_FALSE(button.FromJson(jsonComponent, message, isClicked));
}

/**
 * @tc.name: FromJson012
 * @tc.desc: Test save button from wrong border json
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaveButtonTest, FromJson012, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    SaveButton comp;
    std::string message;
    bool isClicked = true;
    TestCommon::BuildSaveComponentInfo(jsonComponent);
    EXPECT_TRUE(comp.FromJson(jsonComponent, message, isClicked));

    jsonComponent[JsonTagConstants::JSON_BORDER_TAG] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));

    jsonComponent[JsonTagConstants::JSON_BORDER_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_BORDER_WIDTH_TAG, WRONG_TYPE },
    };
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));
}

/**
 * @tc.name: FromJson013
 * @tc.desc: Test save button from wrong style json
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaveButtonTest, FromJson013, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    SaveButton button;
    std::string message;
    bool isClicked = true;
    TestCommon::BuildSaveComponentInfo(jsonComponent);
    EXPECT_TRUE(button.FromJson(jsonComponent, message, isClicked));

    jsonComponent[JsonTagConstants::JSON_STYLE_TAG] = WRONG_TYPE;
    EXPECT_FALSE(button.FromJson(jsonComponent, message, isClicked));

    TestCommon::BuildSaveComponentInfo(jsonComponent);
    auto& styleJson = jsonComponent[JsonTagConstants::JSON_STYLE_TAG];
    styleJson[JsonTagConstants::JSON_TEXT_TAG] = WRONG_TYPE;
    EXPECT_FALSE(button.FromJson(jsonComponent, message, isClicked));

    styleJson[JsonTagConstants::JSON_TEXT_TAG] = SaveDesc::DOWNLOAD;
    styleJson[JsonTagConstants::JSON_ICON_TAG] = WRONG_TYPE;
    EXPECT_FALSE(button.FromJson(jsonComponent, message, isClicked));

    styleJson[JsonTagConstants::JSON_ICON_TAG] = SaveIcon::LINE_ICON;
    styleJson[JsonTagConstants::JSON_BG_TAG] = WRONG_TYPE;
    EXPECT_FALSE(button.FromJson(jsonComponent, message, isClicked));

    styleJson[JsonTagConstants::JSON_BG_TAG] = SecCompBackground::CIRCLE;
    styleJson[JsonTagConstants::JSON_TEXT_TAG] = UNKNOWN_TEXT;
    EXPECT_FALSE(button.FromJson(jsonComponent, message, isClicked));

    styleJson[JsonTagConstants::JSON_TEXT_TAG] = SaveDesc::MAX_LABEL_TYPE;
    EXPECT_FALSE(button.FromJson(jsonComponent, message, isClicked));

    styleJson[JsonTagConstants::JSON_TEXT_TAG] = SaveDesc::DOWNLOAD;
    styleJson[JsonTagConstants::JSON_ICON_TAG] = UNKNOWN_ICON;
    EXPECT_FALSE(button.FromJson(jsonComponent, message, isClicked));

    styleJson[JsonTagConstants::JSON_ICON_TAG] = SaveIcon::MAX_ICON_TYPE;
    EXPECT_FALSE(button.FromJson(jsonComponent, message, isClicked));

    styleJson[JsonTagConstants::JSON_ICON_TAG] = SaveIcon::LINE_ICON;
    styleJson[JsonTagConstants::JSON_BG_TAG] = SecCompBackground::MAX_BG_TYPE;
    EXPECT_FALSE(button.FromJson(jsonComponent, message, isClicked));
}

/**
 * @tc.name: FromJson014
 * @tc.desc: Test save button from wrong rect json
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaveButtonTest, FromJson014, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    SaveButton comp;
    std::string message;
    bool isClicked = true;
    TestCommon::BuildSaveComponentInfo(jsonComponent);
    EXPECT_TRUE(comp.FromJson(jsonComponent, message, isClicked));

    jsonComponent[JsonTagConstants::JSON_WINDOW_RECT] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));

    TestCommon::BuildSaveComponentInfo(jsonComponent);
    auto& rectJson = jsonComponent[JsonTagConstants::JSON_WINDOW_RECT];
    rectJson[JsonTagConstants::JSON_RECT_X] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));

    rectJson[JsonTagConstants::JSON_RECT_X] = TestCommon::TEST_COORDINATE;
    rectJson[JsonTagConstants::JSON_RECT_Y] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));

    rectJson[JsonTagConstants::JSON_RECT_Y] = TestCommon::TEST_COORDINATE;
    rectJson[JsonTagConstants::JSON_RECT_WIDTH] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));

    rectJson[JsonTagConstants::JSON_RECT_WIDTH] = TestCommon::TEST_COORDINATE;
    rectJson[JsonTagConstants::JSON_RECT_HEIGHT] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));
}

/**
 * @tc.name: FromJson015
 * @tc.desc: Test save button from wrong window id json
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaveButtonTest, FromJson015, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    SaveButton comp;
    std::string message;
    bool isClicked = true;
    TestCommon::BuildSaveComponentInfo(jsonComponent);
    EXPECT_TRUE(comp.FromJson(jsonComponent, message, isClicked));

    jsonComponent[JsonTagConstants::JSON_WINDOW_ID] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));
}

/**
 * @tc.name: FromJson016
 * @tc.desc: Test save button from wrong display id json
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaveButtonTest, FromJson016, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    SaveButton comp;
    std::string message;
    bool isClicked = true;
    TestCommon::BuildSaveComponentInfo(jsonComponent);
    EXPECT_TRUE(comp.FromJson(jsonComponent, message, isClicked));

    jsonComponent[JsonTagConstants::JSON_DISPLAY_ID] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));
}

/**
 * @tc.name: FromJson017
 * @tc.desc: Test save button from wrong cross axis state json
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaveButtonTest, FromJson017, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    SaveButton comp;
    std::string message;
    bool isClicked = true;
    TestCommon::BuildSaveComponentInfo(jsonComponent);
    EXPECT_TRUE(comp.FromJson(jsonComponent, message, isClicked));

    jsonComponent[JsonTagConstants::JSON_CROSS_AXIS_STATE] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));

    jsonComponent[JsonTagConstants::JSON_CROSS_AXIS_STATE] = static_cast<int32_t>(CrossAxisState::STATE_INVALID) - 1;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));

    jsonComponent[JsonTagConstants::JSON_CROSS_AXIS_STATE] = static_cast<int32_t>(CrossAxisState::STATE_NO_CROSS) + 1;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));
}

/**
 * @tc.name: FromJson018
 * @tc.desc: Test save button from wrong customize state json
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaveButtonTest, FromJson018, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    SaveButton comp;
    std::string message;
    bool isClicked = true;
    TestCommon::BuildSaveComponentInfo(jsonComponent);
    EXPECT_TRUE(comp.FromJson(jsonComponent, message, isClicked));

    jsonComponent[JsonTagConstants::JSON_IS_CUSTOMIZABLE] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));
}

/**
 * @tc.name: FromJson019
 * @tc.desc: Test save button from wrong tip position json
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaveButtonTest, FromJson019, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    SaveButton comp;
    std::string message;
    bool isClicked = true;
    TestCommon::BuildSaveComponentInfo(jsonComponent);
    EXPECT_TRUE(comp.FromJson(jsonComponent, message, isClicked));

    jsonComponent[JsonTagConstants::JSON_TIP_POSITION] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));

    jsonComponent[JsonTagConstants::JSON_TIP_POSITION] = static_cast<int32_t>(TipPosition::ABOVE_BOTTOM) - 1;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));

    jsonComponent[JsonTagConstants::JSON_TIP_POSITION] = static_cast<int32_t>(TipPosition::BELOW_TOP) + 1;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));
}

/**
 * @tc.name: FromJson020
 * @tc.desc: Test save button from wrong compatible change json
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaveButtonTest, FromJson020, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    SaveButton comp;
    std::string message;
    bool isClicked = true;
    TestCommon::BuildSaveComponentInfo(jsonComponent);
    EXPECT_TRUE(comp.FromJson(jsonComponent, message, isClicked));

    jsonComponent[JsonTagConstants::JSON_NON_COMPATIBLE_CHANGE_TAG] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));

    jsonComponent[JsonTagConstants::JSON_NON_COMPATIBLE_CHANGE_TAG] = true;
    jsonComponent[JsonTagConstants::JSON_IS_ICON_EXCEEDED_TAG] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));

    jsonComponent[JsonTagConstants::JSON_IS_ICON_EXCEEDED_TAG] = true;
    jsonComponent[JsonTagConstants::JSON_IS_BORDER_COVERED_TAG] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));

    jsonComponent[JsonTagConstants::JSON_IS_BORDER_COVERED_TAG] = true;
    jsonComponent[JsonTagConstants::JSON_LINEAR_GRADIENT_BLUR_RADIUS_TAG] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));

    jsonComponent[JsonTagConstants::JSON_LINEAR_GRADIENT_BLUR_RADIUS_TAG] = TestCommon::TEST_COORDINATE;
    jsonComponent[JsonTagConstants::JSON_FOREGROUND_BLUR_RADIUS_TAG] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));

    jsonComponent[JsonTagConstants::JSON_FOREGROUND_BLUR_RADIUS_TAG] = TestCommon::TEST_COORDINATE;
    jsonComponent[JsonTagConstants::JSON_IS_OVERLAY_TEXT_SET_TAG] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));

    jsonComponent[JsonTagConstants::JSON_IS_OVERLAY_TEXT_SET_TAG] = true;
    jsonComponent[JsonTagConstants::JSON_IS_OVERLAY_NODE_SET_TAG] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));
    
    jsonComponent[JsonTagConstants::JSON_IS_ARKUI_COMPONENT] = WRONG_TYPE;
    EXPECT_FALSE(comp.FromJson(jsonComponent, message, isClicked));
}
