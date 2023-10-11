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
#include "sec_comp_service_mock_test.h"

#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "location_button.h"
#include "mock_system_ability_proxy.h"
#include "mock_app_mgr_proxy.h"
#include "paste_button.h"
#include "save_button.h"
#include "sec_comp_err.h"
#include "sec_comp_log.h"
#include "sec_comp_tool.h"
#include "sec_comp_enhance_adapter.h"
#include "service_test_common.h"
#include "system_ability.h"
#include "token_setproc.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Security::SecurityComponent;
using namespace OHOS::Security::AccessToken;

namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
    LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "SecCompServiceMockTest"};
static AccessTokenID g_selfTokenId = 0;
}

void SecCompServiceMockTest::SetUpTestCase()
{}

void SecCompServiceMockTest::TearDownTestCase()
{}

void SecCompServiceMockTest::SetUp()
{
    SC_LOG_INFO(LABEL, "setup");
    if (secCompService_ != nullptr) {
        return;
    }
    SecCompService* ptr = new (std::nothrow) SecCompService(ServiceTestCommon::SA_ID, true);
    secCompService_ = sptr<SecCompService>(ptr);
    ASSERT_NE(nullptr, secCompService_);
    secCompService_->appStateObserver_ = new (std::nothrow) AppStateObserver();
    ASSERT_NE(nullptr, secCompService_->appStateObserver_);
    g_selfTokenId = GetSelfTokenID();
}

void SecCompServiceMockTest::TearDown()
{
    if (secCompService_ != nullptr) {
        secCompService_->appStateObserver_ = nullptr;
    }
    secCompService_ = nullptr;
    EXPECT_EQ(0, SetSelfTokenID(g_selfTokenId));
}

