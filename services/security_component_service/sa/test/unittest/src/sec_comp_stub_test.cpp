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
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SecCompStubTest, OnRemoteRequest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(u"wrong");
    ASSERT_EQ(SC_SERVICE_ERROR_IPC_REQUEST_FAIL, stub_->OnRemoteRequest(static_cast<uint32_t>(
        SecurityComponentServiceInterfaceCode::REGISTER_SECURITY_COMPONENT), data, reply, option));

    data.WriteInterfaceToken(u"ohos.security.ISecCompService");
    ASSERT_EQ(SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL, stub_->OnRemoteRequest(static_cast<uint32_t>(
        SecurityComponentServiceInterfaceCode::REGISTER_SECURITY_COMPONENT), data, reply, option));

    data.WriteInterfaceToken(u"ohos.security.ISecCompService");
    ASSERT_EQ(305, stub_->OnRemoteRequest(1000, data, reply, option));
}

/**
 * @tc.name: RegisterSecurityComponentInner001
 * @tc.desc: Test register security component
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SecCompStubTest, RegisterSecurityComponentInner001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;

    ASSERT_EQ(SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL, stub_->RegisterSecurityComponentInner(data, reply));

    data.WriteUint32(UNKNOWN_SC_TYPE);
    ASSERT_EQ(SC_SERVICE_ERROR_VALUE_INVALID, stub_->RegisterSecurityComponentInner(data, reply));

    data.WriteUint32(MAX_SC_TYPE);
    ASSERT_EQ(SC_SERVICE_ERROR_VALUE_INVALID, stub_->RegisterSecurityComponentInner(data, reply));

    data.WriteUint32(LOCATION_COMPONENT);
    ASSERT_EQ(SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL, stub_->RegisterSecurityComponentInner(data, reply));

    data.WriteUint32(LOCATION_COMPONENT);
    data.WriteString("");
    ASSERT_EQ(SC_OK, stub_->RegisterSecurityComponentInner(data, reply));
}

/**
 * @tc.name: UpdateSecurityComponentInner001
 * @tc.desc: Test update security component
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SecCompStubTest, UpdateSecurityComponentInner001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;

    ASSERT_EQ(SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL, stub_->UpdateSecurityComponentInner(data, reply));

    data.WriteInt32(-1);
    ASSERT_EQ(SC_SERVICE_ERROR_VALUE_INVALID, stub_->UpdateSecurityComponentInner(data, reply));

    data.WriteInt32(1);
    ASSERT_EQ(SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL, stub_->UpdateSecurityComponentInner(data, reply));

    data.WriteInt32(1);
    data.WriteString("");
    ASSERT_EQ(SC_OK, stub_->UpdateSecurityComponentInner(data, reply));
}

/**
 * @tc.name: UnregisterSecurityComponentInner001
 * @tc.desc: Test unregister security component
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SecCompStubTest, UnregisterSecurityComponentInner001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    ASSERT_EQ(SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL, stub_->UnregisterSecurityComponentInner(data, reply));

    data.WriteInt32(-1);
    ASSERT_EQ(SC_SERVICE_ERROR_VALUE_INVALID, stub_->UnregisterSecurityComponentInner(data, reply));

    data.WriteInt32(1);
    ASSERT_EQ(SC_OK, stub_->UnregisterSecurityComponentInner(data, reply));
}

/**
 * @tc.name: ReportSecurityComponentClickEventInner001
 * @tc.desc: Test report click event
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SecCompStubTest, ReportSecurityComponentClickEventInner001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    ASSERT_EQ(SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL, stub_->ReportSecurityComponentClickEventInner(data, reply));

    data.WriteInt32(-1);
    ASSERT_EQ(SC_SERVICE_ERROR_VALUE_INVALID, stub_->ReportSecurityComponentClickEventInner(data, reply));

    data.WriteInt32(1);
    ASSERT_EQ(SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL, stub_->ReportSecurityComponentClickEventInner(data, reply));

    data.WriteInt32(1);
    data.WriteString("");
    ASSERT_EQ(SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL, stub_->ReportSecurityComponentClickEventInner(data, reply));

    data.WriteInt32(1);
    data.WriteString("");
    SecCompClickEvent touchInfo;
    sptr<SecCompClickEventParcel> parcel = new (std::nothrow) SecCompClickEventParcel();
    parcel->touchInfoParams_ = touchInfo;
    data.WriteParcelable(parcel);
    ASSERT_EQ(SC_OK, stub_->ReportSecurityComponentClickEventInner(data, reply));
}

/**
 * @tc.name: VerifySavePermissionInner001
 * @tc.desc: Test VerifySavePermissionInner
 * @tc.type: FUNC
 * @tc.require: AR000HO9J7
 */
HWTEST_F(SecCompStubTest, VerifySavePermissionInner001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    ASSERT_TRUE(stub_->IsMediaLibraryCalling());
    ASSERT_EQ(SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL, stub_->VerifySavePermissionInner(data, reply));
    data.WriteInt32(0);
    ASSERT_EQ(SC_SERVICE_ERROR_VALUE_INVALID, stub_->VerifySavePermissionInner(data, reply));
    data.WriteInt32(1);
    ASSERT_EQ(SC_OK, stub_->VerifySavePermissionInner(data, reply));
    ASSERT_NE(SC_OK, stub_->GetEnhanceRemoteObjectInner(data, reply));
}
