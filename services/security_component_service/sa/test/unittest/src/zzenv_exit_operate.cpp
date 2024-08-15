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

#include <gtest/gtest.h>
#define private public
#include "event_handler.h"
#include "event_runner.h"
#include "sec_comp_manager.h"
#undef private

using namespace testing::ext;

namespace OHOS {
namespace Security {
namespace SecurityComponent {
/**
 * @brief This test class is used to release resources before process end to avoid crash
 */
class ZZEnvExitOperate : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void ZZEnvExitOperate::SetUpTestCase()
{}

void ZZEnvExitOperate::TearDownTestCase()
{}

void ZZEnvExitOperate::SetUp()
{}

void ZZEnvExitOperate::TearDown()
{}

/**
 * @tc.name: ZExitOperateTest001
 * @tc.desc: remove ffrt handler to avoid uaf crash
 * @tc.type: FUNC
 * @tc.require: None
 */
HWTEST_F(ZZEnvExitOperate, ZExitOperateTest001, TestSize.Level1)
{
    if (SecCompManager::GetInstance().secRunner_ != nullptr) {
        SecCompManager::GetInstance().secRunner_->queue_.reset();
        SecCompManager::GetInstance().secRunner_ = nullptr;
    }
    if (SecCompManager::GetInstance().secHandler_ != nullptr) {
        SecCompManager::GetInstance().secHandler_->eventRunner_.reset();
        SecCompManager::GetInstance().secHandler_ = nullptr;
    }
    sleep(5);
}
}
}
}