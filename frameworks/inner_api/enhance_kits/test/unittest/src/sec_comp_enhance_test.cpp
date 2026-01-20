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
#include "sec_comp_enhance_test.h"
#include <unistd.h>
#include "sec_comp_err.h"
#include "sec_comp_log.h"
#include "sec_comp_info.h"

using namespace testing::ext;
using namespace OHOS::Security::SecurityComponent;

namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
    LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "SecCompEnhanceTest"};
static constexpr uint32_t SEC_COMP_ENHANCE_CFG_SIZE = 172;
static constexpr uint32_t MAX_HMAC_SIZE = 160;
}  // namespace

void SecCompEnhanceTest::SetUpTestCase()
{
    system("kill -9 `pidof security_component_service`");
    SC_LOG_INFO(LABEL, "SetUpTestCase.");
}

void SecCompEnhanceTest::TearDownTestCase()
{
    SC_LOG_INFO(LABEL, "TearDownTestCase.");
}

void SecCompEnhanceTest::SetUp()
{
    SC_LOG_INFO(LABEL, "SetUp ok.");
}

void SecCompEnhanceTest::TearDown()
{
    SC_LOG_INFO(LABEL, "TearDown.");
}

/**
 * @tc.name: SetEnhanceCfg001
 * @tc.desc: test SetEnhanceCfg
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompEnhanceTest, SetEnhanceCfg001, TestSize.Level0)
{
    uint8_t cfgData[SEC_COMP_ENHANCE_CFG_SIZE] = { 0 };
    int32_t result = SecCompEnhanceKit::SetEnhanceCfg(cfgData, SEC_COMP_ENHANCE_CFG_SIZE);
#ifdef SECURITY_COMPONENT_ENHANCE_ENABLE
        EXPECT_EQ(result, SC_OK);
#else
        EXPECT_EQ(result, SC_ENHANCE_ERROR_NOT_EXIST_ENHANCE);
#endif
}

/**
 * @tc.name: GetPoniterEventEnhanceData001
 * @tc.desc: test GetPoniterEventEnhanceData
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompEnhanceTest, GetPoniterEventEnhanceData001, TestSize.Level0)
{
    uint8_t originData[16] = { 0 };
    uint32_t dataLen = 16;
    uint8_t* enhanceData = nullptr;
    uint32_t enHancedataLen = MAX_HMAC_SIZE;

    InitSecCompClientEnhance();
    int32_t result = SecCompEnhanceKit::GetPointerEventEnhanceData(originData, dataLen, enhanceData, enHancedataLen);
#ifdef SECURITY_COMPONENT_ENHANCE_ENABLE
        EXPECT_EQ(result, SC_SERVICE_ERROR_SERVICE_NOT_EXIST);
#else
        EXPECT_EQ(result, SC_ENHANCE_ERROR_NOT_EXIST_ENHANCE);
#endif
}
