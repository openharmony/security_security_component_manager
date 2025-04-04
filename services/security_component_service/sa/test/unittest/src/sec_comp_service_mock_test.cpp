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

#include "accesstoken_kit.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "location_button.h"
#include "paste_button.h"
#include "save_button.h"
#include "sec_comp_client.h"
#include "sec_comp_err.h"
#include "sec_comp_log.h"
#include "sec_comp_tool.h"
#include "sec_comp_enhance_adapter.h"
#include "service_test_common.h"
#include "system_ability.h"
#include "token_setproc.h"
#include "window_manager.h"

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

    Rosen::WindowManager::GetInstance().SetDefaultSecCompScene();
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
 * @tc.require:
 */
HWTEST_F(SecCompServiceMockTest, RegisterSecurityComponent001, TestSize.Level1)
{
    // get caller fail
    int32_t scId;
    secCompService_->state_ = ServiceRunningState::STATE_RUNNING;
    secCompService_->Initialize();
    EXPECT_EQ(SC_SERVICE_ERROR_VALUE_INVALID, secCompService_->RegisterSecurityComponentBody(SAVE_COMPONENT, "", scId));
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
    EXPECT_EQ(SC_SERVICE_ERROR_VALUE_INVALID,
        secCompService_->RegisterSecurityComponentBody(SAVE_COMPONENT, "{a=", scId));

    // register security component ok
    EXPECT_EQ(SC_OK, secCompService_->RegisterSecurityComponentBody(SAVE_COMPONENT, saveInfo, scId));
    EXPECT_EQ(SC_OK, secCompService_->UpdateSecurityComponentBody(scId, saveInfo));
    uint8_t buffer[1] = { 0 };
    struct SecCompClickEvent touch = {
        .type = ClickEventType::POINT_EVENT_TYPE,
        .point.touchX = 100,
        .point.touchY = 100,
        .point.timestamp =
            static_cast<uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count()) /
            ServiceTestCommon::TIME_CONVERSION_UNIT,
        .extraInfo = {
            .data = buffer,
            .dataSize = 1
        },
    };
    SecCompInfo secCompInfo{ scId, saveInfo, touch };
    std::string message;
    EXPECT_EQ(SC_OK, secCompService_->ReportSecurityComponentClickEventBody(secCompInfo, nullptr, nullptr, message));
    EXPECT_EQ(SC_OK, secCompService_->UnregisterSecurityComponentBody(scId));
    SecCompPermManager::GetInstance().applySaveCountMap_.clear();
}

