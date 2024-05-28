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
#include <gtest/gtest.h>
#include "i_sec_comp_probe.h"
#include "location_button.h"
#include "sec_comp_enhance_adapter.h"
#include "sec_comp_enhance_kit.h"
#include "sec_comp_err.h"
#include "sec_comp_info.h"
#include "sec_comp_kit.h"
#include "sec_comp_log.h"
#include "sec_comp_tool.h"
#include "sec_comp_ui_register.h"
#include "system_ability_load_callback_stub.h"
#include "test_common.h"
#include "token_setproc.h"

using namespace testing::ext;
using namespace OHOS::Security::SecurityComponent;
using namespace OHOS::Security::AccessToken;
using namespace OHOS;

namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
    LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "SecCompRegisterCallbackTest"};
static AccessTokenID g_selfTokenId = 0;
static int32_t g_selfUid = 0;
static uint32_t g_token_sum = 0;

class MockUiSecCompProbe : public ISecCompProbe {
public:
    int32_t GetComponentInfo(int32_t nodeId, std::string& componentInfo) override
    {
        componentInfo = mockComponentInfo;
        return mockRes;
    }
    std::string mockComponentInfo;
    int32_t mockRes;
};

static MockUiSecCompProbe g_probe;

static __attribute__((noinline)) int32_t RegisterSecurityComponent(
    SecCompType type, std::string& componentInfo, int32_t& scId)
{
    SC_LOG_INFO(LABEL, "RegisterSecurityComponent enter");
    return SecCompKit::RegisterSecurityComponent(type, componentInfo, scId);
}

static __attribute__((noinline)) int32_t ReportSecurityComponentClickEvent(
    int32_t scId, std::string& componentInfo,
    const SecCompClickEvent& clickInfo, sptr<IRemoteObject> callerToken, OnFirstUseDialogCloseFunc dialogCall)
{
    SC_LOG_INFO(LABEL, "ReportSecurityComponentClickEvent enter");
    return SecCompKit::ReportSecurityComponentClickEvent(scId, componentInfo, clickInfo, callerToken,
        std::move(dialogCall));
}

static __attribute__((noinline)) int32_t UpdateSecurityComponent(int32_t scId, std::string& componentInfo)
{
    SC_LOG_INFO(LABEL, "UpdateSecurityComponent enter");
    return SecCompKit::UpdateSecurityComponent(scId, componentInfo);
}

static void InitUiRegister()
{
    std::vector<uintptr_t> callerList = {
        reinterpret_cast<uintptr_t>(RegisterSecurityComponent),
        reinterpret_cast<uintptr_t>(ReportSecurityComponentClickEvent),
        reinterpret_cast<uintptr_t>(UpdateSecurityComponent)
    };
    SecCompUiRegister registerCallback(callerList, &g_probe);
}
}  // namespace

class SecCompRegisterCallbackTest : public testing::Test {
public:
    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp() override;

    void TearDown() override;
};

void SecCompRegisterCallbackTest::SetUpTestCase()
{
    system("kill -9 `pidof security_component_service`");
    InitUiRegister();
    SC_LOG_INFO(LABEL, "SecCompRegisterCallbackTest.");
}

void SecCompRegisterCallbackTest::TearDownTestCase()
{
    SC_LOG_INFO(LABEL, "SecCompRegisterCallbackTest.");
}

void SecCompRegisterCallbackTest::SetUp()
{
    g_selfUid = getuid();
    g_selfTokenId = GetSelfTokenID();
    SC_LOG_INFO(LABEL, "SetUp ok.");
}

void SecCompRegisterCallbackTest::TearDown()
{
    setuid(g_selfUid);
    EXPECT_EQ(0, SetSelfTokenID(g_selfTokenId));
    SC_LOG_INFO(LABEL, "TearDown.");
}

/**
 * @tc.name: RegisterSecurityComponent001
 * @tc.desc: test register security component success.
 * @tc.type: FUNC
 * @tc.require: AR000HO9JM
 */
