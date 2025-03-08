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
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SaveButtonTest, IsParamValid001, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    TestCommon::BuildSaveComponentInfo(jsonComponent);
    SaveButton button;

    ASSERT_TRUE(button.FromJson(jsonComponent));

    auto& stylesJson = jsonComponent[JsonTagConstants::JSON_STYLE_TAG];
    stylesJson[JsonTagConstants::JSON_TEXT_TAG] = UNKNOWN_TEXT;
    ASSERT_FALSE(button.FromJson(jsonComponent));

    stylesJson[JsonTagConstants::JSON_TEXT_TAG] = SaveDesc::DOWNLOAD;
    stylesJson[JsonTagConstants::JSON_ICON_TAG] = UNKNOWN_ICON;
    ASSERT_FALSE(button.FromJson(jsonComponent));

    stylesJson[JsonTagConstants::JSON_ICON_TAG] = SaveIcon::LINE_ICON;
    stylesJson[JsonTagConstants::JSON_BG_TAG] = SecCompBackground::UNKNOWN_BG;
    ASSERT_FALSE(button.FromJson(jsonComponent));

    stylesJson[JsonTagConstants::JSON_BG_TAG] = SecCompBackground::CIRCLE;
    stylesJson[JsonTagConstants::JSON_TEXT_TAG] = SaveDesc::MAX_LABEL_TYPE;
    ASSERT_FALSE(button.FromJson(jsonComponent));

    stylesJson[JsonTagConstants::JSON_TEXT_TAG] = SaveDesc::DOWNLOAD;
    stylesJson[JsonTagConstants::JSON_ICON_TAG] = SaveIcon::MAX_ICON_TYPE;
    ASSERT_FALSE(button.FromJson(jsonComponent));

    stylesJson[JsonTagConstants::JSON_ICON_TAG] = SaveIcon::LINE_ICON;
    stylesJson[JsonTagConstants::JSON_BG_TAG] = SecCompBackground::MAX_BG_TYPE;
    ASSERT_FALSE(button.FromJson(jsonComponent));
}

/**
 * @tc.name: CompareSaveButton001
 * @tc.desc: Test compare save button
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SaveButtonTest, CompareSaveButton001, TestSize.Level1)
{
    SaveButton button1;
    SaveButton button2;

    nlohmann::json jsonComponent;
    TestCommon::BuildSaveComponentInfo(jsonComponent);

    ASSERT_TRUE(button1.FromJson(jsonComponent));
    ASSERT_TRUE(button2.FromJson(jsonComponent));
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
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SaveButtonTest, CompareSaveButton002, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    SaveButton comp1;
    TestCommon::BuildSaveComponentInfo(jsonComponent);
    ASSERT_TRUE(comp1.FromJson(jsonComponent));
    SaveButton comp2 = comp1;

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
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SaveButtonTest, CompareSaveButton003, TestSize.Level1)
{
    SaveButton button1;
    PasteButton button2;
    nlohmann::json jsonComponent;
    TestCommon::BuildSaveComponentInfo(jsonComponent);

    ASSERT_TRUE(button1.FromJson(jsonComponent));
    ASSERT_FALSE(button1.CompareComponentBasicInfo(&button2, true));
}
