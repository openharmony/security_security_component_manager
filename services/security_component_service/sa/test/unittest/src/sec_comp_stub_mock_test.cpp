/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "sec_comp_stub_mock_test.h"

#include "sec_comp_log.h"
#include "sec_comp_err.h"
#include "sec_comp_click_event_parcel.h"
#include "service_test_common.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Security::SecurityComponent;

namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
    LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "SecCompStubMockTest"};
}

void SecCompStubMockTest::SetUpTestCase()
{}

void SecCompStubMockTest::TearDownTestCase()
{}

void SecCompStubMockTest::SetUp()
{
    SC_LOG_INFO(LABEL, "setup");
    if (stub_ != nullptr) {
        return;
    }

    stub_ = new (std::nothrow) SecCompStubMock();
    ASSERT_NE(nullptr, stub_);
}

void SecCompStubMockTest::TearDown()
{
    stub_ = nullptr;
}

/**
 * @tc.name: OnRemoteRequestMock001
 * @tc.desc: Test on remote request
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompStubMockTest, OnRemoteRequestMock001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(u"wrong");
    ASSERT_EQ(SC_SERVICE_ERROR_IPC_REQUEST_FAIL, stub_->OnRemoteRequest(static_cast<uint32_t>(
        SecurityComponentServiceInterfaceCode::REGISTER_SECURITY_COMPONENT), data, reply, option));
    data.FlushBuffer();
    reply.FlushBuffer();

    data.WriteInterfaceToken(u"ohos.security.ISecCompService");
    ASSERT_EQ(305, stub_->OnRemoteRequest(1000, data, reply, option));
}

/**
 * @tc.name: RegisterSecurityComponentInnerMock001
 * @tc.desc: Test register security component
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompStubMockTest, RegisterSecurityComponentInnerMock001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;

    ASSERT_EQ(SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL, stub_->RegisterSecurityComponentInner(data, reply));
    data.FlushBuffer();
    reply.FlushBuffer();

    data.WriteUint32(UNKNOWN_SC_TYPE);
    ASSERT_EQ(SC_SERVICE_ERROR_VALUE_INVALID, stub_->RegisterSecurityComponentInner(data, reply));
    data.FlushBuffer();
    reply.FlushBuffer();

    data.WriteUint32(MAX_SC_TYPE);
    ASSERT_EQ(SC_SERVICE_ERROR_VALUE_INVALID, stub_->RegisterSecurityComponentInner(data, reply));
    data.FlushBuffer();
    reply.FlushBuffer();

    data.WriteUint32(LOCATION_COMPONENT);
    ASSERT_EQ(SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL, stub_->RegisterSecurityComponentInner(data, reply));
    data.FlushBuffer();
    reply.FlushBuffer();

    data.WriteUint32(LOCATION_COMPONENT);
    data.WriteString("");
    ASSERT_EQ(SC_OK, stub_->RegisterSecurityComponentInner(data, reply));
}

/**
 * @tc.name: UpdateSecurityComponentInnerMock001
 * @tc.desc: Test update security component
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompStubMockTest, UpdateSecurityComponentInnerMock001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;

    ASSERT_EQ(SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL, stub_->UpdateSecurityComponentInner(data, reply));
    data.FlushBuffer();
    reply.FlushBuffer();

    data.WriteInt32(-1);
    ASSERT_EQ(SC_SERVICE_ERROR_VALUE_INVALID, stub_->UpdateSecurityComponentInner(data, reply));
    data.FlushBuffer();
    reply.FlushBuffer();

    data.WriteInt32(1);
    ASSERT_EQ(SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL, stub_->UpdateSecurityComponentInner(data, reply));
    data.FlushBuffer();
    reply.FlushBuffer();

    data.WriteInt32(1);
    data.WriteString("");
    ASSERT_EQ(SC_OK, stub_->UpdateSecurityComponentInner(data, reply));
}

/**
 * @tc.name: UnregisterSecurityComponentInnerMock001
 * @tc.desc: Test unregister security component
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompStubMockTest, UnregisterSecurityComponentInnerMock001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    ASSERT_EQ(SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL, stub_->UnregisterSecurityComponentInner(data, reply));
    data.FlushBuffer();
    reply.FlushBuffer();

    data.WriteInt32(-1);
    ASSERT_EQ(SC_SERVICE_ERROR_VALUE_INVALID, stub_->UnregisterSecurityComponentInner(data, reply));
    data.FlushBuffer();
    reply.FlushBuffer();

    data.WriteInt32(1);
    ASSERT_EQ(SC_OK, stub_->UnregisterSecurityComponentInner(data, reply));
}

/**
 * @tc.name: VerifySavePermissionInnerMock001
 * @tc.desc: Test VerifySavePermissionInner
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompStubMockTest, VerifySavePermissionInnerMock001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    setuid(0);
    ASSERT_TRUE(stub_->IsMediaLibraryCalling());
    ASSERT_EQ(SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL, stub_->VerifySavePermissionInner(data, reply));
    data.FlushBuffer();
    reply.FlushBuffer();
    data.WriteInt32(0);
    ASSERT_EQ(SC_SERVICE_ERROR_VALUE_INVALID, stub_->VerifySavePermissionInner(data, reply));
    data.FlushBuffer();
    reply.FlushBuffer();
    data.WriteInt32(1);
    ASSERT_EQ(SC_OK, stub_->VerifySavePermissionInner(data, reply));
    ASSERT_NE(SC_OK, stub_->GetEnhanceRemoteObjectInner(data, reply));
}

/**
 * @tc.name: MarshallingMock001
 * @tc.desc: Test SecCompClickEventParcel::Marshalling
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompStubMockTest, MarshallingMock001, TestSize.Level1)
{
    sptr<SecCompClickEventParcel> clickParcel = new (std::nothrow) SecCompClickEventParcel();
    Parcel out;
    EXPECT_FALSE(clickParcel->Marshalling(out));
    clickParcel->clickInfoParams_.type = ClickEventType::UNKNOWN_EVENT_TYPE;
    EXPECT_FALSE(clickParcel->Marshalling(out));

    clickParcel->clickInfoParams_.extraInfo.dataSize = 1;
    clickParcel->clickInfoParams_.type = ClickEventType::POINT_EVENT_TYPE;
    EXPECT_FALSE(clickParcel->Marshalling(out));
    clickParcel->clickInfoParams_.type = ClickEventType::KEY_EVENT_TYPE;
    EXPECT_FALSE(clickParcel->Marshalling(out));

    uint8_t data[32] = {0};
    clickParcel->clickInfoParams_.extraInfo.dataSize = 32;
    clickParcel->clickInfoParams_.extraInfo.data = data;
    EXPECT_TRUE(clickParcel->Marshalling(out));
}

/**
 * @tc.name: UnmarshallingMock001
 * @tc.desc: Test SecCompClickEventParcel::Unmarshalling
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompStubMockTest, UnmarshallingMock001, TestSize.Level1)
{
    sptr<SecCompClickEventParcel> clickParcel = new (std::nothrow) SecCompClickEventParcel();
    Parcel in;
    in.WriteInt32(1);
    EXPECT_EQ(nullptr, clickParcel->Unmarshalling(in));
    in.WriteInt32(2);
    EXPECT_EQ(nullptr, clickParcel->Unmarshalling(in));
    in.WriteInt32(0);
    EXPECT_EQ(nullptr, clickParcel->Unmarshalling(in));

    in.WriteInt32(2);
    in.WriteUint64(1);
    in.WriteInt32(1);
    int dataSize = MAX_EXTRA_SIZE + 1;
    in.WriteUint32(dataSize);
    EXPECT_EQ(nullptr, clickParcel->Unmarshalling(in));

    in.WriteInt32(2);
    in.WriteUint64(1);
    in.WriteInt32(1);
    in.WriteUint32(1);
    EXPECT_EQ(nullptr, clickParcel->Unmarshalling(in));

    in.WriteInt32(2);
    in.WriteUint64(1);
    in.WriteInt32(1);
    in.WriteUint32(32);
    uint8_t data[32] = {0};
    in.WriteBuffer(data, 32);
    EXPECT_NE(nullptr, clickParcel->Unmarshalling(in));
}
