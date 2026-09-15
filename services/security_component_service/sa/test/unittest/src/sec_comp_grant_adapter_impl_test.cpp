/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include "sec_comp_grant_adapter_impl_test.h"

#include "ability_manager_client.h"
#include "display_manager_lite.h"
#include "sec_comp_info.h"
#include "sec_comp_log.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Security::SecurityComponent;

namespace {
class MockRemoteObject : public IRemoteObject {
public:
    explicit MockRemoteObject(std::u16string descriptor) : IRemoteObject(descriptor)
    {
    }

    ~MockRemoteObject() = default;

    bool IsProxyObject() const override
    {
        return false;
    }

    int32_t GetObjectRefCount() override
    {
        return 0;
    }

    int Dump(int fd, const std::vector<std::u16string>& args) override
    {
        return 0;
    }

    int SendRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) override
    {
        return -1;
    }

    bool AddDeathRecipient(const sptr<DeathRecipient>& recipient) override
    {
        return false;
    }

    bool RemoveDeathRecipient(const sptr<DeathRecipient>& recipient) override
    {
        return false;
    }
};
}

// factory exported by sec_comp_grant_adapter_impl.cpp which is linked into this test
extern "C" OHOS::Security::SecurityComponent::SecCompGrantInterface* GetGrantAdapterInstance(void);