HWTEST_F(SecCompRegisterCallbackTest, RegisterSecurityComponent001, TestSize.Level1)
{
    nlohmann::json jsonRes;
    TestCommon::BuildLocationComponentInfo(jsonRes);
    std::string locationInfo = jsonRes.dump();
    g_probe.mockComponentInfo = locationInfo;
    g_probe.mockRes = 0;

    int32_t scId;
    ASSERT_EQ(SC_OK, RegisterSecurityComponent(LOCATION_COMPONENT, locationInfo, scId));
    ASSERT_NE(-1, scId);

    EXPECT_EQ(SC_OK, SecCompKit::UnregisterSecurityComponent(scId));
}

/**
 * @tc.name: RegisterSecurityComponent002
 * @tc.desc: test register callback failed.
 * @tc.type: FUNC
 * @tc.require: AR000HO9JM
 */
HWTEST_F(SecCompRegisterCallbackTest, RegisterSecurityComponent002, TestSize.Level1)
{
    nlohmann::json jsonRes;
    TestCommon::BuildLocationComponentInfo(jsonRes);
    std::string locationInfo = jsonRes.dump();
    g_probe.mockComponentInfo = locationInfo;
    g_probe.mockRes = -1;

    int32_t scId;
#ifdef SECURITY_COMPONENT_ENHANCE_ENABLE
    ASSERT_EQ(SC_ENHANCE_ERROR_CALLBACK_OPER_FAIL,
        RegisterSecurityComponent(LOCATION_COMPONENT, locationInfo, scId));
    ASSERT_EQ(-1, scId);
#else
    ASSERT_EQ(SC_OK,
        RegisterSecurityComponent(LOCATION_COMPONENT, locationInfo, scId));
    ASSERT_NE(-1, scId);
    ASSERT_EQ(SC_OK, SecCompKit::UnregisterSecurityComponent(scId));
#endif
}

/**
 * @tc.name: RegisterSecurityComponent003
 * @tc.desc: test register in MaliciousAppList.
 * @tc.type: FUNC
 * @tc.require: AR000HO9JM
 */
HWTEST_F(SecCompRegisterCallbackTest, RegisterSecurityComponent003, TestSize.Level1)
{
    nlohmann::json jsonRes;
    TestCommon::BuildLocationComponentInfo(jsonRes);
    std::string locationInfo = jsonRes.dump();
    g_probe.mockComponentInfo = locationInfo;
    g_probe.mockRes = 0;

    int32_t scId;
    ASSERT_EQ(SC_OK, RegisterSecurityComponent(LOCATION_COMPONENT, locationInfo, scId));
    ASSERT_EQ(SC_OK, SecCompKit::UnregisterSecurityComponent(scId));
}

