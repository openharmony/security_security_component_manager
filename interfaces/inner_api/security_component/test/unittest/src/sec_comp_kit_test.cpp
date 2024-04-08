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
#include "sec_comp_kit_test.h"

#include "location_button.h"
#define private public
#include "sec_comp_caller_authorization.h"
#include "sec_comp_client.h"
#include "sec_comp_load_callback.h"
#undef private
#include "sec_comp_err.h"
#include "sec_comp_info.h"
#include "sec_comp_log.h"
#include "sec_comp_tool.h"
#include "test_common.h"

using namespace testing::ext;
using namespace OHOS::Security::SecurityComponent;

namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
    LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "SecCompKitTest"};

static void TestInCallerNotCheckList()
{
    int32_t scId = -1;
    struct SecCompClickEvent click;
    std::string emptyStr = "";
    int registerRes = SecCompKit::RegisterSecurityComponent(LOCATION_COMPONENT, emptyStr, scId);
    int updateRes = SecCompKit::UpdateSecurityComponent(scId, emptyStr);
    OnFirstUseDialogCloseFunc func = [] (int32_t) {};
    int reportRes = SecCompKit::ReportSecurityComponentClickEvent(scId, emptyStr, click, nullptr, std::move(func));

    EXPECT_EQ(registerRes, SC_SERVICE_ERROR_CALLER_INVALID);
    EXPECT_EQ(updateRes, SC_SERVICE_ERROR_CALLER_INVALID);
    EXPECT_EQ(reportRes, SC_SERVICE_ERROR_CALLER_INVALID);
}

static void TestInCallerCheckList()
{
    int32_t scId = -1;
    struct SecCompClickEvent click;
    std::string emptyStr = "";
    int registerRes = SecCompKit::RegisterSecurityComponent(LOCATION_COMPONENT, emptyStr, scId);
    int updateRes = SecCompKit::UpdateSecurityComponent(scId, emptyStr);
    OnFirstUseDialogCloseFunc func = [] (int32_t) {};
    int reportRes = SecCompKit::ReportSecurityComponentClickEvent(scId, emptyStr, click, nullptr, std::move(func));

    EXPECT_NE(registerRes, SC_SERVICE_ERROR_CALLER_INVALID);
    EXPECT_NE(updateRes, SC_SERVICE_ERROR_CALLER_INVALID);
    EXPECT_NE(reportRes, SC_SERVICE_ERROR_CALLER_INVALID);
}
}  // namespace

void SecCompKitTest::SetUpTestCase()
{
    SC_LOG_INFO(LABEL, "SetUpTestCase.");
}

void SecCompKitTest::TearDownTestCase()
{
    SC_LOG_INFO(LABEL, "TearDownTestCase.");
}

void SecCompKitTest::SetUp()
{
    SC_LOG_INFO(LABEL, "SetUp ok.");
}

void SecCompKitTest::TearDown()
{
    SC_LOG_INFO(LABEL, "TearDown.");
}

/**
 * @tc.name: ExceptCall001
 * @tc.desc: do kit except call.
 * @tc.type: FUNC
 * @tc.require: AR000HO9IN
 */
HWTEST_F(SecCompKitTest, ExceptCall001, TestSize.Level1)
{
    LocationButton comp;
    comp.fontSize_ = TestCommon::TEST_SIZE;
    comp.iconSize_ = TestCommon::TEST_SIZE;
    comp.padding_.top = TestCommon::TEST_DIMENSION;
    comp.padding_.right = TestCommon::TEST_DIMENSION;
    comp.padding_.bottom = TestCommon::TEST_DIMENSION;
    comp.padding_.left = TestCommon::TEST_DIMENSION;
    comp.textIconSpace_ = TestCommon::TEST_SIZE;
    comp.bgColor_.value = TestCommon::TEST_COLOR;
    comp.fontColor_.value = TestCommon::TEST_COLOR;
    comp.iconColor_.value = TestCommon::TEST_COLOR;
    comp.borderWidth_ = TestCommon::TEST_SIZE;
    comp.parentEffect_ = true;
    comp.type_ = LOCATION_COMPONENT;
    comp.rect_.x_ = TestCommon::TEST_COORDINATE;
    comp.rect_.y_ = TestCommon::TEST_COORDINATE;
    comp.rect_.width_ = TestCommon::TEST_COORDINATE;
    comp.rect_.height_ = TestCommon::TEST_COORDINATE;

    nlohmann::json jsonRes;
    comp.ToJson(jsonRes);
    int32_t scId = -1;
    std::string jsonStr = jsonRes.dump();
    ASSERT_NE(SC_OK, SecCompKit::RegisterSecurityComponent(LOCATION_COMPONENT, jsonStr, scId));
    ASSERT_EQ(-1, scId);
    ASSERT_NE(SC_OK, SecCompKit::UpdateSecurityComponent(scId, jsonStr));

    struct SecCompClickEvent touch = {
        .type = ClickEventType::POINT_EVENT_TYPE,
        .point.touchX = TestCommon::TEST_COORDINATE,
        .point.touchY = TestCommon::TEST_COORDINATE,
        .point.timestamp = static_cast<uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count())
    };
    OnFirstUseDialogCloseFunc func = nullptr;
    EXPECT_NE(SC_OK, SecCompKit::ReportSecurityComponentClickEvent(scId, jsonStr, touch, nullptr, std::move(func)));
    EXPECT_NE(SC_OK, SecCompKit::UnregisterSecurityComponent(scId));
}