/**
 * @tc.name: RegisterSecurityComponent001
 * @tc.desc: Test register security component
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SecCompServiceMockTest, RegisterSecurityComponent001, TestSize.Level1)
{
    // get caller fail
    int32_t scId;
    secCompService_->state_ = ServiceRunningState::STATE_RUNNING;
    secCompService_->Initialize();
    EXPECT_EQ(SC_SERVICE_ERROR_VALUE_INVALID, secCompService_->RegisterSecurityComponent(SAVE_COMPONENT, "", scId));
    nlohmann::json jsonRes;
    ServiceTestCommon::BuildSaveComponentJson(jsonRes);
    std::string saveInfo = jsonRes.dump();

    // parse component json fail
    ASSERT_EQ(0, SetSelfTokenID(ServiceTestCommon::HAP_TOKEN_ID));
    AppExecFwk::AppStateData stateData = {
        .uid = getuid()
    };
    secCompService_->appStateObserver_->AddProcessToForegroundSet(stateData);
    // wrong json
    EXPECT_EQ(SC_SERVICE_ERROR_VALUE_INVALID, secCompService_->RegisterSecurityComponent(SAVE_COMPONENT, "{a=", scId));

    // register security component ok
    EXPECT_EQ(SC_OK, secCompService_->RegisterSecurityComponent(SAVE_COMPONENT, saveInfo, scId));
    EXPECT_EQ(SC_OK, secCompService_->UpdateSecurityComponent(scId, saveInfo));
    struct SecCompClickEvent touch = {
        .touchX = 100,
        .touchY = 100,
        .timestamp = static_cast<uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count()) /
            ServiceTestCommon::TIME_CONVERSION_UNIT
    };

    EXPECT_EQ(SC_OK, secCompService_->ReportSecurityComponentClickEvent(scId, saveInfo, touch, nullptr));
    sleep(5);
    EXPECT_EQ(SC_OK, secCompService_->UnregisterSecurityComponent(scId));
}

/**
 * @tc.name: RegisterSecurityComponent002
 * @tc.desc: Test register security component check touch info failed
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SecCompServiceMockTest, RegisterSecurityComponent002, TestSize.Level1)
{
    int32_t scId;
    secCompService_->state_ = ServiceRunningState::STATE_RUNNING;
    secCompService_->Initialize();
    nlohmann::json jsonRes;
    ServiceTestCommon::BuildSaveComponentJson(jsonRes);
    std::string saveInfo = jsonRes.dump();

    ASSERT_EQ(0, SetSelfTokenID(ServiceTestCommon::HAP_TOKEN_ID));
    AppExecFwk::AppStateData stateData = {
        .uid = getuid()
    };
    secCompService_->appStateObserver_->AddProcessToForegroundSet(stateData);
    // register security component ok
    EXPECT_EQ(SC_OK, secCompService_->RegisterSecurityComponent(SAVE_COMPONENT, saveInfo, scId));
    struct SecCompClickEvent touch = {
        .touchX = 100,
        .touchY = 100,
        .timestamp = static_cast<uint64_t>(
            std::chrono::high_resolution_clock::now().time_since_epoch().count())
    };
    EXPECT_EQ(SC_SERVICE_ERROR_CLICK_EVENT_INVALID,
        secCompService_->ReportSecurityComponentClickEvent(scId, saveInfo, touch, nullptr));
    EXPECT_EQ(SC_OK, secCompService_->UnregisterSecurityComponent(scId));
}

/**
 * @tc.name: RegisterSecurityComponent003
 * @tc.desc: Test register security component permission grant failed
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SecCompServiceMockTest, RegisterSecurityComponent003, TestSize.Level1)
{
    int32_t scId;
    secCompService_->state_ = ServiceRunningState::STATE_RUNNING;
    secCompService_->Initialize();
    nlohmann::json jsonRes;
    ServiceTestCommon::BuildSaveComponentJson(jsonRes);
    std::string saveInfo = jsonRes.dump();

    ASSERT_EQ(0, SetSelfTokenID(0));
    AppExecFwk::AppStateData stateData = {
        .uid = getuid()
    };
    secCompService_->appStateObserver_->AddProcessToForegroundSet(stateData);
    // register security component ok
    EXPECT_EQ(SC_OK, secCompService_->RegisterSecurityComponent(SAVE_COMPONENT, saveInfo, scId));
    struct SecCompClickEvent touch = {
        .touchX = 100,
        .touchY = 100,
        .timestamp = static_cast<uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count()) /
            ServiceTestCommon::TIME_CONVERSION_UNIT
    };
    EXPECT_EQ(SC_SERVICE_ERROR_PERMISSION_OPER_FAIL,
        secCompService_->ReportSecurityComponentClickEvent(scId, saveInfo, touch, nullptr));
    EXPECT_EQ(SC_OK, secCompService_->UnregisterSecurityComponent(scId));
}

/**
 * @tc.name: ReportSecurityComponentClickEvent001
 * @tc.desc: Test register security component
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SecCompServiceMockTest, ReportSecurityComponentClickEvent001, TestSize.Level1)
{
    int32_t scId;
    secCompService_->state_ = ServiceRunningState::STATE_RUNNING;
    secCompService_->Initialize();
    nlohmann::json jsonRes;
    ServiceTestCommon::BuildSaveComponentJson(jsonRes);
    std::string saveInfo = jsonRes.dump();

    ASSERT_EQ(0, SetSelfTokenID(ServiceTestCommon::HAP_TOKEN_ID));
    AppExecFwk::AppStateData stateData = {
        .uid = getuid()
    };
    secCompService_->appStateObserver_->AddProcessToForegroundSet(stateData);
    // register security component ok
    EXPECT_EQ(SC_OK, secCompService_->RegisterSecurityComponent(SAVE_COMPONENT, saveInfo, scId));
    struct SecCompClickEvent touchInfo = {
        .touchX = 100,
        .touchY = 100,
        .timestamp = static_cast<uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count()) /
            ServiceTestCommon::TIME_CONVERSION_UNIT
    };

    ASSERT_EQ(SC_OK, secCompService_->ReportSecurityComponentClickEvent(scId, saveInfo, touchInfo, nullptr));

    ASSERT_TRUE(secCompService_->ReduceAfterVerifySavePermission(ServiceTestCommon::HAP_TOKEN_ID));

    ASSERT_FALSE(secCompService_->ReduceAfterVerifySavePermission(ServiceTestCommon::HAP_TOKEN_ID));

    ASSERT_EQ(SC_OK, secCompService_->ReportSecurityComponentClickEvent(scId, saveInfo, touchInfo, nullptr));
    sleep(6);
    ASSERT_FALSE(secCompService_->ReduceAfterVerifySavePermission(ServiceTestCommon::HAP_TOKEN_ID));
    EXPECT_EQ(SC_OK, secCompService_->UnregisterSecurityComponent(scId));
}

/**
 * @tc.name: ReportSecurityComponentClickEvent002
 * @tc.desc: Test report security component click with save button
 * @tc.type: FUNC
 * @tc.require: AR000HO9IN
 */
