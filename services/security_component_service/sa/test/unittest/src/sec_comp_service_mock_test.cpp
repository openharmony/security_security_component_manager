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
#include "sec_comp_info.h"
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
 * @tc.name: WriteError001
 * @tc.desc: Test WriteError
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompServiceMockTest, WriteError001, TestSize.Level1)
{
    secCompService_->state_ = ServiceRunningState::STATE_RUNNING;
    secCompService_->Initialize();
    SecCompRawdata rawReply;
    EXPECT_EQ(SC_OK, secCompService_->WriteError(SC_SERVICE_ERROR_VALUE_INVALID, rawReply));
}

/**
 * @tc.name: RegisterReadFromRawdata001
 * @tc.desc: Test register read from rawdata
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompServiceMockTest, RegisterReadFromRawdata001, TestSize.Level1)
{
    secCompService_->state_ = ServiceRunningState::STATE_RUNNING;
    secCompService_->Initialize();
    MessageParcel data;
    SecCompType type;
    std::string outComponentInfo;
    uint32_t uintType;
    std::string componentInfo;

    // rawdata.data is nullptr
    SecCompRawdata rawdataVoid;
    rawdataVoid.size = 1;
    EXPECT_EQ(SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL,
        secCompService_->RegisterReadFromRawdata(rawdataVoid, type, outComponentInfo));

    // Type is UNKONWN_SC_TYPE
    uintType = 0;
    data.WriteUint32(uintType);
    data.WriteString(componentInfo);
    SecCompRawdata rawdataSmallType;
    EXPECT_EQ(true, SecCompEnhanceAdapter::EnhanceSrvSerialize(data, rawdataSmallType));
    EXPECT_EQ(SC_SERVICE_ERROR_VALUE_INVALID,
        secCompService_->RegisterReadFromRawdata(rawdataSmallType, type, outComponentInfo));
    data.FlushBuffer();

    // Type is bigger than MAX_SC_TYPE
    uintType = 100;
    data.WriteUint32(uintType);
    data.WriteString(componentInfo);
    SecCompRawdata rawdataBigType;
    EXPECT_EQ(true, SecCompEnhanceAdapter::EnhanceSrvSerialize(data, rawdataBigType));
    EXPECT_EQ(SC_SERVICE_ERROR_VALUE_INVALID,
        secCompService_->RegisterReadFromRawdata(rawdataBigType, type, outComponentInfo));
    data.FlushBuffer();

    // register read from rawdata OK
    uintType = SAVE_COMPONENT;
    data.WriteUint32(uintType);
    data.WriteString(componentInfo);
    SecCompRawdata rawdataValidType;
    EXPECT_EQ(true, SecCompEnhanceAdapter::EnhanceSrvSerialize(data, rawdataValidType));
    EXPECT_EQ(SC_OK, secCompService_->RegisterReadFromRawdata(rawdataValidType, type, outComponentInfo));
    EXPECT_EQ(SAVE_COMPONENT, type);
}

/**
 * @tc.name: RegisterSecurityComponentBody001
 * @tc.desc: Test register security component
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompServiceMockTest, RegisterSecurityComponentBody001, TestSize.Level1)
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
 * @tc.name: RegisterSecurityComponentBody002
 * @tc.desc: Test register security component check touch info failed
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompServiceMockTest, RegisterSecurityComponentBody002, TestSize.Level1)
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
 * @tc.name: RegisterSecurityComponentBody003
 * @tc.desc: Test register security component permission grant failed
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompServiceMockTest, RegisterSecurityComponentBody003, TestSize.Level1)
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
 * @tc.name: RegisterWriteToRawdata001
 * @tc.desc: Test register write to rawdata
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompServiceMockTest, RegisterWriteToRawdata001, TestSize.Level1)
{
    secCompService_->state_ = ServiceRunningState::STATE_RUNNING;
    secCompService_->Initialize();
    int32_t scId = 1;
    int32_t res = SC_OK;
    SecCompRawdata rawReply;
    EXPECT_EQ(SC_OK, secCompService_->RegisterWriteToRawdata(res, scId, rawReply));
}

/**
 * @tc.name: RegisterSecurityComponent001
 * @tc.desc: Test register security component
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompServiceMockTest, RegisterSecurityComponent001, TestSize.Level1)
{
    secCompService_->state_ = ServiceRunningState::STATE_RUNNING;
    secCompService_->Initialize();
    MessageParcel data;
    uint32_t uintType;

    // RegisterReadFromRawdata fail
    SecCompRawdata rawdataVoid;
    rawdataVoid.size = 1;
    SecCompRawdata rawReplyReadFail;
    MessageParcel replyReadFail;
    EXPECT_EQ(SC_OK,
        secCompService_->RegisterSecurityComponent(rawdataVoid, rawReplyReadFail));
    SecCompEnhanceAdapter::EnhanceSrvDeserialize(rawReplyReadFail, replyReadFail);
    EXPECT_EQ(SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL, replyReadFail.ReadInt32());

    // RegisterSecurityComponentBody fail
    uintType = SAVE_COMPONENT;
    data.WriteUint32(uintType);
    data.WriteString("");
    SecCompRawdata rawdataBodyFail;
    SecCompRawdata rawReplyBodyFail;
    MessageParcel replyBodyFail;
    EXPECT_EQ(true, SecCompEnhanceAdapter::EnhanceSrvSerialize(data, rawdataBodyFail));
    EXPECT_EQ(SC_OK,
        secCompService_->RegisterSecurityComponent(rawdataBodyFail, rawReplyBodyFail));
    SecCompEnhanceAdapter::EnhanceSrvDeserialize(rawReplyBodyFail, replyBodyFail);
    EXPECT_EQ(SC_SERVICE_ERROR_VALUE_INVALID, replyBodyFail.ReadInt32());
    data.FlushBuffer();

    // RegisterSecurityComponent OK
    nlohmann::json jsonRes;
    ServiceTestCommon::BuildSaveComponentJson(jsonRes);
    std::string saveInfo = jsonRes.dump();
    uintType = SAVE_COMPONENT;
    data.WriteUint32(uintType);
    data.WriteString(saveInfo);
    SecCompRawdata rawdata;
    SecCompRawdata rawReply;
    MessageParcel reply;
    EXPECT_EQ(true, SecCompEnhanceAdapter::EnhanceSrvSerialize(data, rawdata));
    EXPECT_EQ(SC_OK,
        secCompService_->RegisterSecurityComponent(rawdata, rawReply));
    SecCompEnhanceAdapter::EnhanceSrvDeserialize(rawReply, reply);
    EXPECT_EQ(SC_OK, reply.ReadInt32());
}

/**
 * @tc.name: UpdateReadFromRawdata001
 * @tc.desc: Test UpdateReadFromRawdata
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompServiceMockTest, UpdateReadFromRawdata001, TestSize.Level1)
{
    secCompService_->state_ = ServiceRunningState::STATE_RUNNING;
    secCompService_->Initialize();
    MessageParcel data;
    int32_t scIdOut;
    std::string componentInfoOut;

    // rawdata.data is nullptr
    SecCompRawdata rawdataVoid;
    rawdataVoid.size = 1;
    EXPECT_EQ(SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL,
        secCompService_->UpdateReadFromRawdata(rawdataVoid, scIdOut, componentInfoOut));

    // scId is invalid
    data.WriteInt32(-1);
    SecCompRawdata rawdataScidInvalid;
    EXPECT_EQ(true, SecCompEnhanceAdapter::EnhanceSrvSerialize(data, rawdataScidInvalid));
    EXPECT_EQ(SC_SERVICE_ERROR_VALUE_INVALID,
        secCompService_->UpdateReadFromRawdata(rawdataScidInvalid, scIdOut, componentInfoOut));
    data.FlushBuffer();

    // UpdateReadFromRawdata OK
    data.WriteInt32(ServiceTestCommon::TEST_SC_ID_1);
    data.WriteString("");
    SecCompRawdata rawdata;
    EXPECT_EQ(true, SecCompEnhanceAdapter::EnhanceSrvSerialize(data, rawdata));
    EXPECT_EQ(SC_OK,
        secCompService_->UpdateReadFromRawdata(rawdata, scIdOut, componentInfoOut));
    data.FlushBuffer();
}

/**
 * @tc.name: UpdateWriteToRawdata001
 * @tc.desc: Test UpdateWriteToRawdata
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompServiceMockTest, UpdateWriteToRawdata001, TestSize.Level1)
{
    secCompService_->state_ = ServiceRunningState::STATE_RUNNING;
    secCompService_->Initialize();
    SecCompRawdata rawReply;
    EXPECT_EQ(SC_OK,
        secCompService_->UpdateWriteToRawdata(SC_OK, rawReply));
}

/**
 * @tc.name: UpdateSecurityComponent001
 * @tc.desc: Test UpdateSecurityComponent fail
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompServiceMockTest, UpdateSecurityComponent001, TestSize.Level1)
{
    secCompService_->state_ = ServiceRunningState::STATE_RUNNING;
    secCompService_->Initialize();
    MessageParcel data;

    // UpdateReadFromRawdata from fail
    SecCompRawdata rawdataVoid;
    rawdataVoid.size = 1;
    SecCompRawdata rawReplyVoid;
    MessageParcel replyVoid;
    EXPECT_EQ(SC_OK,
        secCompService_->UpdateSecurityComponent(rawdataVoid, rawReplyVoid));
    ASSERT_TRUE(SecCompEnhanceAdapter::EnhanceSrvDeserialize(rawReplyVoid, replyVoid));
    EXPECT_EQ(SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL, replyVoid.ReadInt32());

    // UpdateSecurityComponentBody fail
    SecCompRawdata rawdataBodyFail;
    SecCompRawdata rawReplyBodyFail;
    MessageParcel replyBodyFail;
    data.WriteInt32(ServiceTestCommon::TEST_SC_ID_1);
    data.WriteString("");
    EXPECT_EQ(true, SecCompEnhanceAdapter::EnhanceSrvSerialize(data, rawdataBodyFail));
    EXPECT_EQ(SC_OK,
        secCompService_->UpdateSecurityComponent(rawdataBodyFail, rawReplyBodyFail));
    ASSERT_TRUE(SecCompEnhanceAdapter::EnhanceSrvDeserialize(rawReplyBodyFail, replyBodyFail));
    EXPECT_EQ(SC_SERVICE_ERROR_VALUE_INVALID, replyBodyFail.ReadInt32());
    data.FlushBuffer();
}

/**
 * @tc.name: UpdateSecurityComponent002
 * @tc.desc: Test UpdateSecurityComponent OK
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompServiceMockTest, UpdateSecurityComponent002, TestSize.Level1)
{
    secCompService_->state_ = ServiceRunningState::STATE_RUNNING;
    secCompService_->Initialize();
    MessageParcel data;
    nlohmann::json jsonRes;
    ServiceTestCommon::BuildLocationComponentJson(jsonRes);
    std::string locationInfo = jsonRes.dump();
    int32_t scId;
    EXPECT_EQ(SC_OK, secCompService_->RegisterSecurityComponentBody(
        LOCATION_COMPONENT, locationInfo, scId));

    SecCompRawdata rawdata;
    SecCompRawdata rawReply;
    MessageParcel reply;
    data.WriteInt32(scId);
    data.WriteString(locationInfo);
    EXPECT_EQ(true, SecCompEnhanceAdapter::EnhanceSrvSerialize(data, rawdata));
    EXPECT_EQ(SC_OK,
        secCompService_->UpdateSecurityComponent(rawdata, rawReply));
    ASSERT_TRUE(SecCompEnhanceAdapter::EnhanceSrvDeserialize(rawReply, reply));
    EXPECT_EQ(SC_OK, reply.ReadInt32());
}

/**
 * @tc.name: UnregisterReadFromRawdata001
 * @tc.desc: Test UnregisterReadFromRawdata
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompServiceMockTest, UnregisterReadFromRawdata001, TestSize.Level1)
{
    secCompService_->state_ = ServiceRunningState::STATE_RUNNING;
    secCompService_->Initialize();
    MessageParcel data;
    int32_t scIdOut;
    std::string componentInfoOut;

    // rawdata.data is nullptr
    SecCompRawdata rawdataVoid;
    rawdataVoid.size = 1;
    EXPECT_EQ(SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL,
        secCompService_->UnregisterReadFromRawdata(rawdataVoid, scIdOut));

    // scId is invalid
    data.WriteInt32(-1);
    SecCompRawdata rawdataScidInvalid;
    EXPECT_EQ(true, SecCompEnhanceAdapter::EnhanceSrvSerialize(data, rawdataScidInvalid));
    EXPECT_EQ(SC_SERVICE_ERROR_VALUE_INVALID,
        secCompService_->UnregisterReadFromRawdata(rawdataScidInvalid, scIdOut));
    data.FlushBuffer();

    // UpdateReadFromRawdata OK
    data.WriteInt32(ServiceTestCommon::TEST_SC_ID_1);
    SecCompRawdata rawdata;
    EXPECT_EQ(true, SecCompEnhanceAdapter::EnhanceSrvSerialize(data, rawdata));
    EXPECT_EQ(SC_OK,
        secCompService_->UnregisterReadFromRawdata(rawdata, scIdOut));
    EXPECT_EQ(ServiceTestCommon::TEST_SC_ID_1, scIdOut);
    data.FlushBuffer();
}

/**
 * @tc.name: UnregisterWriteToRawdata001
 * @tc.desc: Test UnregisterWriteToRawdata
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompServiceMockTest, UnregisterWriteToRawdata001, TestSize.Level1)
{
    secCompService_->state_ = ServiceRunningState::STATE_RUNNING;
    secCompService_->Initialize();
    SecCompRawdata rawReply;
    EXPECT_EQ(SC_OK,
        secCompService_->UnregisterWriteToRawdata(SC_OK, rawReply));
}

/**
 * @tc.name: UnregisterSecurityComponent001
 * @tc.desc: Test UnregisterSecurityComponent fail
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompServiceMockTest, UnregisterSecurityComponent001, TestSize.Level1)
{
    secCompService_->state_ = ServiceRunningState::STATE_RUNNING;
    secCompService_->Initialize();
    MessageParcel data;

    // UnregisterReadFromRawdata fail
    SecCompRawdata rawdataVoid;
    rawdataVoid.size = 1;
    SecCompRawdata rawReplyVoid;
    MessageParcel replyVoid;
    EXPECT_EQ(SC_OK,
        secCompService_->UnregisterSecurityComponent(rawdataVoid, rawReplyVoid));
    ASSERT_TRUE(SecCompEnhanceAdapter::EnhanceSrvDeserialize(rawReplyVoid, replyVoid));
    EXPECT_EQ(SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL, replyVoid.ReadInt32());

    // UpdateSecurityComponentBody fail
    SecCompRawdata rawdataBodyFail;
    SecCompRawdata rawReplyBodyFail;
    MessageParcel replyBodyFail;
    data.WriteInt32(ServiceTestCommon::TEST_SC_ID_1);
    EXPECT_EQ(true, SecCompEnhanceAdapter::EnhanceSrvSerialize(data, rawdataBodyFail));
    EXPECT_EQ(SC_OK,
        secCompService_->UnregisterSecurityComponent(rawdataBodyFail, rawReplyBodyFail));
    ASSERT_TRUE(SecCompEnhanceAdapter::EnhanceSrvDeserialize(rawReplyBodyFail, replyBodyFail));
    EXPECT_EQ(SC_SERVICE_ERROR_COMPONENT_NOT_EXIST, replyBodyFail.ReadInt32());
}

/**
 * @tc.name: UnregisterSecurityComponent002
 * @tc.desc: Test UnregisterSecurityComponent OK
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompServiceMockTest, UnregisterSecurityComponent002, TestSize.Level1)
{
    secCompService_->state_ = ServiceRunningState::STATE_RUNNING;
    secCompService_->Initialize();
    MessageParcel data;
    nlohmann::json jsonRes;
    ServiceTestCommon::BuildLocationComponentJson(jsonRes);
    std::string locationInfo = jsonRes.dump();
    int32_t scId;
    EXPECT_EQ(SC_OK, secCompService_->RegisterSecurityComponentBody(
        LOCATION_COMPONENT, locationInfo, scId));

    SecCompRawdata rawdata;
    SecCompRawdata rawReply;
    MessageParcel reply;
    data.WriteInt32(scId);
    EXPECT_EQ(true, SecCompEnhanceAdapter::EnhanceSrvSerialize(data, rawdata));
    EXPECT_EQ(SC_OK,
        secCompService_->UnregisterSecurityComponent(rawdata, rawReply));
    ASSERT_TRUE(SecCompEnhanceAdapter::EnhanceSrvDeserialize(rawReply, reply));
    EXPECT_EQ(SC_OK, reply.ReadInt32());
}

/**
 * @tc.name: PreRegisterReadFromRawdata001
 * @tc.desc: Test PreRegisterReadFromRawdata
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompServiceMockTest, PreRegisterReadFromRawdata001, TestSize.Level1)
{
    secCompService_->state_ = ServiceRunningState::STATE_RUNNING;
    secCompService_->Initialize();
    MessageParcel data;

    // rawdata.data is nullptr
    SecCompRawdata rawdataVoid;
    rawdataVoid.size = 1;
    EXPECT_EQ(SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL,
        secCompService_->PreRegisterReadFromRawdata(rawdataVoid));

    // PreRegisterReadFromRawdata OK
    SecCompRawdata rawdata;
    EXPECT_EQ(true, SecCompEnhanceAdapter::EnhanceSrvSerialize(data, rawdata));
    EXPECT_EQ(SC_OK,
        secCompService_->PreRegisterReadFromRawdata(rawdata));
}

/**
 * @tc.name: PreRegisterWriteToRawdata001
 * @tc.desc: Test PreRegisterWriteToRawdata
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompServiceMockTest, PreRegisterWriteToRawdata001, TestSize.Level1)
{
    secCompService_->state_ = ServiceRunningState::STATE_RUNNING;
    secCompService_->Initialize();
    SecCompRawdata rawReply;
    EXPECT_EQ(SC_OK,
        secCompService_->PreRegisterWriteToRawdata(SC_OK, rawReply));
}

/**
 * @tc.name: PreRegisterSecCompProcess001
 * @tc.desc: Test PreRegisterSecCompProcess
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompServiceMockTest, PreRegisterSecCompProcess001, TestSize.Level1)
{
    secCompService_->state_ = ServiceRunningState::STATE_RUNNING;
    secCompService_->Initialize();
    MessageParcel data;

    // UnregisterReadFromRawdata fail
    SecCompRawdata rawdataVoid;
    rawdataVoid.size = 1;
    SecCompRawdata rawReplyVoid;
    MessageParcel replyVoid;
    EXPECT_EQ(SC_OK,
        secCompService_->PreRegisterSecCompProcess(rawdataVoid, rawReplyVoid));
    ASSERT_TRUE(SecCompEnhanceAdapter::EnhanceSrvDeserialize(rawReplyVoid, replyVoid));
    EXPECT_EQ(SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL, replyVoid.ReadInt32());

    // UnregisterReadFromRawdata OK
    SecCompRawdata rawdata;
    SecCompRawdata rawReply;
    MessageParcel reply;
    EXPECT_EQ(true, SecCompEnhanceAdapter::EnhanceSrvSerialize(data, rawdata));
    EXPECT_EQ(SC_OK,
        secCompService_->PreRegisterSecCompProcess(rawdata, rawReply));
    ASSERT_TRUE(SecCompEnhanceAdapter::EnhanceSrvDeserialize(rawReply, reply));
    EXPECT_EQ(SC_OK, reply.ReadInt32());
}

/**
 * @tc.name: ReportSecurityComponentClickEventBody001
 * @tc.desc: Test register security component
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompServiceMockTest, ReportSecurityComponentClickEventBody001, TestSize.Level1)
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
    SC_LOG_INFO(LABEL, "ReportSecurityComponentClickEventBody002");
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

/**
 * @tc.name: ReportSecurityComponentClickEvent001
 * @tc.desc: Test ReportSecurityComponentClickEvent read rawData fail
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompServiceMockTest, ReportSecurityComponentClickEvent001, TestSize.Level1)
{
    secCompService_->state_ = ServiceRunningState::STATE_RUNNING;
    secCompService_->Initialize();
    MessageParcel data;

    // rawdata.data is nullptr
    SecCompRawdata rawdataVoid;
    SecCompRawdata rawReplyVoid;
    MessageParcel replyVoid;
    EXPECT_EQ(SC_OK,
        secCompService_->ReportSecurityComponentClickEvent(nullptr, nullptr, rawdataVoid, rawReplyVoid));
    ASSERT_TRUE(SecCompEnhanceAdapter::EnhanceSrvDeserialize(rawReplyVoid, replyVoid));
    EXPECT_EQ(SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL, replyVoid.ReadInt32());

    // scId is invalid
    data.WriteInt32(-1);
    SecCompRawdata rawdataScidInvalid;
    SecCompRawdata rawReplyScidInvalid;
    MessageParcel replyScidInvalid;
    EXPECT_EQ(true, SecCompEnhanceAdapter::EnhanceSrvSerialize(data, rawdataScidInvalid));
    EXPECT_EQ(SC_OK,
        secCompService_->ReportSecurityComponentClickEvent(nullptr, nullptr, rawdataScidInvalid, rawReplyScidInvalid));
    ASSERT_TRUE(SecCompEnhanceAdapter::EnhanceSrvDeserialize(rawReplyScidInvalid, replyScidInvalid));
    EXPECT_EQ(SC_SERVICE_ERROR_VALUE_INVALID, replyScidInvalid.ReadInt32());
}

/**
 * @tc.name: ReportSecurityComponentClickEvent002
 * @tc.desc: Test ReportSecurityComponentClickEvent OK
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompServiceMockTest, ReportSecurityComponentClickEvent002, TestSize.Level1)
{
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

    MessageParcel data;
    data.WriteInt32(scId);
    data.WriteString(locationInfo);
    sptr<SecCompClickEventParcel> parcel = new (std::nothrow) SecCompClickEventParcel();
    ASSERT_NE(nullptr, parcel);
    parcel->clickInfoParams_ = clickInfo1;
    data.WriteParcelable(parcel);

    SecCompRawdata rawdata;
    SecCompRawdata rawReply;
    MessageParcel reply;
    EXPECT_EQ(true, SecCompEnhanceAdapter::EnhanceSrvSerialize(data, rawdata));
    EXPECT_EQ(SC_OK,
        secCompService_->ReportSecurityComponentClickEvent(nullptr, nullptr, rawdata, rawReply));
    ASSERT_TRUE(SecCompEnhanceAdapter::EnhanceSrvDeserialize(rawReply, reply));
    EXPECT_EQ(SC_OK, reply.ReadInt32());
}