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

#include "sec_comp_stub_test.h"

#include "sec_comp_log.h"
#include "sec_comp_err.h"
#include "sec_comp_click_event_parcel.h"
#include "service_test_common.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Security::SecurityComponent;

namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
    LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "SecCompStubTest"};
}

void SecCompStubTest::SetUpTestCase()
{}

void SecCompStubTest::TearDownTestCase()
{}

void SecCompStubTest::SetUp()
{
    SC_LOG_INFO(LABEL, "setup");
    if (stub_ != nullptr) {
        return;
    }

    stub_ = new (std::nothrow) SecCompStubMock();
    ASSERT_NE(nullptr, stub_);
}

void SecCompStubTest::TearDown()
{
    stub_ = nullptr;
}

/**
 * @tc.name: OnRemoteRequest001
 * @tc.desc: Test on remote request
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompStubTest, OnRemoteRequest001, TestSize.Level0)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(u"wrong");
    ASSERT_EQ(ERR_TRANSACTION_FAILED, stub_->OnRemoteRequest(static_cast<uint32_t>(
        ISecCompServiceIpcCode::COMMAND_REGISTER_SECURITY_COMPONENT), data, reply, option));
    data.FlushBuffer();
    reply.FlushBuffer();

    data.WriteInterfaceToken(u"OHOS.Security.SecurityComponent.ISecCompService");
    ASSERT_EQ(305, stub_->OnRemoteRequest(1000, data, reply, option));
}

/**
 * @tc.name: RegisterSecurityComponentInner001
 * @tc.desc: Test register security component
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompStubTest, RegisterSecurityComponentInner001, TestSize.Level0)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    data.WriteInterfaceToken(u"OHOS.Security.SecurityComponent.ISecCompService");
    ASSERT_EQ(ERR_INVALID_DATA, stub_->OnRemoteRequest(static_cast<uint32_t>(
        ISecCompServiceIpcCode::COMMAND_REGISTER_SECURITY_COMPONENT), data, reply, option));
}

/**
 * @tc.name: UpdateSecurityComponentInner001
 * @tc.desc: Test update security component
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompStubTest, UpdateSecurityComponentInner001, TestSize.Level0)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    data.WriteInterfaceToken(u"OHOS.Security.SecurityComponent.ISecCompService");
    ASSERT_EQ(ERR_INVALID_DATA, stub_->OnRemoteRequest(static_cast<uint32_t>(
        ISecCompServiceIpcCode::COMMAND_UPDATE_SECURITY_COMPONENT), data, reply, option));
}

/**
 * @tc.name: UnregisterSecurityComponentInner001
 * @tc.desc: Test unregister security component
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompStubTest, UnregisterSecurityComponentInner001, TestSize.Level0)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    data.WriteInterfaceToken(u"OHOS.Security.SecurityComponent.ISecCompService");
    ASSERT_EQ(ERR_INVALID_DATA, stub_->OnRemoteRequest(static_cast<uint32_t>(
        ISecCompServiceIpcCode::COMMAND_UNREGISTER_SECURITY_COMPONENT), data, reply, option));
}

/**
 * @tc.name: Marshalling001
 * @tc.desc: Test SecCompClickEventParcel::Marshalling
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompStubTest, Marshalling001, TestSize.Level0)
{
    sptr<SecCompClickEventParcel> clickParcel = new (std::nothrow) SecCompClickEventParcel();
    ASSERT_NE(nullptr, clickParcel);
    Parcel out;
    EXPECT_FALSE(clickParcel->Marshalling(out));
    clickParcel->clickInfoParams_.type = ClickEventType::UNKNOWN_EVENT_TYPE;
    EXPECT_FALSE(clickParcel->Marshalling(out));

    clickParcel->clickInfoParams_.extraInfo.dataSize = 1;
    clickParcel->clickInfoParams_.type = ClickEventType::POINT_EVENT_TYPE;
    EXPECT_FALSE(clickParcel->Marshalling(out));
    clickParcel->clickInfoParams_.type = ClickEventType::KEY_EVENT_TYPE;
    EXPECT_FALSE(clickParcel->Marshalling(out));
    clickParcel->clickInfoParams_.type = ClickEventType::ACCESSIBILITY_EVENT_TYPE;
    EXPECT_FALSE(clickParcel->Marshalling(out));

    uint8_t data[32] = {0};
    clickParcel->clickInfoParams_.extraInfo.dataSize = 32;
    clickParcel->clickInfoParams_.extraInfo.data = data;
    EXPECT_TRUE(clickParcel->Marshalling(out));
}

/**
 * @tc.name: Unmarshalling001
 * @tc.desc: Test SecCompClickEventParcel::Unmarshalling for type
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompStubTest, Unmarshalling001, TestSize.Level0)
{
    sptr<SecCompClickEventParcel> clickParcel = new (std::nothrow) SecCompClickEventParcel();
    ASSERT_NE(nullptr, clickParcel);
    Parcel in;
    EXPECT_EQ(nullptr, clickParcel->Unmarshalling(in));

    in.FlushBuffer();
    in.WriteInt32(0); // 0: ClickEventType::UNKNOWN_EVENT_TYPE
    EXPECT_EQ(nullptr, clickParcel->Unmarshalling(in));

    in.FlushBuffer();
    in.WriteInt32(1); // 1: ClickEventType::POINT_EVENT_TYPE
    EXPECT_EQ(nullptr, clickParcel->Unmarshalling(in));

    in.FlushBuffer();
    in.WriteInt32(1); // 1: ClickEventType::POINT_EVENT_TYPE
    in.WriteDouble(1.0);
    EXPECT_EQ(nullptr, clickParcel->Unmarshalling(in));

    in.FlushBuffer();
    in.WriteInt32(1); // 1: ClickEventType::POINT_EVENT_TYPE
    in.WriteDouble(1.0);
    in.WriteDouble(1.0);
    EXPECT_EQ(nullptr, clickParcel->Unmarshalling(in));

    in.FlushBuffer();
    in.WriteInt32(1); // 1: ClickEventType::POINT_EVENT_TYPE
    in.WriteDouble(1.0);
    in.WriteDouble(1.0);
    in.WriteUint64(1);
    EXPECT_EQ(nullptr, clickParcel->Unmarshalling(in));

    in.FlushBuffer();
    in.WriteInt32(2); // 2: ClickEventType::KEY_EVENT_TYPE
    EXPECT_EQ(nullptr, clickParcel->Unmarshalling(in));

    in.FlushBuffer();
    in.WriteInt32(2); // ClickEventType::KEY_EVENT_TYPE
    in.WriteUint64(1);
    EXPECT_EQ(nullptr, clickParcel->Unmarshalling(in));

    in.FlushBuffer();
    in.WriteInt32(2); // 2: ClickEventType::KEY_EVENT_TYPE
    in.WriteUint64(1);
    in.WriteInt32(1);
    EXPECT_EQ(nullptr, clickParcel->Unmarshalling(in));

    in.FlushBuffer();
    in.WriteInt32(3); // 3: ClickEventType::ACCESSIBILITY_EVENT_TYPE
    EXPECT_EQ(nullptr, clickParcel->Unmarshalling(in));

    in.FlushBuffer();
    in.WriteInt32(3); // 3: ClickEventType::ACCESSIBILITY_EVENT_TYPE
    in.WriteInt64(1);
    EXPECT_EQ(nullptr, clickParcel->Unmarshalling(in));

    in.FlushBuffer();
    in.WriteInt32(3); // 3: ClickEventType::ACCESSIBILITY_EVENT_TYPE
    in.WriteInt64(1);
    in.WriteInt64(1);
    EXPECT_EQ(nullptr, clickParcel->Unmarshalling(in));
}

/**
 * @tc.name: Unmarshalling002
 * @tc.desc: Test SecCompClickEventParcel::Unmarshalling for POINT_EVENT_TYPE
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompStubTest, Unmarshalling002, TestSize.Level0)
{
    sptr<SecCompClickEventParcel> clickParcel = new (std::nothrow) SecCompClickEventParcel();
    ASSERT_NE(nullptr, clickParcel);
    Parcel in;
    in.WriteInt32(1); // 1: ClickEventType::POINT_EVENT_TYPE
    in.WriteDouble(1.0);
    in.WriteDouble(1.0);
    in.WriteUint64(1);
    in.WriteUint32(0);
    EXPECT_NE(nullptr, clickParcel->Unmarshalling(in));

    in.FlushBuffer();
    in.WriteInt32(1); // 1: ClickEventType::POINT_EVENT_TYPE
    in.WriteDouble(1.0);
    in.WriteDouble(1.0);
    in.WriteUint64(1);
    uint32_t dataSize = MAX_EXTRA_SIZE + 1;
    in.WriteUint32(dataSize);
    EXPECT_EQ(nullptr, clickParcel->Unmarshalling(in));

    in.FlushBuffer();
    in.WriteInt32(1); // 1: ClickEventType::POINT_EVENT_TYPE
    in.WriteDouble(1.0);
    in.WriteDouble(1.0);
    in.WriteUint64(1);
    in.WriteUint32(1);
    EXPECT_EQ(nullptr, clickParcel->Unmarshalling(in));

    in.FlushBuffer();
    in.WriteInt32(1); // 1: ClickEventType::POINT_EVENT_TYPE
    in.WriteDouble(1.0);
    in.WriteDouble(1.0);
    in.WriteUint64(1);
    in.WriteUint32(32); // dataLen is 32
    uint8_t data[32] = {0};
    in.WriteBuffer(data, 32); // the length of data is 32
    EXPECT_NE(nullptr, clickParcel->Unmarshalling(in));
}

/**
 * @tc.name: Unmarshalling003
 * @tc.desc: Test SecCompClickEventParcel::Unmarshalling for KEY_EVENT_TYPE
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompStubTest, Unmarshalling003, TestSize.Level0)
{
    sptr<SecCompClickEventParcel> clickParcel = new (std::nothrow) SecCompClickEventParcel();
    ASSERT_NE(nullptr, clickParcel);
    Parcel in;
    in.WriteInt32(2); // 2: ClickEventType::KEY_EVENT_TYPE
    in.WriteUint64(1);
    in.WriteInt32(1);
    in.WriteUint32(0);
    EXPECT_NE(nullptr, clickParcel->Unmarshalling(in));

    in.FlushBuffer();
    in.WriteInt32(2); // 2: ClickEventType::KEY_EVENT_TYPE
    in.WriteUint64(1);
    in.WriteInt32(1);
    uint32_t dataSize = MAX_EXTRA_SIZE + 1;
    in.WriteUint32(dataSize);
    EXPECT_EQ(nullptr, clickParcel->Unmarshalling(in));

    in.FlushBuffer();
    in.WriteInt32(2); // 2: ClickEventType::KEY_EVENT_TYPE
    in.WriteUint64(1);
    in.WriteInt32(1);
    in.WriteUint32(1);
    EXPECT_EQ(nullptr, clickParcel->Unmarshalling(in));

    in.FlushBuffer();
    in.WriteInt32(2); // 2: ClickEventType::KEY_EVENT_TYPE
    in.WriteUint64(1);
    in.WriteInt32(1);
    in.WriteUint32(32); // dataLen is 32
    uint8_t data[32] = {0};
    in.WriteBuffer(data, 32); // the length of data is 32
    EXPECT_NE(nullptr, clickParcel->Unmarshalling(in));
}

/**
 * @tc.name: Unmarshalling004
 * @tc.desc: Test SecCompClickEventParcel::Unmarshalling for ACCESSIBILITY_EVENT_TYPE
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompStubTest, Unmarshalling004, TestSize.Level0)
{
    sptr<SecCompClickEventParcel> clickParcel = new (std::nothrow) SecCompClickEventParcel();
    ASSERT_NE(nullptr, clickParcel);
    Parcel in;
    in.WriteInt32(3); // 3: ClickEventType::ACCESSIBILITY_EVENT_TYPE
    in.WriteInt64(1);
    in.WriteInt64(1);
    in.WriteUint32(0);
    EXPECT_NE(nullptr, clickParcel->Unmarshalling(in));

    in.FlushBuffer();
    in.WriteInt32(3); // 3: ClickEventType::ACCESSIBILITY_EVENT_TYPE
    in.WriteInt64(1);
    in.WriteInt64(1);
    uint32_t dataSize = MAX_EXTRA_SIZE + 1;
    in.WriteUint32(dataSize);
    EXPECT_EQ(nullptr, clickParcel->Unmarshalling(in));

    in.FlushBuffer();
    in.WriteInt32(3); // 3: ClickEventType::ACCESSIBILITY_EVENT_TYPE
    in.WriteInt64(1);
    in.WriteInt64(1);
    in.WriteUint32(1);
    EXPECT_EQ(nullptr, clickParcel->Unmarshalling(in));

    in.FlushBuffer();
    in.WriteInt32(3); // 3: ClickEventType::ACCESSIBILITY_EVENT_TYPE
    in.WriteInt64(1);
    in.WriteInt64(1);
    in.WriteUint32(32); // dataLen is 32
    uint8_t data[32] = {0};
    in.WriteBuffer(data, 32); // the length of data is 32
    EXPECT_NE(nullptr, clickParcel->Unmarshalling(in));
}