/**
 * @tc.name: RegisterSecurityComponent002
 * @tc.desc: Test register security component check touch info failed
 * @tc.type: FUNC
 * @tc.require:
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
    EXPECT_EQ(SC_OK, secCompService_->RegisterSecurityComponentBody(SAVE_COMPONENT, saveInfo, scId));
    struct SecCompClickEvent touch = {
        .type = ClickEventType::POINT_EVENT_TYPE,
        .point.touchX = 100,
        .point.touchY = 100,
        .point.timestamp = static_cast<uint64_t>(
            std::chrono::high_resolution_clock::now().time_since_epoch().count())
    };
    SecCompInfo secCompInfo{ scId, saveInfo, touch };
    std::string message;
    EXPECT_EQ(SC_SERVICE_ERROR_CLICK_EVENT_INVALID,
        secCompService_->ReportSecurityComponentClickEventBody(secCompInfo, nullptr, nullptr, message));
    EXPECT_EQ(SC_OK, secCompService_->UnregisterSecurityComponentBody(scId));
    SecCompPermManager::GetInstance().applySaveCountMap_.clear();
}

/**
 * @tc.name: RegisterSecurityComponent003
 * @tc.desc: Test register security component permission grant failed
 * @tc.type: FUNC
 * @tc.require:
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
    EXPECT_EQ(SC_OK, secCompService_->RegisterSecurityComponentBody(SAVE_COMPONENT, saveInfo, scId));
    uint8_t buffer[1] = { 0 };
    struct SecCompClickEvent touch = {
        .type = ClickEventType::POINT_EVENT_TYPE,
        .point.touchX = 100,
        .point.touchY = 100,
        .point.timestamp =
            static_cast<uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count()) /
            ServiceTestCommon::TIME_CONVERSION_UNIT,
        .extraInfo = {
            .data = buffer,
            .dataSize = 1
        },
    };
    SecCompInfo secCompInfo{ scId, saveInfo, touch };
    std::string message;
    EXPECT_EQ(SC_SERVICE_ERROR_PERMISSION_OPER_FAIL,
        secCompService_->ReportSecurityComponentClickEventBody(secCompInfo, nullptr, nullptr, message));
    EXPECT_EQ(SC_OK, secCompService_->UnregisterSecurityComponentBody(scId));
    SecCompPermManager::GetInstance().applySaveCountMap_.clear();
}

/**
 * @tc.name: ReportSecurityComponentClickEvent001
 * @tc.desc: Test register security component
 * @tc.type: FUNC
 * @tc.require:
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
    EXPECT_EQ(SC_OK, secCompService_->RegisterSecurityComponentBody(SAVE_COMPONENT, saveInfo, scId));
    uint8_t buffer[1] = { 0 };
    struct SecCompClickEvent clickInfo = {
        .type = ClickEventType::POINT_EVENT_TYPE,
        .point.touchX = 100,
        .point.touchY = 100,
        .point.timestamp =
            static_cast<uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count()) /
            ServiceTestCommon::TIME_CONVERSION_UNIT,
        .extraInfo = {
            .data = buffer,
            .dataSize = 1
        },
    };
    SecCompInfo secCompInfo{ scId, saveInfo, clickInfo };
    std::string message;
    ASSERT_EQ(SC_OK, secCompService_->ReportSecurityComponentClickEventBody(secCompInfo, nullptr, nullptr, message));

    // test 10s valid
    bool isGranted;
    secCompService_->VerifySavePermission(ServiceTestCommon::HAP_TOKEN_ID, isGranted);
    ASSERT_TRUE(isGranted);
    secCompService_->VerifySavePermission(ServiceTestCommon::HAP_TOKEN_ID, isGranted);
    ASSERT_TRUE(isGranted);
    sleep(11);
    secCompService_->VerifySavePermission(ServiceTestCommon::HAP_TOKEN_ID, isGranted);
    ASSERT_FALSE(isGranted);

    // test 10s multiple clicks
    secCompInfo.clickInfo.point.timestamp = static_cast<uint64_t>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count()) / ServiceTestCommon::TIME_CONVERSION_UNIT;
    ASSERT_EQ(SC_OK, secCompService_->ReportSecurityComponentClickEventBody(secCompInfo, nullptr, nullptr, message));
    sleep(3);
    secCompInfo.clickInfo.point.timestamp = static_cast<uint64_t>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count()) / ServiceTestCommon::TIME_CONVERSION_UNIT;
    ASSERT_EQ(SC_OK, secCompService_->ReportSecurityComponentClickEventBody(secCompInfo, nullptr, nullptr, message));
    sleep(8);
    secCompService_->VerifySavePermission(ServiceTestCommon::HAP_TOKEN_ID, isGranted);
    ASSERT_TRUE(isGranted);
    sleep(2);
    EXPECT_EQ(SC_OK, secCompService_->UnregisterSecurityComponentBody(scId));
}

/**
 * @tc.name: ReportSecurityComponentClickEventBody002
 * @tc.desc: Test verify location permission
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompServiceMockTest, ReportSecurityComponentClickEventBody002, TestSize.Level1)
{
    SC_LOG_INFO(LABEL, "ReportSecurityComponentClickEvent002");
    int32_t scId;
    secCompService_->state_ = ServiceRunningState::STATE_RUNNING;
    secCompService_->Initialize();
    nlohmann::json jsonRes;
    ServiceTestCommon::BuildLocationComponentJson(jsonRes);
    std::string locationInfo = jsonRes.dump();

    ASSERT_EQ(0, SetSelfTokenID(ServiceTestCommon::HAP_TOKEN_ID));
    AppExecFwk::AppStateData stateData = {
        .uid = getuid()
    };
    secCompService_->appStateObserver_->AddProcessToForegroundSet(stateData);
    // register security component ok
    EXPECT_EQ(SC_OK, secCompService_->RegisterSecurityComponentBody(LOCATION_COMPONENT, locationInfo, scId));
    uint8_t buffer[1] = { 0 };
    struct SecCompClickEvent clickInfo1 = {
        .type = ClickEventType::POINT_EVENT_TYPE,
        .point.touchX = 100,
        .point.touchY = 100,
        .point.timestamp =
            static_cast<uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count()) /
            ServiceTestCommon::TIME_CONVERSION_UNIT,
        .extraInfo = {
            .data = buffer,
            .dataSize = 1
        },
    };
    SecCompInfo secCompInfo{ scId, locationInfo, clickInfo1 };
    std::string message;
    ASSERT_EQ(SC_OK,
        secCompService_->ReportSecurityComponentClickEventBody(secCompInfo, nullptr, nullptr, message));

    // test 10s valid
    ASSERT_EQ(AccessTokenKit::VerifyAccessToken(ServiceTestCommon::HAP_TOKEN_ID, "ohos.permission.LOCATION"), 0);
    ASSERT_EQ(AccessTokenKit::VerifyAccessToken(ServiceTestCommon::HAP_TOKEN_ID,
        "ohos.permission.APPROXIMATELY_LOCATION"), 0);
    sleep(11);
    ASSERT_EQ(AccessTokenKit::VerifyAccessToken(ServiceTestCommon::HAP_TOKEN_ID, "ohos.permission.LOCATION"), 0);
    ASSERT_EQ(AccessTokenKit::VerifyAccessToken(ServiceTestCommon::HAP_TOKEN_ID,
        "ohos.permission.APPROXIMATELY_LOCATION"), 0);

    SecCompManager::GetInstance().NotifyProcessBackground(getpid());
    ASSERT_EQ(AccessTokenKit::VerifyAccessToken(ServiceTestCommon::HAP_TOKEN_ID, "ohos.permission.LOCATION"), 0);
    ASSERT_EQ(AccessTokenKit::VerifyAccessToken(ServiceTestCommon::HAP_TOKEN_ID,
        "ohos.permission.APPROXIMATELY_LOCATION"), 0);
    sleep(11);
    ASSERT_NE(AccessTokenKit::VerifyAccessToken(ServiceTestCommon::HAP_TOKEN_ID, "ohos.permission.LOCATION"), 0);
    ASSERT_NE(AccessTokenKit::VerifyAccessToken(ServiceTestCommon::HAP_TOKEN_ID,
        "ohos.permission.APPROXIMATELY_LOCATION"), 0);
}