/**
 * @tc.name: RegisterSecurityComponent004
 * @tc.desc: Test register security component check touch info failed
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SecCompRegisterCallbackTest, RegisterSecurityComponent004, TestSize.Level1)
{
    system("param set sec.comp.enhance 1");
    nlohmann::json jsonRes;
    TestCommon::BuildSaveComponentInfo(jsonRes);
    std::string saveInfo = jsonRes.dump();
    int32_t scId;

    EXPECT_EQ(SC_OK, RegisterSecurityComponent(SAVE_COMPONENT, saveInfo, scId));
    uint8_t data[TestCommon::MAX_HMAC_SIZE] = { 0 };
    struct SecCompClickEvent clickInfo = {
        .type = ClickEventType::POINT_EVENT_TYPE,
        .point.touchX = TestCommon::TEST_COORDINATE,
        .point.touchY = TestCommon::TEST_COORDINATE,
        .point.timestamp = static_cast<uint64_t>(
            std::chrono::high_resolution_clock::now().time_since_epoch().count())
    };
    clickInfo.extraInfo.dataSize = TestCommon::MAX_HMAC_SIZE;
    clickInfo.extraInfo.data = data;

    sptr<SystemAbilityLoadCallbackStub> callback = new (std::nothrow) SystemAbilityLoadCallbackStub();
    ASSERT_NE(callback, nullptr);
    auto token = callback->AsObject();
    OnFirstUseDialogCloseFunc func = [] (int32_t) {};
    EXPECT_EQ(SC_SERVICE_ERROR_CLICK_EVENT_INVALID,
        ReportSecurityComponentClickEvent(scId, saveInfo, clickInfo, token, std::move(func)));
    EXPECT_EQ(SC_OK, SecCompKit::UnregisterSecurityComponent(scId));
    system("param set sec.comp.enhance 0");
}

/**
 * @tc.name: RegisterSecurityComponent005
 * @tc.desc: Test register security component permission grant failed
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SecCompRegisterCallbackTest, RegisterSecurityComponent005, TestSize.Level1)
{
    system("param set sec.comp.enhance 1");
    nlohmann::json jsonRes;
    TestCommon::BuildSaveComponentInfo(jsonRes);
    std::string saveInfo = jsonRes.dump();
    int32_t scId;

    EXPECT_EQ(SC_OK, RegisterSecurityComponent(SAVE_COMPONENT, saveInfo, scId));
    uint8_t data[TestCommon::MAX_HMAC_SIZE] = { 0 };
    struct SecCompClickEvent clickInfo = {
        .type = ClickEventType::POINT_EVENT_TYPE,
        .point.touchX = TestCommon::TEST_COORDINATE,
        .point.touchY = TestCommon::TEST_COORDINATE,
        .point.timestamp = static_cast<uint64_t>(
            std::chrono::high_resolution_clock::now().time_since_epoch().count()) / TestCommon::TIME_CONVERSION_UNIT
    };
    clickInfo.extraInfo.dataSize = TestCommon::MAX_HMAC_SIZE;
    clickInfo.extraInfo.data = data;
    sptr<SystemAbilityLoadCallbackStub> callback = new (std::nothrow) SystemAbilityLoadCallbackStub();
    ASSERT_NE(callback, nullptr);
    auto token = callback->AsObject();
    OnFirstUseDialogCloseFunc func = [] (int32_t) {};
    EXPECT_EQ(SC_SERVICE_ERROR_WAIT_FOR_DIALOG_CLOSE,
        ReportSecurityComponentClickEvent(scId, saveInfo, clickInfo, token, std::move(func)));
    EXPECT_EQ(SC_OK, SecCompKit::UnregisterSecurityComponent(scId));
    system("param set sec.comp.enhance 0");
}

/**
 * @tc.name: RegisterSecurityComponent006
 * @tc.desc: Test register security component permission grant failed caller error
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SecCompRegisterCallbackTest, RegisterSecurityComponent006, TestSize.Level1)
{
    nlohmann::json jsonRes;
    TestCommon::BuildSaveComponentInfo(jsonRes);
    std::string saveInfo = jsonRes.dump();
    int32_t scId;
    setuid(100);
    EXPECT_EQ(SC_SERVICE_ERROR_VALUE_INVALID, RegisterSecurityComponent(SAVE_COMPONENT, saveInfo, scId));
}

/**
 * @tc.name: ReportSecurityComponentClickEvent001
 * @tc.desc: Test register security component success
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SecCompRegisterCallbackTest, ReportSecurityComponentClickEvent001, TestSize.Level1)
{
    system("param set sec.comp.enhance 1");
    nlohmann::json jsonRes;
    TestCommon::BuildSaveComponentInfo(jsonRes);
    std::string saveInfo = jsonRes.dump();
    int32_t scId;

    EXPECT_EQ(SC_OK, RegisterSecurityComponent(SAVE_COMPONENT, saveInfo, scId));
    uint8_t data[TestCommon::MAX_HMAC_SIZE] = { 0 };
    struct SecCompClickEvent clickInfo = {
        .type = ClickEventType::POINT_EVENT_TYPE,
        .point.touchX = TestCommon::TEST_COORDINATE,
        .point.touchY = TestCommon::TEST_COORDINATE,
        .point.timestamp = static_cast<uint64_t>(
            std::chrono::high_resolution_clock::now().time_since_epoch().count()) / TestCommon::TIME_CONVERSION_UNIT
    };
    clickInfo.extraInfo.dataSize = TestCommon::MAX_HMAC_SIZE;
    clickInfo.extraInfo.data = data;

    sptr<SystemAbilityLoadCallbackStub> callback = new (std::nothrow) SystemAbilityLoadCallbackStub();
    ASSERT_NE(callback, nullptr);
    auto token = callback->AsObject();
    OnFirstUseDialogCloseFunc func = [] (int32_t) {};
    ASSERT_EQ(SC_SERVICE_ERROR_WAIT_FOR_DIALOG_CLOSE,
        ReportSecurityComponentClickEvent(scId, saveInfo, clickInfo, token, std::move(func)));
    EXPECT_EQ(SC_OK, SecCompKit::UnregisterSecurityComponent(scId));
    system("param set sec.comp.enhance 0");
}

/**
 * @tc.name: ReportSecurityComponentClickEvent002
 * @tc.desc: Test report security component caller error
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SecCompRegisterCallbackTest, ReportSecurityComponentClickEvent002, TestSize.Level1)
{
    system("param set sec.comp.enhance 1");
    nlohmann::json jsonRes;
    TestCommon::BuildSaveComponentInfo(jsonRes);
    std::string saveInfo = jsonRes.dump();
    int32_t scId;
    ASSERT_EQ(0, SetSelfTokenID(TestCommon::HAP_TOKEN_ID + g_token_sum));
    g_token_sum ++;

    EXPECT_EQ(SC_OK, RegisterSecurityComponent(SAVE_COMPONENT, saveInfo, scId));
    uint8_t data[TestCommon::MAX_HMAC_SIZE] = { 0 };
    struct SecCompClickEvent clickInfo = {
        .type = ClickEventType::POINT_EVENT_TYPE,
        .point.touchX = TestCommon::TEST_COORDINATE,
        .point.touchY = TestCommon::TEST_COORDINATE,
        .point.timestamp = static_cast<uint64_t>(
            std::chrono::high_resolution_clock::now().time_since_epoch().count()) / TestCommon::TIME_CONVERSION_UNIT
    };
    clickInfo.extraInfo.dataSize = TestCommon::MAX_HMAC_SIZE;
    clickInfo.extraInfo.data = data;

    setuid(100);
    sptr<SystemAbilityLoadCallbackStub> callback = new (std::nothrow) SystemAbilityLoadCallbackStub();
    ASSERT_NE(callback, nullptr);
    auto token = callback->AsObject();
    OnFirstUseDialogCloseFunc func = [] (int32_t) {};
    ASSERT_EQ(SC_SERVICE_ERROR_VALUE_INVALID,
        ReportSecurityComponentClickEvent(scId, saveInfo, clickInfo, token, std::move(func)));
    setuid(g_selfUid);
    EXPECT_EQ(SC_OK, SecCompKit::UnregisterSecurityComponent(scId));
    system("param set sec.comp.enhance 0");
}

/**
 * @tc.name: ReportSecurityComponentClickEvent003
 * @tc.desc: Test report security component enhance data is empty
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SecCompRegisterCallbackTest, ReportSecurityComponentClickEvent003, TestSize.Level1)
{
    nlohmann::json jsonRes;
    TestCommon::BuildSaveComponentInfo(jsonRes);
    std::string saveInfo = jsonRes.dump();
    int32_t scId;
    ASSERT_EQ(0, SetSelfTokenID(TestCommon::HAP_TOKEN_ID + g_token_sum));
    g_token_sum ++;

    EXPECT_EQ(SC_OK, RegisterSecurityComponent(SAVE_COMPONENT, saveInfo, scId));

    struct SecCompClickEvent clickInfo = {
        .type = ClickEventType::POINT_EVENT_TYPE,
        .point.touchX = TestCommon::TEST_COORDINATE,
        .point.touchY = TestCommon::TEST_COORDINATE,
        .point.timestamp = static_cast<uint64_t>(
            std::chrono::high_resolution_clock::now().time_since_epoch().count()) / TestCommon::TIME_CONVERSION_UNIT
    };
    sptr<SystemAbilityLoadCallbackStub> callback = new (std::nothrow) SystemAbilityLoadCallbackStub();
    ASSERT_NE(callback, nullptr);
    auto token = callback->AsObject();
    OnFirstUseDialogCloseFunc func = [] (int32_t) {};
#ifdef SECURITY_COMPONENT_ENHANCE_ENABLE
    ASSERT_EQ(SC_SERVICE_ERROR_CLICK_EVENT_INVALID,
        ReportSecurityComponentClickEvent(scId, saveInfo, clickInfo, token, std::move(func)));
#else
    ASSERT_EQ(SC_SERVICE_ERROR_WAIT_FOR_DIALOG_CLOSE,
        ReportSecurityComponentClickEvent(scId, saveInfo, clickInfo, token, std::move(func)));
#endif
    EXPECT_EQ(SC_OK, SecCompKit::UnregisterSecurityComponent(scId));
}

/**
 * @tc.name: ReportSecurityComponentClickEvent004
 * @tc.desc: Test report security component dialog callback is nullptr
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SecCompRegisterCallbackTest, ReportSecurityComponentClickEvent004, TestSize.Level1)
{
    nlohmann::json jsonRes;
    TestCommon::BuildSaveComponentInfo(jsonRes);
    std::string saveInfo = jsonRes.dump();
    int32_t scId;
    ASSERT_EQ(0, SetSelfTokenID(TestCommon::HAP_TOKEN_ID + g_token_sum));
    g_token_sum ++;

    EXPECT_EQ(SC_OK, RegisterSecurityComponent(SAVE_COMPONENT, saveInfo, scId));

    struct SecCompClickEvent clickInfo = {
        .type = ClickEventType::POINT_EVENT_TYPE,
        .point.touchX = TestCommon::TEST_COORDINATE,
        .point.touchY = TestCommon::TEST_COORDINATE,
        .point.timestamp = static_cast<uint64_t>(
            std::chrono::high_resolution_clock::now().time_since_epoch().count()) / TestCommon::TIME_CONVERSION_UNIT
    };
    sptr<SystemAbilityLoadCallbackStub> callback = new (std::nothrow) SystemAbilityLoadCallbackStub();
    ASSERT_NE(callback, nullptr);
    auto token = callback->AsObject();
    OnFirstUseDialogCloseFunc func = nullptr;
    ASSERT_EQ(SC_ENHANCE_ERROR_VALUE_INVALID,
        ReportSecurityComponentClickEvent(scId, saveInfo, clickInfo, token, std::move(func)));
    EXPECT_EQ(SC_OK, SecCompKit::UnregisterSecurityComponent(scId));
}

/**
 * @tc.name: ReportClickWithoutHmac001
 * @tc.desc: Test report click event permission denied
 * @tc.type: FUNC
 * @tc.require: AR000HO9JM
 */
