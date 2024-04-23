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

#include "sec_comp_enhance_adapter_test.h"
#include <unistd.h>
#include "sec_comp_err.h"
#include "sec_comp_log.h"
#include "sec_comp_info.h"

using namespace testing::ext;
using namespace OHOS::Security::SecurityComponent;

namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
    LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "SecCompEnhanceAdapterTest"};
static constexpr uint32_t SEC_COMP_ENHANCE_CFG_SIZE = 76;
static constexpr uint32_t MAX_HMAC_SIZE = 64;
}  // namespace

void SecCompEnhanceAdapterTest::SetUpTestCase()
{
    SC_LOG_INFO(LABEL, "SetUpTestCase.");
}

void SecCompEnhanceAdapterTest::TearDownTestCase()
{
    SC_LOG_INFO(LABEL, "TearDownTestCase.");
}

void SecCompEnhanceAdapterTest::SetUp()
{
    SC_LOG_INFO(LABEL, "SetUp ok.");
}

void SecCompEnhanceAdapterTest::TearDown()
{
    SC_LOG_INFO(LABEL, "TearDown.");
}

/**
 * @tc.name: EnhanceAdapter001
 * @tc.desc: test enhance adapter fail
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompEnhanceAdapterTest, EnhanceAdapter001, TestSize.Level1)
{
    SecCompEnhanceAdapter::isEnhanceSrvHandlerInit = false;
    EXPECT_EQ(SecCompEnhanceAdapter::EnableInputEnhance(), SC_ENHANCE_ERROR_NOT_EXIST_ENHANCE);
    SecCompEnhanceAdapter::isEnhanceSrvHandlerInit = false;
    EXPECT_EQ(SecCompEnhanceAdapter::DisableInputEnhance(), SC_ENHANCE_ERROR_NOT_EXIST_ENHANCE);

    SecCompEnhanceAdapter::isEnhanceInputHandlerInit = false;
    uint8_t cfgData[SEC_COMP_ENHANCE_CFG_SIZE] = { 0 };
    EXPECT_EQ(SC_ENHANCE_ERROR_NOT_EXIST_ENHANCE,
        SecCompEnhanceAdapter::SetEnhanceCfg(cfgData, SEC_COMP_ENHANCE_CFG_SIZE));

    SecCompEnhanceAdapter::isEnhanceInputHandlerInit = false;
    uint8_t originData[MAX_HMAC_SIZE] = { 0 };
    uint32_t enHancedataLen = MAX_HMAC_SIZE;
    EXPECT_EQ(SC_ENHANCE_ERROR_NOT_EXIST_ENHANCE,
        SecCompEnhanceAdapter::GetPointerEventEnhanceData(originData, MAX_HMAC_SIZE, nullptr, enHancedataLen));

    SecCompEnhanceAdapter::isEnhanceInputHandlerInit = false;
    SecCompClickEvent touchInfo;
    ASSERT_EQ(SC_ENHANCE_ERROR_NOT_EXIST_ENHANCE, SecCompEnhanceAdapter::CheckExtraInfo(touchInfo));

    SecCompEnhanceAdapter::isEnhanceClientHandlerInit = false;
    std::string componentInfo;
    SecCompEnhanceAdapter::EnhanceDataPreprocess(1, componentInfo);

    SecCompEnhanceAdapter::isEnhanceSrvHandlerInit = false;
    ASSERT_EQ(nullptr, SecCompEnhanceAdapter::GetEnhanceRemoteObject());

    SecCompEnhanceAdapter::isEnhanceSrvHandlerInit = false;
    SecCompEnhanceAdapter::StartEnhanceService();
    SecCompEnhanceAdapter::isEnhanceSrvHandlerInit = false;
    SecCompEnhanceAdapter::ExitEnhanceService();
    SecCompEnhanceAdapter::isEnhanceSrvHandlerInit = false;
    SecCompEnhanceAdapter::NotifyProcessDied(0);
    SecCompEnhanceAdapter::isEnhanceClientHandlerInit = false;
    SecCompEnhanceAdapter::RegisterScIdEnhance(0);
    SecCompEnhanceAdapter::isEnhanceClientHandlerInit = false;
    SecCompEnhanceAdapter::UnregisterScIdEnhance(0);
    SecCompEnhanceAdapter::isEnhanceSrvHandlerInit = false;
    SecCompEnhanceAdapter::AddSecurityComponentProcess(0);
    SecCompEnhanceAdapter::isEnhanceSrvHandlerInit = false;

    OHOS::MessageParcel tmpData;
    OHOS::MessageParcel data;
    OHOS::MessageParcel reply;

    SecCompEnhanceAdapter::isEnhanceClientHandlerInit = false;
    SecCompEnhanceAdapter::EnhanceSerializeSessionInfo(tmpData, data);
    SecCompEnhanceAdapter::isEnhanceClientHandlerInit = false;
    SecCompEnhanceAdapter::EnhanceDeserializeSessionInfo(tmpData, data);
    SecCompEnhanceAdapter::isEnhanceSrvHandlerInit = false;
    SecCompEnhanceAdapter::SerializeSessionInfoEnhance(tmpData, data, 0);
    SecCompEnhanceAdapter::isEnhanceSrvHandlerInit = false;
    SecCompEnhanceAdapter::DeserializeSessionInfoEnhance(tmpData, data, reply, 0);
    std::shared_ptr<SecCompBase> compInfo;
    const nlohmann::json jsonComponent;
    ASSERT_EQ(SC_OK, SecCompEnhanceAdapter::CheckComponentInfoEnhance(0, compInfo, jsonComponent));
}
