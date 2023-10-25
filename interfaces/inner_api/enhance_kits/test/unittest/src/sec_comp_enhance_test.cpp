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
static bool g_inputEnhanceExist = false;
static bool g_srvEnhanceExist = false;
static constexpr uint32_t SEC_COMP_ENHANCE_CFG_SIZE = 76;
#ifdef _ARM64_
static const std::string LIB_PATH = "/system/lib64/";
#else
static const std::string LIB_PATH = "/system/lib/";
#endif
static const std::string ENHANCE_INPUT_INTERFACE_LIB = LIB_PATH + "libsec_comp_input_enhance.z.so";
static const std::string ENHANCE_SRV_INTERFACE_LIB = LIB_PATH + "libsec_comp_service_enhance.z.so";
static constexpr uint32_t MAX_HMAC_SIZE = 64;
}  // namespace

void SecCompEnhanceTest::SetUpTestCase()
{
    if (access(ENHANCE_INPUT_INTERFACE_LIB.c_str(), F_OK) == 0) {
        g_inputEnhanceExist = true;
    }

    if (access(ENHANCE_SRV_INTERFACE_LIB.c_str(), F_OK) == 0) {
        g_srvEnhanceExist = true;
    }
    system("kill -9 `pidof security_compon`");
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
 * @tc.require: AR000HO9IN
 */
HWTEST_F(SecCompEnhanceTest, SetEnhanceCfg001, TestSize.Level1)
{
    uint8_t cfgData[SEC_COMP_ENHANCE_CFG_SIZE] = { 0 };
    int32_t result = SecCompEnhanceKit::SetEnhanceCfg(cfgData, SEC_COMP_ENHANCE_CFG_SIZE);
    if (g_inputEnhanceExist) {
        EXPECT_EQ(result, SC_OK);
    } else {
        EXPECT_EQ(result, SC_ENHANCE_ERROR_NOT_EXIST_ENHANCE);
    }
}

/**
 * @tc.name: GetPoniterEventEnhanceData001
 * @tc.desc: test GetPoniterEventEnhanceData
 * @tc.type: FUNC
 * @tc.require: AR000HO9IN
 */
HWTEST_F(SecCompEnhanceTest, GetPoniterEventEnhanceData001, TestSize.Level1)
{
    uint8_t originData[16] = { 0 };
    uint32_t dataLen = 16;
    uint8_t* enhanceData = nullptr;
    uint32_t enHancedataLen = MAX_HMAC_SIZE;

    int32_t result = SecCompEnhanceKit::GetPointerEventEnhanceData(originData, dataLen, enhanceData, enHancedataLen);
    if (g_inputEnhanceExist) {
        EXPECT_EQ(result, SC_SERVICE_ERROR_SERVICE_NOT_EXIST);
    } else {
        EXPECT_EQ(result, SC_ENHANCE_ERROR_NOT_EXIST_ENHANCE);
    }
}

/**
 * @tc.name: EnableInputEnhance001
 * @tc.desc: test UnmarshallEnhanceCfg
 * @tc.type: FUNC
 * @tc.require: AR000HO9IN
 */
HWTEST_F(SecCompEnhanceTest, EnableInputEnhance001, TestSize.Level1)
{
    int32_t result = SecCompEnhanceAdapter::EnableInputEnhance();
    if (g_srvEnhanceExist) {
        EXPECT_EQ(result, SC_OK);
    } else {
        EXPECT_EQ(result, SC_ENHANCE_ERROR_NOT_EXIST_ENHANCE);
    }
}

/**
 * @tc.name: DisableInputEnhance001
 * @tc.desc: test DisableInputEnhance
 * @tc.type: FUNC
 * @tc.require: AR000HO9IN
 */
HWTEST_F(SecCompEnhanceTest, DisableInputEnhance001, TestSize.Level1)
{
    int32_t result = SecCompEnhanceAdapter::DisableInputEnhance();
    if (g_srvEnhanceExist) {
        EXPECT_EQ(result, SC_OK);
    } else {
        EXPECT_EQ(result, SC_ENHANCE_ERROR_NOT_EXIST_ENHANCE);
    }
}

/**
 * @tc.name: InitEnhanceHandler001
 * @tc.desc: test InitEnhanceHandler
 * @tc.type: FUNC
 * @tc.require: AR000HO9IN
 */
HWTEST_F(SecCompEnhanceTest, InitEnhanceHandler001, TestSize.Level1)
{
    SecCompEnhanceAdapter::isEnhanceInputHandlerInit = false;
    uint8_t originData[16] = { 0 };
    uint32_t dataLen = 16;
    uint8_t* enhanceData = nullptr;
    uint32_t enHancedataLen = MAX_HMAC_SIZE;
    ASSERT_NE(SC_OK,
        SecCompEnhanceAdapter::GetPointerEventEnhanceData(originData, dataLen, enhanceData, enHancedataLen));
    SecCompEnhanceAdapter::isEnhanceInputHandlerInit = false;
    SecCompClickEvent touchInfo;
    ASSERT_NE(SC_OK, SecCompEnhanceAdapter::CheckExtraInfo(touchInfo));
    SecCompEnhanceAdapter::isEnhanceInputHandlerInit = false;

    std::string componentInfo;
    int32_t scId = 1;
    SecCompEnhanceAdapter::EnhanceDataPreprocess(scId, componentInfo);
    SecCompEnhanceAdapter::isEnhanceInputHandlerInit = false;
    SecCompEnhanceAdapter::RegisterScIdEnhance(scId);
    SecCompEnhanceAdapter::isEnhanceInputHandlerInit = false;
    SecCompEnhanceAdapter::DisableInputEnhance();
    SecCompEnhanceAdapter::isEnhanceInputHandlerInit = false;
    SecCompEnhanceAdapter::StartEnhanceService();
    SecCompEnhanceAdapter::isEnhanceInputHandlerInit = false;
    SecCompEnhanceAdapter::ExistEnhanceService();
    SecCompEnhanceAdapter::isEnhanceInputHandlerInit = false;
    SecCompEnhanceAdapter::NotifyProcessDied(scId);
    SecCompEnhanceAdapter::isEnhanceInputHandlerInit = false;
    std::shared_ptr<SecCompBase> compInfo;
    const nlohmann::json jsonComponent;
    SecCompEnhanceAdapter::CheckComponentInfoEnhnace(scId, compInfo, jsonComponent);
    SecCompEnhanceAdapter::isEnhanceInputHandlerInit = false;
    SecCompEnhanceAdapter::GetEnhanceRemoteObject();
}

/**
 * @tc.name: InitEnhanceHandler002
 * @tc.desc: test InitEnhanceHandler
 * @tc.type: FUNC
 * @tc.require: AR000HO9IN
 */
HWTEST_F(SecCompEnhanceTest, InitEnhanceHandler002, TestSize.Level1)
{
    SecCompEnhanceAdapter::isEnhanceInputHandlerInit = true;
    uint8_t cfgData[SEC_COMP_ENHANCE_CFG_SIZE] = { 0 };
    SecCompEnhanceAdapter::inputHandler = nullptr;
    ASSERT_EQ(SC_ENHANCE_ERROR_NOT_EXIST_ENHANCE,
        SecCompEnhanceAdapter::SetEnhanceCfg(cfgData, SEC_COMP_ENHANCE_CFG_SIZE));
    uint8_t originData[16] = { 0 };
    uint32_t dataLen = 16;
    uint8_t* enhanceData = nullptr;
    uint32_t enHancedataLen = MAX_HMAC_SIZE;
    ASSERT_EQ(SC_ENHANCE_ERROR_NOT_EXIST_ENHANCE,
        SecCompEnhanceAdapter::GetPointerEventEnhanceData(originData, dataLen, enhanceData, enHancedataLen));
    SecCompClickEvent touchInfo;
    SecCompEnhanceAdapter::srvHandler = nullptr;
    ASSERT_EQ(SC_ENHANCE_ERROR_NOT_EXIST_ENHANCE, SecCompEnhanceAdapter::CheckExtraInfo(touchInfo));
    std::string componentInfo;
    int32_t scId = 1;
    SecCompEnhanceAdapter::clientHandler = nullptr;
    ASSERT_TRUE(SecCompEnhanceAdapter::EnhanceDataPreprocess(componentInfo));
    ASSERT_TRUE(SecCompEnhanceAdapter::EnhanceDataPreprocess(scId, componentInfo));
    SecCompEnhanceAdapter::RegisterScIdEnhance(scId);
    ASSERT_EQ(SecCompEnhanceAdapter::srvHandler, nullptr);
    ASSERT_EQ(SC_ENHANCE_ERROR_NOT_EXIST_ENHANCE, SecCompEnhanceAdapter::EnableInputEnhance());
    ASSERT_EQ(SC_ENHANCE_ERROR_NOT_EXIST_ENHANCE, SecCompEnhanceAdapter::DisableInputEnhance());
    SecCompEnhanceAdapter::StartEnhanceService();
    SecCompEnhanceAdapter::ExistEnhanceService();
    SecCompEnhanceAdapter::NotifyProcessDied(scId);
    std::shared_ptr<SecCompBase> compInfo;
    const nlohmann::json jsonComponent;
    ASSERT_EQ(SC_OK, SecCompEnhanceAdapter::CheckComponentInfoEnhnace(scId, compInfo, jsonComponent));
    ASSERT_EQ(nullptr, SecCompEnhanceAdapter::GetEnhanceRemoteObject());
}