HWTEST_F(SecCompRegisterCallbackTest, ReportClickWithoutHmac001, TestSize.Level1)
{
    system("param set sec.comp.enhance 1");
    nlohmann::json jsonRes;
    TestCommon::BuildLocationComponentInfo(jsonRes);
    std::string locationInfo = jsonRes.dump();
    g_probe.mockComponentInfo = locationInfo;
    g_probe.mockRes = 0;

    int32_t scId;
    ASSERT_EQ(SC_OK, RegisterSecurityComponent(LOCATION_COMPONENT, locationInfo, scId));
    ASSERT_NE(-1, scId);
    uint8_t data[TestCommon::MAX_HMAC_SIZE] = { 0 };
    struct SecCompClickEvent clickInfo = {
        .type = ClickEventType::POINT_EVENT_TYPE,
        .point.touchX = TestCommon::TEST_COORDINATE,
        .point.touchY = TestCommon::TEST_COORDINATE,
        .point.timestamp = static_cast<uint64_t>(
            std::chrono::high_resolution_clock::now().time_since_epoch().count()) / TestCommon::TIME_CONVERSION_UNIT
    };
    clickInfo.extraInfo.dataSize = TestCommon::MAX_HMAC_SIZE;
    clickInfo.extraInfo.data = data;
    sptr<SystemAbilityLoadCallbackStub> callback = new (std::nothrow) SystemAbilityLoadCallbackStub();
    ASSERT_NE(callback, nullptr);
    auto token = callback->AsObject();
    OnFirstUseDialogCloseFunc func = [] (int32_t) {};
    EXPECT_EQ(SC_SERVICE_ERROR_WAIT_FOR_DIALOG_CLOSE,
        ReportSecurityComponentClickEvent(scId, locationInfo, clickInfo, token, std::move(func)));
    EXPECT_EQ(SC_OK, SecCompKit::UnregisterSecurityComponent(scId));
    system("param set sec.comp.enhance 0");
}