namespace OHOS {
namespace Security {
namespace SecurityComponent {
namespace {
SecCompGrantInterface* GetImpl()
{
    return GetGrantAdapterInstance();
}
}

void SecCompGrantAdapterImplTest::SetUpTestCase()
{
}

void SecCompGrantAdapterImplTest::TearDownTestCase()
{
}

void SecCompGrantAdapterImplTest::SetUp()
{
    AAFwk::AbilityManagerClient::GetInstance()->ResetRecord();
}

void SecCompGrantAdapterImplTest::TearDown()
{
    AAFwk::AbilityManagerClient::GetInstance()->ResetRecord();
}

/*
 * @tc.name: GetDisplaySize001
 * @tc.desc: GetDisplaySize returns mock screen size and non round shape
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompGrantAdapterImplTest, GetDisplaySize001, TestSize.Level1)
{
    SecCompGrantInterface* impl = GetImpl();
    ASSERT_NE(nullptr, impl);
    int32_t width = 0;
    int32_t height = 0;
    bool isRoundScreen = true;
    // mock display: 1500x1500 rectangle, crossAxisState keeps physical height branch off
    EXPECT_TRUE(impl->GetDisplaySize(0, static_cast<int32_t>(CrossAxisState::STATE_NO_CROSS),
        width, height, isRoundScreen));
    EXPECT_EQ(1500, width);
    EXPECT_EQ(1500, height);
    EXPECT_FALSE(isRoundScreen);
    // second call keeps serving the same mock display info
    int32_t widthSecond = 0;
    int32_t heightSecond = 0;
    EXPECT_TRUE(impl->GetDisplaySize(0, static_cast<int32_t>(CrossAxisState::STATE_NO_CROSS),
        widthSecond, heightSecond, isRoundScreen));
    EXPECT_EQ(width, widthSecond);
    EXPECT_EQ(height, heightSecond);
}

/*
 * @tc.name: GetFoldCreaseBottomY001
 * @tc.desc: GetFoldCreaseBottomY returns false when mock crease rects are empty
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompGrantAdapterImplTest, GetFoldCreaseBottomY001, TestSize.Level1)
{
    SecCompGrantInterface* impl = GetImpl();
    ASSERT_NE(nullptr, impl);
    int32_t bottomY = -1;
    // mock display info returns empty crease rects: failure branch
    EXPECT_FALSE(impl->GetFoldCreaseBottomY(bottomY));
    // repeated calls stay on the failure branch without crash
    EXPECT_FALSE(impl->GetFoldCreaseBottomY(bottomY));
    // failure keeps the output untouched by implementation
    EXPECT_EQ(-1, bottomY);
}

/*
 * @tc.name: StartGrantAbilityDialog001
 * @tc.desc: dialog params drive the dialog branch and pass user id to ability manager
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompGrantAdapterImplTest, StartGrantAbilityDialog001, TestSize.Level1)
{
    SecCompGrantInterface* impl = GetImpl();
    ASSERT_NE(nullptr, impl);
    SecCompGrantParams params;
    params.typeNum = 1;
    params.callerUid = 3057;
    params.userId = 101;
    params.windowId = 3;
    params.displayId = 0;
    params.crossAxisState = static_cast<int32_t>(CrossAxisState::STATE_NO_CROSS);
    sptr<IRemoteObject> callerToken = new MockRemoteObject(std::u16string());
    sptr<IRemoteObject> srvCallback = new MockRemoteObject(std::u16string());
    // dialog branch: callback remote is passed through and start succeeds
    EXPECT_TRUE(impl->StartGrantAbility(params, callerToken, srvCallback));
    EXPECT_EQ(101, AAFwk::AbilityManagerClient::GetInstance()->GetLastUserId());
    EXPECT_EQ(callerToken, AAFwk::AbilityManagerClient::GetInstance()->GetLastCallerToken());
}
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS

/*
 * @tc.name: GetDisplaySizeDisplayNull001
 * @tc.desc: null display from manager drives the null display branch
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompGrantAdapterImplTest, GetDisplaySizeDisplayNull001, TestSize.Level1)
{
    SecCompGrantInterface* impl = GetImpl();
    ASSERT_NE(nullptr, impl);
    Rosen::DisplayManagerLite::SetDisplayMode(Rosen::DisplayManagerLite::DISPLAY_NULL);
    int32_t width = 100;
    int32_t height = 100;
    bool isRoundScreen = false;
    // null display branch: query fails and outputs keep untouched
    EXPECT_FALSE(impl->GetDisplaySize(0, static_cast<int32_t>(CrossAxisState::STATE_NO_CROSS),
        width, height, isRoundScreen));
    EXPECT_EQ(100, width);
    EXPECT_EQ(100, height);
    Rosen::DisplayManagerLite::ResetMock();
}

/*
 * @tc.name: GetDisplaySizeInfoNull001
 * @tc.desc: null display info drives the null info branch
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompGrantAdapterImplTest, GetDisplaySizeInfoNull001, TestSize.Level1)
{
    SecCompGrantInterface* impl = GetImpl();
    ASSERT_NE(nullptr, impl);
    Rosen::DisplayManagerLite::SetDisplayMode(Rosen::DisplayManagerLite::DISPLAY_INFO_NULL);
    int32_t width = 200;
    int32_t height = 200;
    bool isRoundScreen = false;
    // null display info branch: query fails
    EXPECT_FALSE(impl->GetDisplaySize(0, static_cast<int32_t>(CrossAxisState::STATE_NO_CROSS),
        width, height, isRoundScreen));
    EXPECT_EQ(200, width);
    EXPECT_EQ(200, height);
    Rosen::DisplayManagerLite::ResetMock();
}

/*
 * @tc.name: GetDisplaySizeCross001
 * @tc.desc: cross axis state drives the physical height branch
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompGrantAdapterImplTest, GetDisplaySizeCross001, TestSize.Level1)
{
    SecCompGrantInterface* impl = GetImpl();
    ASSERT_NE(nullptr, impl);
    int32_t width = 0;
    int32_t height = 0;
    bool isRoundScreen = false;
    // cross state: mock physical height equals normal height, both branches return 1500
    EXPECT_TRUE(impl->GetDisplaySize(0, static_cast<int32_t>(CrossAxisState::STATE_CROSS),
        width, height, isRoundScreen));
    EXPECT_EQ(1500, width);
    EXPECT_EQ(1500, height);
    // dialog start with cross state also drives the physical height branch inside SetDisplayParams
    SecCompGrantParams params;
    params.typeNum = 1;
    params.userId = 100;
    params.windowId = 3;
    params.displayId = 0;
    params.crossAxisState = static_cast<int32_t>(CrossAxisState::STATE_CROSS);
    sptr<IRemoteObject> callerToken = new MockRemoteObject(std::u16string());
    sptr<IRemoteObject> srvCallback = new MockRemoteObject(std::u16string());
    EXPECT_TRUE(impl->StartGrantAbility(params, callerToken, srvCallback));
    Rosen::DisplayManagerLite::ResetMock();
}

/*
 * @tc.name: GetFoldCreaseBottomYNull001
 * @tc.desc: null crease region drives the null region branch
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompGrantAdapterImplTest, GetFoldCreaseBottomYNull001, TestSize.Level1)
{
    SecCompGrantInterface* impl = GetImpl();
    ASSERT_NE(nullptr, impl);
    Rosen::DisplayManagerLite::SetCreaseMode(Rosen::DisplayManagerLite::CREASE_NULL);
    int32_t bottomY = -1;
    // null region branch: query fails
    EXPECT_FALSE(impl->GetFoldCreaseBottomY(bottomY));
    EXPECT_EQ(-1, bottomY);
    Rosen::DisplayManagerLite::ResetMock();
}

/*
 * @tc.name: GetFoldCreaseBottomYValid001
 * @tc.desc: valid crease mode returns empty rects and fails on the empty branch
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompGrantAdapterImplTest, GetFoldCreaseBottomYValid001, TestSize.Level1)
{
    SecCompGrantInterface* impl = GetImpl();
    ASSERT_NE(nullptr, impl);
    Rosen::DisplayManagerLite::SetCreaseMode(Rosen::DisplayManagerLite::CREASE_VALID);
    int32_t bottomY = -1;
    // valid mode still returns empty rects from the mock: empty branch
    EXPECT_FALSE(impl->GetFoldCreaseBottomY(bottomY));
    EXPECT_EQ(-1, bottomY);
    Rosen::DisplayManagerLite::ResetMock();
}

/*
 * @tc.name: StartGrantAbilityDisplayFail001
 * @tc.desc: null display makes StartGrantAbility fail through SetDisplayParams
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompGrantAdapterImplTest, StartGrantAbilityDisplayFail001, TestSize.Level1)
{
    SecCompGrantInterface* impl = GetImpl();
    ASSERT_NE(nullptr, impl);
    Rosen::DisplayManagerLite::SetDisplayMode(Rosen::DisplayManagerLite::DISPLAY_NULL);
    SecCompGrantParams params;
    params.typeNum = 1;
    params.userId = 100;
    params.windowId = 3;
    params.displayId = 0;
    params.crossAxisState = static_cast<int32_t>(CrossAxisState::STATE_NO_CROSS);
    sptr<IRemoteObject> callerToken = new MockRemoteObject(std::u16string());
    sptr<IRemoteObject> srvCallback = new MockRemoteObject(std::u16string());
    // SetDisplayParams fails inside: ability must not be started
    EXPECT_FALSE(impl->StartGrantAbility(params, callerToken, srvCallback));
    EXPECT_EQ(-1, AAFwk::AbilityManagerClient::GetInstance()->GetLastUserId());
    Rosen::DisplayManagerLite::ResetMock();
}
