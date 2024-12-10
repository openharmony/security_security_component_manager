/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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
#include "sec_comp_entity_test.h"

#include "sec_comp_log.h"
#include "location_button.h"
#include "paste_button.h"
#include "save_button.h"
#include "sec_comp_err.h"
#include "sec_comp_tool.h"
#include "service_test_common.h"
#include "sec_comp_info_helper_test.h"
#include "window_manager.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Security::SecurityComponent;
using namespace OHOS::Security::AccessToken;

namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
    LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "SecCompEntityTest"};
}

void SecCompEntityTest::SetUpTestCase()
{}

void SecCompEntityTest::TearDownTestCase()
{}

void SecCompEntityTest::SetUp()
{
    SC_LOG_INFO(LABEL, "setup");
    if (entity_ != nullptr) {
        return;
    }

    std::shared_ptr<LocationButton> component = std::make_shared<LocationButton>();
    ASSERT_NE(nullptr, component);

    entity_ = std::make_shared<SecCompEntity>(component, 1, 1, 1, 1);
    ASSERT_NE(nullptr, entity_);

    Rosen::WindowManager::GetInstance().SetDefaultSecCompScene();
}

void SecCompEntityTest::TearDown()
{
    entity_ = nullptr;
}

/**
 * @tc.name: GrantTempPermission001
 * @tc.desc: Test grant location permission
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SecCompEntityTest, GrantTempPermission001, TestSize.Level1)
{
    entity_->isGrant_ = false;
    entity_->componentInfo_->type_ = UNKNOWN_SC_TYPE;
    ASSERT_EQ(SC_SERVICE_ERROR_PERMISSION_OPER_FAIL, entity_->GrantTempPermission());
    ASSERT_TRUE(entity_->isGrant_);

    entity_->isGrant_ = false;
    entity_->componentInfo_->type_ = LOCATION_COMPONENT;
    ASSERT_EQ(SC_OK, entity_->GrantTempPermission());
    ASSERT_TRUE(entity_->isGrant_);

    entity_->isGrant_ = false;
    entity_->componentInfo_->type_ = PASTE_COMPONENT;
    ASSERT_EQ(SC_OK, entity_->GrantTempPermission());
    ASSERT_TRUE(entity_->isGrant_);
}

/**
 * @tc.name: GrantTempPermission002
 * @tc.desc: Test grant paste permission with invalid tokenId.
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SecCompEntityTest, GrantTempPermission002, TestSize.Level1)
{
    std::shared_ptr<PasteButton> pasteComponent = std::make_shared<PasteButton>();
    ASSERT_NE(nullptr, pasteComponent);

    entity_ = std::make_shared<SecCompEntity>(pasteComponent, 0, 1, 1, 1);
    ASSERT_NE(nullptr, entity_);

    ASSERT_EQ(SC_SERVICE_ERROR_PERMISSION_OPER_FAIL, entity_->GrantTempPermission());
}

/**
 * @tc.name: CheckClickInfo001
 * @tc.desc: Test touch info
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SecCompEntityTest, CheckClickInfo001, TestSize.Level1)
{
    SecCompClickEvent touch = {
        .type = ClickEventType::POINT_EVENT_TYPE,
        .point.touchX = ServiceTestCommon::TEST_COORDINATE,
        .point.touchY = ServiceTestCommon::TEST_COORDINATE,
        .point.timestamp = 0,
    };
    ASSERT_NE(entity_->CheckClickInfo(touch), SC_OK);

    uint64_t current = static_cast<uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    touch.point.timestamp = current + 10000L; // 10s
    ASSERT_NE(entity_->CheckClickInfo(touch), SC_OK);

    entity_->componentInfo_->rect_.x_ = ServiceTestCommon::TEST_DIFF_COORDINATE; // click event will not hit this rect
    entity_->componentInfo_->rect_.y_ = ServiceTestCommon::TEST_DIFF_COORDINATE;
    entity_->componentInfo_->rect_.width_ = ServiceTestCommon::TEST_DIFF_COORDINATE;
    entity_->componentInfo_->rect_.height_ = ServiceTestCommon::TEST_DIFF_COORDINATE;
    touch.point.timestamp = static_cast<uint64_t>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count()) / ServiceTestCommon::TIME_CONVERSION_UNIT;
    ASSERT_NE(entity_->CheckClickInfo(touch), SC_OK);

    entity_->componentInfo_->rect_.x_ = ServiceTestCommon::TEST_COORDINATE;
    entity_->componentInfo_->rect_.y_ = ServiceTestCommon::TEST_COORDINATE;
    uint8_t buffer[1] = { 0 };
    touch.extraInfo.dataSize = 1;
    touch.extraInfo.data = buffer;
    touch.point.timestamp = static_cast<uint64_t>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count()) / ServiceTestCommon::TIME_CONVERSION_UNIT;
    ASSERT_EQ(entity_->CheckClickInfo(touch), SC_OK);
}

/**
 * @tc.name: CheckClickInfo002
 * @tc.desc: Test touch info with window check failed
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompEntityTest, CheckClickInfo002, TestSize.Level1)
{
    SecCompClickEvent touch = {
        .type = ClickEventType::POINT_EVENT_TYPE,
        .point.touchX = ServiceTestCommon::TEST_COORDINATE,
        .point.touchY = ServiceTestCommon::TEST_COORDINATE,
        .point.timestamp = static_cast<uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count()),
    };
    ASSERT_NE(entity_->CheckClickInfo(touch), SC_OK);

    entity_->componentInfo_->rect_.x_ = ServiceTestCommon::TEST_COORDINATE;
    entity_->componentInfo_->rect_.y_ = ServiceTestCommon::TEST_COORDINATE;
    entity_->componentInfo_->rect_.width_ = ServiceTestCommon::TEST_COORDINATE;
    entity_->componentInfo_->rect_.height_ = ServiceTestCommon::TEST_COORDINATE;

    // GetAccessibilityWindowInfo failed
    OHOS::Rosen::WindowManager::GetInstance().result_ = static_cast<OHOS::Rosen::WMError>(-1);
    ASSERT_EQ(entity_->CheckClickInfo(touch), SC_SERVICE_ERROR_CLICK_EVENT_INVALID);
}

/**
 * @tc.name: CompareComponentBasicInfo001
 * @tc.desc: Test Basic info
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SecCompEntityTest, CompareComponentBasicInfo001, TestSize.Level1)
{
    nlohmann::json jsonComponent;
    ServiceTestCommon::BuildLocationComponentJson(jsonComponent);
    SecCompBase* other = SecCompInfoHelper::ParseComponent(LOCATION_COMPONENT, jsonComponent);
    bool isRectCheck = true;
    ASSERT_FALSE(entity_->CompareComponentBasicInfo(other, isRectCheck));
}

/**
 * @tc.name: CheckKeyEvent001
 * @tc.desc: Test CheckKeyEvent
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SecCompEntityTest, CheckKeyEvent001, TestSize.Level1)
{
    SecCompClickEvent clickInfo;
    clickInfo.type = ClickEventType::KEY_EVENT_TYPE;
    auto current = static_cast<uint64_t>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count()) / 1000;
    clickInfo.key.timestamp = current - 1000000L - 1;
    ASSERT_EQ(SC_SERVICE_ERROR_CLICK_EVENT_INVALID, entity_->CheckClickInfo(clickInfo));

    clickInfo.key.timestamp = current + 1;
    ASSERT_EQ(SC_SERVICE_ERROR_CLICK_EVENT_INVALID, entity_->CheckClickInfo(clickInfo));

    clickInfo.key.timestamp = current - 1;
    clickInfo.key.keyCode = 1;
    ASSERT_EQ(SC_SERVICE_ERROR_CLICK_EVENT_INVALID, entity_->CheckClickInfo(clickInfo));

    clickInfo.key.keyCode = KEY_SPACE;
    ASSERT_EQ(SC_OK, entity_->CheckKeyEvent(clickInfo));
    clickInfo.key.keyCode = KEY_ENTER;
    ASSERT_EQ(SC_OK, entity_->CheckKeyEvent(clickInfo));
    clickInfo.key.keyCode = KEY_NUMPAD_ENTER;
    ASSERT_EQ(SC_OK, entity_->CheckKeyEvent(clickInfo));
}