/**
 * @tc.name: VerifySavePermission001
 * @tc.desc: Test register security component wrong hap
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SecCompRegisterCallbackTest, VerifySavePermission001, TestSize.Level1)
{
    system("param set sec.comp.enhance 1");
    nlohmann::json jsonRes;
    TestCommon::BuildSaveComponentInfo(jsonRes);
    std::string saveInfo = jsonRes.dump();
    int32_t scId;
    ASSERT_EQ(0, SetSelfTokenID(TestCommon::HAP_TOKEN_ID + g_token_sum));
    g_token_sum ++;

    EXPECT_EQ(SC_OK, RegisterSecurityComponent(SAVE_COMPONENT, saveInfo, scId));
    uint8_t data[TestCommon::MAX_HMAC_SIZE] = { 0 };
    struct SecCompClickEvent clickInfo = {
        .type = ClickEventType::POINT_EVENT_TYPE,
        .point.touchX = TestCommon::TEST_COORDINATE,
        .point.touchY = TestCommon::TEST_COORDINATE,
        .point.timestamp = static_cast<uint64_t>(
            std::chrono::high_resolution_clock::now().time_since_epoch().count()) / TestCommon::TIME_CONVERSION_UNIT
    };
    clickInfo.extraInfo.dataSize = TestCommon::MAX_HMAC_SIZE;
    clickInfo.extraInfo.data = data;
    sptr<SystemAbilityLoadCallbackStub> callback = new (std::nothrow) SystemAbilityLoadCallbackStub();
    ASSERT_NE(callback, nullptr);
    auto token = callback->AsObject();
    OnFirstUseDialogCloseFunc func = [] (int32_t) {};
    ASSERT_EQ(SC_SERVICE_ERROR_WAIT_FOR_DIALOG_CLOSE,
        ReportSecurityComponentClickEvent(scId, saveInfo, clickInfo, token, std::move(func)));
    setuid(100);
    ASSERT_FALSE(SecCompKit::VerifySavePermission(TestCommon::HAP_TOKEN_ID));
    // mediaLibraryTokenId_ != 0
    ASSERT_FALSE(SecCompKit::VerifySavePermission(TestCommon::HAP_TOKEN_ID));
    setuid(g_selfUid);
    EXPECT_EQ(SC_OK, SecCompKit::UnregisterSecurityComponent(scId));
    system("param set sec.comp.enhance 0");
}

/**
 * @tc.name: VerifySavePermission002
 * @tc.desc: Test register security component invalid tokenId
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SecCompRegisterCallbackTest, VerifySavePermission002, TestSize.Level1)
{
    system("param set sec.comp.enhance 1");
    nlohmann::json jsonRes;
    TestCommon::BuildSaveComponentInfo(jsonRes);
    std::string saveInfo = jsonRes.dump();
    int32_t scId;

    EXPECT_EQ(SC_OK, RegisterSecurityComponent(SAVE_COMPONENT, saveInfo, scId));
    uint8_t data[TestCommon::MAX_HMAC_SIZE] = { 0 };
    struct SecCompClickEvent clickInfo = {
        .type = ClickEventType::POINT_EVENT_TYPE,
        .point.touchX = TestCommon::TEST_COORDINATE,
        .point.touchY = TestCommon::TEST_COORDINATE,
        .point.timestamp = static_cast<uint64_t>(
            std::chrono::high_resolution_clock::now().time_since_epoch().count()) / TestCommon::TIME_CONVERSION_UNIT
    };
    clickInfo.extraInfo.dataSize = TestCommon::MAX_HMAC_SIZE;
    clickInfo.extraInfo.data = data;
    sptr<SystemAbilityLoadCallbackStub> callback = new (std::nothrow) SystemAbilityLoadCallbackStub();
    ASSERT_NE(callback, nullptr);
    auto token = callback->AsObject();
    OnFirstUseDialogCloseFunc func = [] (int32_t) {};
    ASSERT_EQ(SC_SERVICE_ERROR_WAIT_FOR_DIALOG_CLOSE,
        ReportSecurityComponentClickEvent(scId, saveInfo, clickInfo, token, std::move(func)));
    EXPECT_EQ(SC_OK, SecCompKit::UnregisterSecurityComponent(scId));
    system("param set sec.comp.enhance 0");
}

/**
 * @tc.name: UnregisterSecurityComponent001
 * @tc.desc: Test unregister security component caller error
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SecCompRegisterCallbackTest, UnregisterSecurityComponent001, TestSize.Level1)
{
    system("param set sec.comp.enhance 1");
    nlohmann::json jsonRes;
    TestCommon::BuildSaveComponentInfo(jsonRes);
    std::string saveInfo = jsonRes.dump();
    int32_t scId;

    EXPECT_EQ(SC_OK, RegisterSecurityComponent(SAVE_COMPONENT, saveInfo, scId));
    uint8_t data[TestCommon::MAX_HMAC_SIZE] = { 0 };
    struct SecCompClickEvent clickInfo = {
        .type = ClickEventType::POINT_EVENT_TYPE,
        .point.touchX = TestCommon::TEST_COORDINATE,
        .point.touchY = TestCommon::TEST_COORDINATE,
        .point.timestamp = static_cast<uint64_t>(
            std::chrono::high_resolution_clock::now().time_since_epoch().count()) / TestCommon::TIME_CONVERSION_UNIT
    };
    clickInfo.extraInfo.dataSize = TestCommon::MAX_HMAC_SIZE;
    clickInfo.extraInfo.data = data;
    sptr<SystemAbilityLoadCallbackStub> callback = new (std::nothrow) SystemAbilityLoadCallbackStub();
    ASSERT_NE(callback, nullptr);
    auto token = callback->AsObject();
    OnFirstUseDialogCloseFunc func = [] (int32_t) {};
    EXPECT_EQ(SC_SERVICE_ERROR_WAIT_FOR_DIALOG_CLOSE,
        ReportSecurityComponentClickEvent(scId, saveInfo, clickInfo, token, std::move(func)));
    EXPECT_EQ(SC_OK, SecCompKit::UnregisterSecurityComponent(scId));
    system("param set sec.comp.enhance 0");
}

/**
 * @tc.name: UpdateSecurityComponent001
 * @tc.desc: Test update security component success
 * @tc.type: FUNC
 * @tc.require: AR000HO9IN
 */