/**
 * @tc.name: ExceptCall001
 * @tc.desc: test caller check.
 * @tc.type: FUNC
 * @tc.require: AR000HO9JS
 */
HWTEST_F(SecCompKitTest, TestCallerCheck001, TestSize.Level1)
{
    std::vector<uintptr_t> callerList = {
        reinterpret_cast<uintptr_t>(TestInCallerCheckList),
    };
    SecCompUiRegister registerCallback(callerList, nullptr);
    TestInCallerCheckList();
    TestInCallerNotCheckList();

    // prohibit init caller list repeately
    std::vector<uintptr_t> callerList1 = {
        reinterpret_cast<uintptr_t>(TestInCallerNotCheckList),
    };
    SecCompUiRegister registerCallback1(callerList1, nullptr);
    TestInCallerNotCheckList();
    SecCompCallerAuthorization::GetInstance().kitCallerList_.clear();
    SecCompCallerAuthorization::GetInstance().isInit_ = false;
}

/**
 * @tc.name: ExceptCall002
 * @tc.desc: test invalid caller register.
 * @tc.type: FUNC
 * @tc.require: AR000HO9JS
 */
HWTEST_F(SecCompKitTest, TestCallerCheck002, TestSize.Level1)
{
    std::vector<uintptr_t> callerList;
    SecCompUiRegister registerCallback(callerList, nullptr);
    TestInCallerNotCheckList();
    SecCompCallerAuthorization::GetInstance().kitCallerList_.clear();
    SecCompCallerAuthorization::GetInstance().isInit_ = false;

    for (size_t i = 0; i < TestCommon::MAX_CALLER_SIZE + 1; i++) {
        callerList.emplace_back(reinterpret_cast<uintptr_t>(TestInCallerNotCheckList));
    }
    SecCompUiRegister registerCallback2(callerList, nullptr);
    TestInCallerNotCheckList();
    SecCompCallerAuthorization::GetInstance().kitCallerList_.clear();
    SecCompCallerAuthorization::GetInstance().isInit_ = false;
}

/**
 * @tc.name: RegisterWithoutCallback001
 * @tc.desc: test register without callback.
 * @tc.type: FUNC
 * @tc.require: AR000HO9JM
 */
HWTEST_F(SecCompKitTest, RegisterWithoutCallback001, TestSize.Level1)
{
    nlohmann::json jsonRes;
    TestCommon::BuildLocationComponentInfo(jsonRes);
    std::string locationInfo = jsonRes.dump();

    int32_t scId;
#ifdef SECURITY_COMPONENT_ENHANCE_ENABLE
    ASSERT_EQ(SC_ENHANCE_ERROR_VALUE_INVALID,
        SecCompKit::RegisterSecurityComponent(LOCATION_COMPONENT, locationInfo, scId));
    ASSERT_EQ(0, scId);
#else
    ASSERT_EQ(SC_OK,
        SecCompKit::RegisterSecurityComponent(LOCATION_COMPONENT, locationInfo, scId));
    ASSERT_NE(-1, scId);
    EXPECT_EQ(SC_OK, SecCompKit::UnregisterSecurityComponent(scId));
#endif
}

/**
 * @tc.name: FinishStartSAFail001
 * @tc.desc: Test update security component caller error
 * @tc.type: FUNC
 * @tc.require: AR000HO9IN
 */
HWTEST_F(SecCompKitTest, FinishStartSAFail001, TestSize.Level1)
{
    SecCompClient::GetInstance().FinishStartSAFail();
    EXPECT_TRUE(SecCompClient::GetInstance().readyFlag_);
    SecCompClient::GetInstance().OnRemoteDiedHandle();
    EXPECT_EQ(nullptr, SecCompClient::GetInstance().proxy_);
    SecCompClient::GetInstance().GetProxyFromRemoteObject(nullptr);
}

/**
 * @tc.name: OnLoadSystemAbilitySuccess001
 * @tc.desc: Test update security component caller error
 * @tc.type: FUNC
 * @tc.require: AR000HO9IN
 */
HWTEST_F(SecCompKitTest, OnLoadSystemAbilitySuccess001, TestSize.Level1)
{
    std::shared_ptr<SecCompLoadCallback> loadCallback = std::make_shared<SecCompLoadCallback>();
    EXPECT_NE(nullptr, loadCallback);
    int32_t systemAbilityId = SC_OK;
    loadCallback->OnLoadSystemAbilitySuccess(systemAbilityId, nullptr);
    loadCallback->OnLoadSystemAbilityFail(systemAbilityId);
    systemAbilityId = SA_ID_SECURITY_COMPONENT_SERVICE;
    loadCallback->OnLoadSystemAbilitySuccess(systemAbilityId, nullptr);
    loadCallback->OnLoadSystemAbilityFail(systemAbilityId);
}
