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
#include "paste_button_test.h"

#include <string>
#include "sec_comp_err.h"
#include "sec_comp_log.h"
#include "test_common.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Security::SecurityComponent;

namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
    LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "PasteButtonTest"};
}

void PasteButtonTest::SetUpTestCase()
{}

void PasteButtonTest::TearDownTestCase()
{}

void PasteButtonTest::SetUp()
{
    SC_LOG_INFO(LABEL, "setup");
}

void PasteButtonTest::TearDown()
{}

/**
 * @tc.name: IsParamValid001
 * @tc.desc: Test paste button from wrong value params json
 * @tc.type: FUNC
 * @tc.require: AR000HO9JB
 */
HWTEST_F(PasteButtonTest, IsParamValid001, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    TestCommon::BuildPasteComponentInfo(jsonComponent);
    PasteButton button;

    ASSERT_TRUE(button.FromJson(jsonComponent));

    auto& styleJson = jsonComponent[JsonTagConstants::JSON_STYLE_TAG];
    styleJson[JsonTagConstants::JSON_TEXT_TAG] = UNKNOWN_TEXT;
    ASSERT_FALSE(button.FromJson(jsonComponent));

    styleJson[JsonTagConstants::JSON_TEXT_TAG] = PasteDesc::PASTE;
    styleJson[JsonTagConstants::JSON_ICON_TAG] = UNKNOWN_ICON;
    ASSERT_FALSE(button.FromJson(jsonComponent));

    styleJson[JsonTagConstants::JSON_ICON_TAG] = PasteIcon::LINE_ICON;
    styleJson[JsonTagConstants::JSON_BG_TAG] = SecCompBackground::UNKNOWN_BG;
    ASSERT_FALSE(button.FromJson(jsonComponent));

    styleJson[JsonTagConstants::JSON_BG_TAG] = SecCompBackground::CIRCLE;
    styleJson[JsonTagConstants::JSON_TEXT_TAG] = PasteDesc::MAX_LABEL_TYPE;
    ASSERT_FALSE(button.FromJson(jsonComponent));

    styleJson[JsonTagConstants::JSON_TEXT_TAG] = PasteDesc::PASTE;
    styleJson[JsonTagConstants::JSON_ICON_TAG] = PasteIcon::MAX_ICON_TYPE;
    ASSERT_FALSE(button.FromJson(jsonComponent));

    styleJson[JsonTagConstants::JSON_ICON_TAG] = PasteIcon::LINE_ICON;
    styleJson[JsonTagConstants::JSON_BG_TAG] = SecCompBackground::MAX_BG_TYPE;
    ASSERT_FALSE(button.FromJson(jsonComponent));
}

/**
 * @tc.name: ComparePasteButton001
 * @tc.desc: Test compare paste button
 * @tc.type: FUNC
 * @tc.require: AR000HO9JB
 */
HWTEST_F(PasteButtonTest, ComparePasteButton001, TestSize.Level1)
{
    PasteButton button1;
    PasteButton button2;

    nlohmann::json jsonComponent;
    TestCommon::BuildPasteComponentInfo(jsonComponent);

    ASSERT_TRUE(button1.FromJson(jsonComponent));
    ASSERT_TRUE(button2.FromJson(jsonComponent));
    ASSERT_TRUE(button1.CompareComponentBasicInfo(&button2, true));

    button1.text_ = UNKNOWN_TEXT;
    ASSERT_FALSE(button1.CompareComponentBasicInfo(&button2, true));
    button1.text_ = static_cast<int32_t>(PasteDesc::PASTE);

    button1.icon_ = UNKNOWN_ICON;
    ASSERT_FALSE(button1.CompareComponentBasicInfo(&button2, true));
    button1.icon_ = static_cast<int32_t>(PasteIcon::LINE_ICON);

    button1.bg_ = SecCompBackground::UNKNOWN_BG;
    ASSERT_FALSE(button1.CompareComponentBasicInfo(&button2, true));
    button1.bg_ = SecCompBackground::CIRCLE;

    ASSERT_TRUE(button1.CompareComponentBasicInfo(&button2, true));
}

/**
 * @tc.name: ComparePasteButton002
 * @tc.desc: Test PasteButton compare
 * @tc.type: FUNC
 * @tc.require: AR000HO9JB
 */
HWTEST_F(PasteButtonTest, ComparePasteButton002, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    PasteButton comp1;
    TestCommon::BuildPasteComponentInfo(jsonComponent);
    ASSERT_TRUE(comp1.FromJson(jsonComponent));
    PasteButton comp2 = comp1;

    comp1.type_ = SAVE_COMPONENT;
    ASSERT_FALSE(comp1.CompareComponentBasicInfo(&comp2, true));
    comp1.type_ = PASTE_COMPONENT;

    comp1.icon_ = static_cast<int32_t>(PasteIcon::FILLED_ICON);
    ASSERT_FALSE(comp1.CompareComponentBasicInfo(&comp2, true));
    comp1.icon_ = static_cast<int32_t>(PasteIcon::LINE_ICON);

    comp1.text_ = static_cast<int32_t>(PasteDesc::MAX_LABEL_TYPE);
    ASSERT_FALSE(comp1.CompareComponentBasicInfo(&comp2, true));
    comp1.text_ = static_cast<int32_t>(PasteDesc::PASTE);

    comp1.bg_ = SecCompBackground::NORMAL;
    ASSERT_FALSE(comp1.CompareComponentBasicInfo(&comp2, true));
    comp1.bg_ = SecCompBackground::CIRCLE;

    ASSERT_TRUE(comp1.CompareComponentBasicInfo(&comp2, true));
}

/**
 * @tc.name: ComparePasteButton003
 * @tc.desc: Test PasteButton compare
 * @tc.type: FUNC
 * @tc.require: AR000HO9JB
 */
HWTEST_F(PasteButtonTest, ComparePasteButton003, TestSize.Level1)
{
    PasteButton button1;
    SaveButton button2;
    nlohmann::json jsonComponent;
    TestCommon::BuildPasteComponentInfo(jsonComponent);

    ASSERT_TRUE(button1.FromJson(jsonComponent));
    ASSERT_FALSE(button1.CompareComponentBasicInfo(&button2, true));
}