HWTEST_F(SecCompRegisterCallbackTest, UpdateSecurityComponent001, TestSize.Level1)
{
    nlohmann::json jsonRes;
    TestCommon::BuildSaveComponentInfo(jsonRes);
    std::string saveInfo = jsonRes.dump();
    int32_t scId;
    ASSERT_EQ(SC_OK, RegisterSecurityComponent(SAVE_COMPONENT, saveInfo, scId));
    ASSERT_NE(-1, scId);
    ASSERT_EQ(SC_OK, UpdateSecurityComponent(scId, saveInfo));
    EXPECT_EQ(SC_OK, SecCompKit::UnregisterSecurityComponent(scId));
}

/**
 * @tc.name: UpdateSecurityComponent002
 * @tc.desc: Test update security component caller error
 * @tc.type: FUNC
 * @tc.require: AR000HO9IN
 */
HWTEST_F(SecCompRegisterCallbackTest, UpdateSecurityComponent002, TestSize.Level1)
{
    nlohmann::json jsonRes;
    TestCommon::BuildSaveComponentInfo(jsonRes);
    std::string saveInfo = jsonRes.dump();
    int32_t scId;

    ASSERT_EQ(0, SetSelfTokenID(TestCommon::HAP_TOKEN_ID + g_token_sum));
    g_token_sum ++;
    ASSERT_EQ(SC_OK, RegisterSecurityComponent(SAVE_COMPONENT, saveInfo, scId));
    ASSERT_NE(-1, scId);
    setuid(100);
    ASSERT_EQ(SC_SERVICE_ERROR_VALUE_INVALID, UpdateSecurityComponent(scId, saveInfo));
    setuid(g_selfUid);
    EXPECT_EQ(SC_OK, SecCompKit::UnregisterSecurityComponent(scId));
}