HWTEST_F(SecCompServiceMockTest, ReportSecurityComponentClickEvent002, TestSize.Level1)
{
    int32_t scId;
    secCompService_->state_ = ServiceRunningState::STATE_RUNNING;
    secCompService_->Initialize();
    nlohmann::json jsonRes;
    ServiceTestCommon::BuildSaveComponentJson(jsonRes);
    std::string saveInfo = jsonRes.dump();

    ASSERT_EQ(0, SetSelfTokenID(ServiceTestCommon::HAP_TOKEN_ID));
    AppExecFwk::AppStateData stateData = {
        .uid = getuid()
    };
    secCompService_->appStateObserver_->AddProcessToForegroundSet(stateData);
    // register security component ok
    EXPECT_EQ(SC_OK, secCompService_->RegisterSecurityComponent(SAVE_COMPONENT, saveInfo, scId));
    struct SecCompClickEvent touchInfo = {
        .touchX = 100,
        .touchY = 100,
        .timestamp = static_cast<uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count()) /
            ServiceTestCommon::TIME_CONVERSION_UNIT
    };

    ASSERT_EQ(SC_OK, secCompService_->ReportSecurityComponentClickEvent(scId, saveInfo, touchInfo, nullptr));
    ASSERT_EQ(SC_OK, secCompService_->ReportSecurityComponentClickEvent(scId, saveInfo, touchInfo, nullptr));

    ASSERT_TRUE(secCompService_->ReduceAfterVerifySavePermission(ServiceTestCommon::HAP_TOKEN_ID));
    ASSERT_TRUE(secCompService_->ReduceAfterVerifySavePermission(ServiceTestCommon::HAP_TOKEN_ID));
    ASSERT_FALSE(secCompService_->ReduceAfterVerifySavePermission(ServiceTestCommon::HAP_TOKEN_ID));

    ASSERT_EQ(SC_OK, secCompService_->ReportSecurityComponentClickEvent(scId, saveInfo, touchInfo, nullptr));
    sleep(3);
    touchInfo.timestamp = static_cast<uint64_t>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count()) / ServiceTestCommon::TIME_CONVERSION_UNIT;
    ASSERT_EQ(SC_OK, secCompService_->ReportSecurityComponentClickEvent(scId, saveInfo, touchInfo, nullptr));
    sleep(3);
    ASSERT_TRUE(secCompService_->ReduceAfterVerifySavePermission(ServiceTestCommon::HAP_TOKEN_ID));
    ASSERT_FALSE(secCompService_->ReduceAfterVerifySavePermission(ServiceTestCommon::HAP_TOKEN_ID));

    touchInfo.timestamp = static_cast<uint64_t>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count()) / ServiceTestCommon::TIME_CONVERSION_UNIT;
    ASSERT_EQ(SC_OK, secCompService_->ReportSecurityComponentClickEvent(scId, saveInfo, touchInfo, nullptr));
    sleep(3);
    touchInfo.timestamp = static_cast<uint64_t>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count()) / ServiceTestCommon::TIME_CONVERSION_UNIT;
    ASSERT_EQ(SC_OK, secCompService_->ReportSecurityComponentClickEvent(scId, saveInfo, touchInfo, nullptr));
    ASSERT_TRUE(secCompService_->ReduceAfterVerifySavePermission(ServiceTestCommon::HAP_TOKEN_ID));
    sleep(3);
    ASSERT_TRUE(secCompService_->ReduceAfterVerifySavePermission(ServiceTestCommon::HAP_TOKEN_ID));
    ASSERT_FALSE(secCompService_->ReduceAfterVerifySavePermission(ServiceTestCommon::HAP_TOKEN_ID));
}

/**
 * @tc.name: ReportSecurityComponentClickEvent003
 * @tc.desc: Test report security component click twice with save button
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SecCompServiceMockTest, ReportSecurityComponentClickEvent003, TestSize.Level1)
{
    int32_t scId;
    secCompService_->state_ = ServiceRunningState::STATE_RUNNING;
    secCompService_->Initialize();
    nlohmann::json jsonRes;
    ServiceTestCommon::BuildSaveComponentJson(jsonRes);
    std::string saveInfo = jsonRes.dump();

    ASSERT_EQ(0, SetSelfTokenID(ServiceTestCommon::HAP_TOKEN_ID));
    AppExecFwk::AppStateData stateData = {
        .uid = getuid()
    };
    secCompService_->appStateObserver_->AddProcessToForegroundSet(stateData);
    // register security component ok
    EXPECT_EQ(SC_OK, secCompService_->RegisterSecurityComponent(SAVE_COMPONENT, saveInfo, scId));
    struct SecCompClickEvent touchInfo = {
        .touchX = 100,
        .touchY = 100,
        .timestamp = static_cast<uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count()) /
            ServiceTestCommon::TIME_CONVERSION_UNIT
    };

    ASSERT_EQ(SC_OK, secCompService_->ReportSecurityComponentClickEvent(scId, saveInfo, touchInfo, nullptr));
    sleep(3);
    touchInfo.timestamp = static_cast<uint64_t>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count()) / ServiceTestCommon::TIME_CONVERSION_UNIT;
    ASSERT_EQ(SC_OK, secCompService_->ReportSecurityComponentClickEvent(scId, saveInfo, touchInfo, nullptr));
    sleep(6);
    ASSERT_FALSE(secCompService_->ReduceAfterVerifySavePermission(ServiceTestCommon::HAP_TOKEN_ID));

    touchInfo.timestamp = static_cast<uint64_t>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count()) / ServiceTestCommon::TIME_CONVERSION_UNIT;
    ASSERT_EQ(SC_OK, secCompService_->ReportSecurityComponentClickEvent(scId, saveInfo, touchInfo, nullptr));
    ASSERT_TRUE(secCompService_->ReduceAfterVerifySavePermission(ServiceTestCommon::HAP_TOKEN_ID));
    ASSERT_EQ(SC_OK, secCompService_->ReportSecurityComponentClickEvent(scId, saveInfo, touchInfo, nullptr));
    sleep(3);
    ASSERT_TRUE(secCompService_->ReduceAfterVerifySavePermission(ServiceTestCommon::HAP_TOKEN_ID));
    ASSERT_FALSE(secCompService_->ReduceAfterVerifySavePermission(ServiceTestCommon::HAP_TOKEN_ID));
}
