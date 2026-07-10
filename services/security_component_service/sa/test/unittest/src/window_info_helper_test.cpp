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
#include <gtest/gtest.h>

#include "location_button.h"
#include "sec_comp_err.h"
#include "sec_comp_log.h"
#include "service_test_common.h"
#include "window_info_helper.h"
#include "window_manager.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Rosen;
using namespace OHOS::Security::SecurityComponent;

namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
    LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "WindowInfoHelperTest"};
}

namespace OHOS {
namespace Security {
namespace SecurityComponent {
class WindowInfoHelperTest : public testing::Test {
public:
    static void SetUpTestCase() {};

    static void TearDownTestCase() {};

    void SetUp()
    {
        SC_LOG_INFO(LABEL, "setup");
    };

    void TearDown() {};
};
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS


/**
 * @tc.name: CheckOtherWindowCoverComp001
 * @tc.desc: Test pinter event cross other window
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WindowInfoHelperTest, CheckOtherWindowCoverComp001, TestSize.Level0)
{
    WindowManager::GetInstance().result_ = WMError::WM_OK;
    std::vector<sptr<UnreliableWindowInfo>> list;
    sptr<UnreliableWindowInfo> compWin = new UnreliableWindowInfo();
    compWin->windowId_ = 0;
    compWin->zOrder_ = 1;
    compWin->floatingScale_ = 1.0; // window did not scale
    list.emplace_back(compWin);
    list.emplace_back(nullptr);

    sptr<UnreliableWindowInfo> downNotCrossWin = new UnreliableWindowInfo();
    downNotCrossWin->windowId_ = 1;
    downNotCrossWin->zOrder_ = 0;
    downNotCrossWin->floatingScale_ = 0.0;
    downNotCrossWin->windowRect_ = Rosen::Rect{ 0, 0, 0, 0 };
    list.emplace_back(downNotCrossWin);

    sptr<UnreliableWindowInfo> downCrossWin = new UnreliableWindowInfo();
    downCrossWin->windowId_ = 1;
    downCrossWin->zOrder_ = 0;
    downCrossWin->floatingScale_ = 0.9; // window is scaled to 90%
    downCrossWin->windowRect_ = Rosen::Rect {
        ServiceTestCommon::TEST_COORDINATE, ServiceTestCommon::TEST_COORDINATE,
        ServiceTestCommon::TEST_COORDINATE, ServiceTestCommon::TEST_COORDINATE
    };
    list.emplace_back(downCrossWin);

    sptr<UnreliableWindowInfo> upWin = new UnreliableWindowInfo();
    upWin->windowId_ = 1;
    upWin->zOrder_ = 2; // layer larger than comp window
    upWin->floatingScale_ = 1.0; // window did not scale
    upWin->windowRect_ = Rosen::Rect {
        ServiceTestCommon::TEST_COORDINATE, ServiceTestCommon::TEST_COORDINATE,
        ServiceTestCommon::TEST_COORDINATE, ServiceTestCommon::TEST_COORDINATE
    };
    list.emplace_back(upWin);
    WindowManager::GetInstance().info_ = list;

    SecCompRect compRect = {
        ServiceTestCommon::TEST_COORDINATE, ServiceTestCommon::TEST_COORDINATE,
        ServiceTestCommon::TEST_COORDINATE, ServiceTestCommon::TEST_COORDINATE
    };

    std::string message;
    ASSERT_FALSE(WindowInfoHelper::CheckOtherWindowCoverComp(0, compRect, message));
}

/**
 * @tc.name: CheckOtherWindowCoverComp002
 * @tc.desc: Test component window is not found in list
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WindowInfoHelperTest, CheckOtherWindowCoverComp002, TestSize.Level0)
{
    WindowManager::GetInstance().result_ = WMError::WM_OK;
    std::vector<sptr<UnreliableWindowInfo>> list;
    sptr<UnreliableWindowInfo> compWin = new UnreliableWindowInfo();
    compWin->windowId_ = 1;
    compWin->zOrder_ = 1;
    compWin->floatingScale_ = 1.0;
    list.emplace_back(compWin);
    WindowManager::GetInstance().info_ = list;

    SecCompRect compRect = {
        ServiceTestCommon::TEST_COORDINATE, ServiceTestCommon::TEST_COORDINATE,
        ServiceTestCommon::TEST_COORDINATE, ServiceTestCommon::TEST_COORDINATE
    };
    std::string message;
    ASSERT_FALSE(WindowInfoHelper::CheckOtherWindowCoverComp(0, compRect, message));
}

/**
 * @tc.name: CheckOtherWindowCoverComp003
 * @tc.desc: Test component window is not covered
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WindowInfoHelperTest, CheckOtherWindowCoverComp003, TestSize.Level0)
{
    WindowManager::GetInstance().result_ = WMError::WM_OK;
    std::vector<sptr<UnreliableWindowInfo>> list;
    sptr<UnreliableWindowInfo> compWin = new UnreliableWindowInfo();
    compWin->windowId_ = 0;
    compWin->zOrder_ = 1;
    compWin->floatingScale_ = 1.0;
    list.emplace_back(compWin);
    WindowManager::GetInstance().info_ = list;

    SecCompRect compRect = {
        ServiceTestCommon::TEST_COORDINATE, ServiceTestCommon::TEST_COORDINATE,
        ServiceTestCommon::TEST_COORDINATE, ServiceTestCommon::TEST_COORDINATE
    };
    std::string message;
    ASSERT_TRUE(WindowInfoHelper::CheckOtherWindowCoverComp(0, compRect, message));
}

/**
 * @tc.name: CheckOtherWindowCoverComp004
 * @tc.desc: Test component window is not covered
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WindowInfoHelperTest, CheckOtherWindowCoverComp004, TestSize.Level0)
{
    WindowManager::GetInstance().result_ = WMError::WM_OK;
    std::vector<sptr<UnreliableWindowInfo>> list;
    sptr<UnreliableWindowInfo> compWin = new UnreliableWindowInfo();
    compWin->windowId_ = 1;
    compWin->zOrder_ = 1;
    compWin->floatingScale_ = 1.0;
    compWin->windowRect_ = Rosen::Rect {
        0, 0,
        ServiceTestCommon::TEST_COORDINATE + 2, ServiceTestCommon::TEST_COORDINATE + 2
    };
    list.emplace_back(compWin);
    WindowManager::GetInstance().info_ = list;

    SecCompRect compRect = {
        ServiceTestCommon::TEST_COORDINATE, ServiceTestCommon::TEST_COORDINATE,
        ServiceTestCommon::TEST_COORDINATE, ServiceTestCommon::TEST_COORDINATE
    };
    compRect.borderRadius_ = {
        ServiceTestCommon::TEST_COORDINATE / 2, ServiceTestCommon::TEST_COORDINATE / 2,
        ServiceTestCommon::TEST_COORDINATE / 2, ServiceTestCommon::TEST_COORDINATE / 2
    };
    std::string message;
    ASSERT_FALSE(WindowInfoHelper::CheckOtherWindowCoverComp(0, compRect, message));
}

/**
 * @tc.name: CheckOtherWindowCoverComp005
 * @tc.desc: Test component window is not covered
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WindowInfoHelperTest, CheckOtherWindowCoverComp005, TestSize.Level0)
{
    WindowManager::GetInstance().result_ = WMError::WM_OK;
    std::vector<sptr<UnreliableWindowInfo>> list;
    sptr<UnreliableWindowInfo> compWin = new UnreliableWindowInfo();
    compWin->windowId_ = 1;
    compWin->zOrder_ = 1;
    compWin->floatingScale_ = 1.0;
    compWin->windowRect_ = Rosen::Rect {
        0, (ServiceTestCommon::TEST_COORDINATE * 2) - 2,
        ServiceTestCommon::TEST_COORDINATE + 2, ServiceTestCommon::TEST_COORDINATE + 2
    };
    list.emplace_back(compWin);
    WindowManager::GetInstance().info_ = list;

    SecCompRect compRect = {
        ServiceTestCommon::TEST_COORDINATE, ServiceTestCommon::TEST_COORDINATE,
        ServiceTestCommon::TEST_COORDINATE, ServiceTestCommon::TEST_COORDINATE
    };
    compRect.borderRadius_ = {
        ServiceTestCommon::TEST_COORDINATE / 2, ServiceTestCommon::TEST_COORDINATE / 2,
        ServiceTestCommon::TEST_COORDINATE / 2, ServiceTestCommon::TEST_COORDINATE / 2
    };
    std::string message;
    ASSERT_FALSE(WindowInfoHelper::CheckOtherWindowCoverComp(0, compRect, message));
}

/**
 * @tc.name: CheckOtherWindowCoverComp006
 * @tc.desc: Test component window is not covered
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WindowInfoHelperTest, CheckOtherWindowCoverComp006, TestSize.Level0)
{
    WindowManager::GetInstance().result_ = WMError::WM_OK;
    std::vector<sptr<UnreliableWindowInfo>> list;
    sptr<UnreliableWindowInfo> compWin = new UnreliableWindowInfo();
    compWin->windowId_ = 1;
    compWin->zOrder_ = 1;
    compWin->floatingScale_ = 1.0;
    compWin->windowRect_ = Rosen::Rect {
        (ServiceTestCommon::TEST_COORDINATE * 2) - 2, 0,
        ServiceTestCommon::TEST_COORDINATE + 2, ServiceTestCommon::TEST_COORDINATE + 2
    };
    list.emplace_back(compWin);
    WindowManager::GetInstance().info_ = list;

    SecCompRect compRect = {
        ServiceTestCommon::TEST_COORDINATE, ServiceTestCommon::TEST_COORDINATE,
        ServiceTestCommon::TEST_COORDINATE, ServiceTestCommon::TEST_COORDINATE
    };
    compRect.borderRadius_ = {
        ServiceTestCommon::TEST_COORDINATE / 2, ServiceTestCommon::TEST_COORDINATE / 2,
        ServiceTestCommon::TEST_COORDINATE / 2, ServiceTestCommon::TEST_COORDINATE / 2
    };
    std::string message;
    ASSERT_FALSE(WindowInfoHelper::CheckOtherWindowCoverComp(0, compRect, message));
}

/**
 * @tc.name: CheckOtherWindowCoverComp007
 * @tc.desc: Test component window is not covered
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WindowInfoHelperTest, CheckOtherWindowCoverComp007, TestSize.Level0)
{
    WindowManager::GetInstance().result_ = WMError::WM_OK;
    std::vector<sptr<UnreliableWindowInfo>> list;
    sptr<UnreliableWindowInfo> compWin = new UnreliableWindowInfo();
    compWin->windowId_ = 1;
    compWin->zOrder_ = 1;
    compWin->floatingScale_ = 1.0;
    compWin->windowRect_ = Rosen::Rect {
        (ServiceTestCommon::TEST_COORDINATE * 2) - 2, (ServiceTestCommon::TEST_COORDINATE * 2) - 2,
        ServiceTestCommon::TEST_COORDINATE + 2, ServiceTestCommon::TEST_COORDINATE + 2
    };
    list.emplace_back(compWin);
    WindowManager::GetInstance().info_ = list;

    SecCompRect compRect = {
        ServiceTestCommon::TEST_COORDINATE, ServiceTestCommon::TEST_COORDINATE,
        ServiceTestCommon::TEST_COORDINATE, ServiceTestCommon::TEST_COORDINATE
    };
    compRect.borderRadius_ = {
        ServiceTestCommon::TEST_COORDINATE / 2, ServiceTestCommon::TEST_COORDINATE / 2,
        ServiceTestCommon::TEST_COORDINATE / 2, ServiceTestCommon::TEST_COORDINATE / 2
    };
    std::string message;
    ASSERT_FALSE(WindowInfoHelper::CheckOtherWindowCoverComp(0, compRect, message));
}

/**
 * @tc.name: TryGetWindowInfo001
 * @tc.desc: Test TryGetWindowInfo with normal windowId match
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WindowInfoHelperTest, TryGetWindowInfo001, TestSize.Level0)
{
    WindowManager::GetInstance().result_ = WMError::WM_OK;
    std::vector<sptr<AccessibilityWindowInfo>> list;
    sptr<AccessibilityWindowInfo> win1 = new AccessibilityWindowInfo();
    win1->wid_ = 0;
    list.emplace_back(win1);

    sptr<AccessibilityWindowInfo> win2 = new AccessibilityWindowInfo();
    win2->wid_ = 1;
    list.emplace_back(win2);
    WindowManager::GetInstance().list_ = list;

    sptr<AccessibilityWindowInfo> windowInfo;
    ASSERT_TRUE(WindowInfoHelper::TryGetWindowInfo(0, windowInfo));
    ASSERT_NE(nullptr, windowInfo);
    EXPECT_EQ(0, windowInfo->wid_);

    ASSERT_TRUE(WindowInfoHelper::TryGetWindowInfo(1, windowInfo));
    ASSERT_NE(nullptr, windowInfo);
    EXPECT_EQ(1, windowInfo->wid_);
}

/**
 * @tc.name: TryGetWindowInfo002
 * @tc.desc: Test TryGetWindowInfo with smart edge windowId match (wid_ == 1 && windowId == innerWid_)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WindowInfoHelperTest, TryGetWindowInfo002, TestSize.Level0)
{
    WindowManager::GetInstance().result_ = WMError::WM_OK;
    std::vector<sptr<AccessibilityWindowInfo>> list;
    sptr<AccessibilityWindowInfo> smartEdgeWin = new AccessibilityWindowInfo();
    smartEdgeWin->wid_ = 1;
    smartEdgeWin->innerWid_ = 100;
    list.emplace_back(smartEdgeWin);
    WindowManager::GetInstance().list_ = list;

    sptr<AccessibilityWindowInfo> windowInfo;
    ASSERT_TRUE(WindowInfoHelper::TryGetWindowInfo(100, windowInfo));
    ASSERT_NE(nullptr, windowInfo);
    EXPECT_EQ(1, windowInfo->wid_);
    EXPECT_EQ(100, windowInfo->innerWid_);
}

/**
 * @tc.name: TryGetWindowInfo003
 * @tc.desc: Test TryGetWindowInfo with smart edge windowId not match
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WindowInfoHelperTest, TryGetWindowInfo003, TestSize.Level0)
{
    WindowManager::GetInstance().result_ = WMError::WM_OK;
    std::vector<sptr<AccessibilityWindowInfo>> list;
    sptr<AccessibilityWindowInfo> smartEdgeWin = new AccessibilityWindowInfo();
    smartEdgeWin->wid_ = 1;
    smartEdgeWin->innerWid_ = 100;
    list.emplace_back(smartEdgeWin);
    WindowManager::GetInstance().list_ = list;

    sptr<AccessibilityWindowInfo> windowInfo;
    ASSERT_FALSE(WindowInfoHelper::TryGetWindowInfo(101, windowInfo));
    ASSERT_EQ(nullptr, windowInfo);
}

/**
 * @tc.name: TryGetWindowInfo004
 * @tc.desc: Test TryGetWindowInfo with mixed windows (normal and smart edge)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WindowInfoHelperTest, TryGetWindowInfo004, TestSize.Level0)
{
    WindowManager::GetInstance().result_ = WMError::WM_OK;
    std::vector<sptr<AccessibilityWindowInfo>> list;
    sptr<AccessibilityWindowInfo> normalWin = new AccessibilityWindowInfo();
    normalWin->wid_ = 0;
    list.emplace_back(normalWin);

    sptr<AccessibilityWindowInfo> smartEdgeWin = new AccessibilityWindowInfo();
    smartEdgeWin->wid_ = 1;
    smartEdgeWin->innerWid_ = 100;
    list.emplace_back(smartEdgeWin);
    WindowManager::GetInstance().list_ = list;

    sptr<AccessibilityWindowInfo> windowInfo;
    ASSERT_TRUE(WindowInfoHelper::TryGetWindowInfo(0, windowInfo));
    ASSERT_NE(nullptr, windowInfo);
    EXPECT_EQ(0, windowInfo->wid_);

    ASSERT_TRUE(WindowInfoHelper::TryGetWindowInfo(100, windowInfo));
    ASSERT_NE(nullptr, windowInfo);
    EXPECT_EQ(1, windowInfo->wid_);
    EXPECT_EQ(100, windowInfo->innerWid_);

    ASSERT_FALSE(WindowInfoHelper::TryGetWindowInfo(999, windowInfo));
}

/**
 * @tc.name: TryGetWindowInfo005
 * @tc.desc: Test null window info is skipped
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WindowInfoHelperTest, TryGetWindowInfo005, TestSize.Level0)
{
    auto oldResult = WindowManager::GetInstance().result_;
    auto oldList = WindowManager::GetInstance().list_;
    WindowManager::GetInstance().result_ = WMError::WM_OK;
    std::vector<sptr<AccessibilityWindowInfo>> list;
    list.emplace_back(nullptr);
    sptr<AccessibilityWindowInfo> compWin = new AccessibilityWindowInfo();
    compWin->wid_ = 1;
    list.emplace_back(compWin);
    WindowManager::GetInstance().list_ = list;

    sptr<AccessibilityWindowInfo> windowInfo = nullptr;
    EXPECT_TRUE(WindowInfoHelper::TryGetWindowInfo(1, windowInfo));
    EXPECT_NE(nullptr, windowInfo);
    if (windowInfo != nullptr) {
        EXPECT_EQ(1, windowInfo->wid_);
    }
    WindowManager::GetInstance().list_ = oldList;
    WindowManager::GetInstance().result_ = oldResult;
}
