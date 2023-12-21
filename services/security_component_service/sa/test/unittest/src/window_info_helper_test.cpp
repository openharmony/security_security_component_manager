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
HWTEST_F(WindowInfoHelperTest, CheckOtherWindowCoverComp001, TestSize.Level1)
{
    WindowManager::GetInstance().result_ = WMError::WM_OK;
    std::vector<sptr<AccessibilityWindowInfo>> list;
    sptr<AccessibilityWindowInfo> compWin = new AccessibilityWindowInfo();
    compWin->wid_ = 0;
    compWin->layer_ = 1;
    compWin->scaleVal_ = 1.0; // window did not scale
    list.emplace_back(compWin);
    list.emplace_back(nullptr);

    sptr<AccessibilityWindowInfo> downNotCrossWin = new AccessibilityWindowInfo();
    downNotCrossWin->wid_ = 1;
    downNotCrossWin->layer_ = 0;
    downNotCrossWin->scaleVal_ = 0.0;
    downNotCrossWin->windowRect_ = Rosen::Rect{ 0, 0, 0, 0 };
    list.emplace_back(downNotCrossWin);

    sptr<AccessibilityWindowInfo> downCrossWin = new AccessibilityWindowInfo();
    downCrossWin->wid_ = 1;
    downCrossWin->layer_ = 0;
    downCrossWin->scaleVal_ = 0.9; // window is scaled to 90%
    downCrossWin->windowRect_ = Rosen::Rect {
        ServiceTestCommon::TEST_COORDINATE, ServiceTestCommon::TEST_COORDINATE,
        ServiceTestCommon::TEST_COORDINATE, ServiceTestCommon::TEST_COORDINATE
    };
    list.emplace_back(downCrossWin);

    sptr<AccessibilityWindowInfo> upWin = new AccessibilityWindowInfo();
    upWin->wid_ = 1;
    upWin->layer_ = 2; // layer larger than comp window
    upWin->scaleVal_ = 1.0; // window did not scale
    upWin->windowRect_ = Rosen::Rect {
        ServiceTestCommon::TEST_COORDINATE, ServiceTestCommon::TEST_COORDINATE,
        ServiceTestCommon::TEST_COORDINATE, ServiceTestCommon::TEST_COORDINATE
    };
    list.emplace_back(upWin);
    WindowManager::GetInstance().list_ = list;

    SecCompRect compRect = {
        ServiceTestCommon::TEST_COORDINATE, ServiceTestCommon::TEST_COORDINATE,
        ServiceTestCommon::TEST_COORDINATE, ServiceTestCommon::TEST_COORDINATE
    };

    ASSERT_FALSE(WindowInfoHelper::CheckOtherWindowCoverComp(0, compRect));
}

/**
 * @tc.name: CheckOtherWindowCoverComp002
 * @tc.desc: Test component window is not found in list
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WindowInfoHelperTest, CheckOtherWindowCoverComp002, TestSize.Level1)
{
    WindowManager::GetInstance().result_ = WMError::WM_OK;
    std::vector<sptr<AccessibilityWindowInfo>> list;
    sptr<AccessibilityWindowInfo> compWin = new AccessibilityWindowInfo();
    compWin->wid_ = 1;
    compWin->layer_ = 1;
    compWin->scaleVal_ = 1.0;
    list.emplace_back(compWin);
    WindowManager::GetInstance().list_ = list;

    SecCompRect compRect = {
        ServiceTestCommon::TEST_COORDINATE, ServiceTestCommon::TEST_COORDINATE,
        ServiceTestCommon::TEST_COORDINATE, ServiceTestCommon::TEST_COORDINATE
    };
    ASSERT_FALSE(WindowInfoHelper::CheckOtherWindowCoverComp(0, compRect));
}

/**
 * @tc.name: CheckOtherWindowCoverComp003
 * @tc.desc: Test component window is not covered
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WindowInfoHelperTest, CheckOtherWindowCoverComp003, TestSize.Level1)
{
    WindowManager::GetInstance().result_ = WMError::WM_OK;
    std::vector<sptr<AccessibilityWindowInfo>> list;
    sptr<AccessibilityWindowInfo> compWin = new AccessibilityWindowInfo();
    compWin->wid_ = 0;
    compWin->layer_ = 1;
    compWin->scaleVal_ = 1.0;
    list.emplace_back(compWin);
    WindowManager::GetInstance().list_ = list;

    SecCompRect compRect = {
        ServiceTestCommon::TEST_COORDINATE, ServiceTestCommon::TEST_COORDINATE,
        ServiceTestCommon::TEST_COORDINATE, ServiceTestCommon::TEST_COORDINATE
    };
    ASSERT_TRUE(WindowInfoHelper::CheckOtherWindowCoverComp(0, compRect));
}
