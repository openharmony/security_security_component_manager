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
 * @tc.name: ToJsonStr001
 * @tc.desc: Test ToJsonStr normal branch
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LocationButtonTest, ToJsonStr001, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    LocationButton button;
    std::string message;
    bool isClicked = true;

    TestCommon::BuildLocationComponentInfo(jsonComponent);
    EXPECT_TRUE(button.FromJson(jsonComponent, message, isClicked));
    EXPECT_EQ(jsonComponent.dump(), button.ToJsonStr());
}

/**
 * @tc.name: CompareComponentBasicInfo001
 * @tc.desc: Test CompareComponentBasicInfo other is null
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LocationButtonTest, CompareComponentBasicInfo001, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    TestCommon::BuildLocationComponentInfo(jsonComponent);
    LocationButton button;

    EXPECT_FALSE(button.CompareComponentBasicInfo(nullptr, true));
}

/**
 * @tc.name: CompareLocationButton001
 * @tc.desc: Test LocationButton compare
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LocationButtonTest, CompareLocationButton001, TestSize.Level1)
{
    LocationButton button1;
    LocationButton button2;

    nlohmann::json jsonComponent;
    std::string message;
    bool isClicked = true;
    TestCommon::BuildLocationComponentInfo(jsonComponent);

    EXPECT_TRUE(button1.FromJson(jsonComponent, message, isClicked));
    EXPECT_TRUE(button2.FromJson(jsonComponent, message, isClicked));
    EXPECT_TRUE(button1.CompareComponentBasicInfo(&button2, true));

    button1.text_ = UNKNOWN_TEXT;
    EXPECT_FALSE(button1.CompareComponentBasicInfo(&button2, true));
    button1.text_ = static_cast<int32_t>(LocationDesc::SELECT_LOCATION);

    button1.icon_ = UNKNOWN_ICON;
    EXPECT_FALSE(button1.CompareComponentBasicInfo(&button2, true));
    button1.icon_ = static_cast<int32_t>(LocationIcon::LINE_ICON);

    button1.bg_ = SecCompBackground::UNKNOWN_BG;
    EXPECT_FALSE(button1.CompareComponentBasicInfo(&button2, true));
    button1.bg_ = SecCompBackground::CIRCLE;

    EXPECT_TRUE(button1.CompareComponentBasicInfo(&button2, true));
}

/**
 * @tc.name: CompareLocationButton002
 * @tc.desc: Test LocationButton compare
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LocationButtonTest, CompareLocationButton002, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    LocationButton comp1;
    std::string message;
    bool isClicked = true;
    TestCommon::BuildLocationComponentInfo(jsonComponent);
    EXPECT_TRUE(comp1.FromJson(jsonComponent, message, isClicked));
    LocationButton comp2 = comp1;
    comp2.isClickEvent_ = true;

    comp1.type_ = SAVE_COMPONENT;
    EXPECT_FALSE(comp1.CompareComponentBasicInfo(&comp2, true));
    comp1.type_ = LOCATION_COMPONENT;

    comp1.fontSize_ = DEFAULT_DIMENSION;
    EXPECT_FALSE(comp1.CompareComponentBasicInfo(&comp2, true));
    comp1.fontSize_ = TestCommon::TEST_SIZE;

    comp1.iconSize_ = DEFAULT_DIMENSION;
    EXPECT_FALSE(comp1.CompareComponentBasicInfo(&comp2, true));
    comp1.iconSize_ = TestCommon::TEST_SIZE;

    comp1.padding_.top = DEFAULT_DIMENSION;
    EXPECT_FALSE(comp1.CompareComponentBasicInfo(&comp2, true));
    comp1.padding_.top = TestCommon::TEST_DIMENSION;

    comp1.padding_.right = DEFAULT_DIMENSION;
    EXPECT_FALSE(comp1.CompareComponentBasicInfo(&comp2, true));
    comp1.padding_.right = TestCommon::TEST_DIMENSION;

    comp1.padding_.bottom = DEFAULT_DIMENSION;
    EXPECT_FALSE(comp1.CompareComponentBasicInfo(&comp2, true));
    comp1.padding_.bottom = TestCommon::TEST_DIMENSION;

    comp1.padding_.left = DEFAULT_DIMENSION;
    EXPECT_FALSE(comp1.CompareComponentBasicInfo(&comp2, true));
    comp1.padding_.left = TestCommon::TEST_DIMENSION;

    comp1.textIconSpace_ = DEFAULT_DIMENSION;
    EXPECT_FALSE(comp1.CompareComponentBasicInfo(&comp2, true));
    comp1.textIconSpace_ = TestCommon::TEST_SIZE;

    comp1.borderWidth_ = DEFAULT_DIMENSION;
    EXPECT_FALSE(comp1.CompareComponentBasicInfo(&comp2, true));
    comp1.borderWidth_ = TestCommon::TEST_SIZE;

    comp1.fontColor_.value = TestCommon::TEST_DIFF_COLOR;
    EXPECT_FALSE(comp1.CompareComponentBasicInfo(&comp2, true));
    comp1.fontColor_.value = TestCommon::TEST_COLOR_RED;

    comp1.bgColor_.value = TestCommon::TEST_DIFF_COLOR;
    EXPECT_FALSE(comp1.CompareComponentBasicInfo(&comp2, true));
    comp1.bgColor_.value = TestCommon::TEST_COLOR_YELLOW;

    comp1.iconColor_.value = TestCommon::TEST_DIFF_COLOR;
    EXPECT_FALSE(comp1.CompareComponentBasicInfo(&comp2, true));
    comp1.iconColor_.value = TestCommon::TEST_COLOR_BLUE;

    EXPECT_TRUE(comp1.CompareComponentBasicInfo(&comp2, true));
}

/**
 * @tc.name: CompareLocationButton003
 * @tc.desc: Test LocationButton compare
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LocationButtonTest, CompareLocationButton003, TestSize.Level1)
{
    LocationButton button1;
    SaveButton button2;
    nlohmann::json jsonComponent;
    std::string message;
    bool isClicked = true;
    TestCommon::BuildLocationComponentInfo(jsonComponent);

    EXPECT_TRUE(button1.FromJson(jsonComponent, message, isClicked));
    EXPECT_FALSE(button1.CompareComponentBasicInfo(&button2, false));
}

/**
 * @tc.name: IsCorrespondenceType001
 * @tc.desc: Test IsCorrespondenceType
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LocationButtonTest, IsCorrespondenceType001, TestSize.Level1)
{
    LocationButton button;
    button.type_ = LOCATION_COMPONENT;
    EXPECT_TRUE(button.IsCorrespondenceType());
    button.type_ = SAVE_COMPONENT;
    EXPECT_FALSE(button.IsCorrespondenceType());
}
