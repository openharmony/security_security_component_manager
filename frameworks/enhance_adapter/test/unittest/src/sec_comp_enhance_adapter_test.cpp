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

#include <atomic>
#include <thread>
#include <vector>

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
static constexpr uint32_t MAX_HMAC_SIZE = 160;

enum class DynamicLoadMode {
    OPEN_FAILURE,
    SYMBOL_FAILURE,
    NULL_CLIENT_INSTANCE,
    SUCCESS,
};

std::atomic<DynamicLoadMode> g_dynamicLoadMode = DynamicLoadMode::OPEN_FAILURE;
std::atomic_uint32_t g_dlcloseCount = 0;
uint8_t g_fakeLibraryHandle = 0;

class InputEnhanceMock final : public SecCompInputEnhanceInterface {
public:
    int32_t SetEnhanceCfg(uint8_t*, uint32_t) override
    {
        ++callCount_;
        return SC_OK;
    }

    int32_t GetPointerEventEnhanceData(void*, uint32_t, uint8_t*, uint32_t&) override
    {
        ++callCount_;
        return SC_OK;
    }

    uint32_t callCount_ = 0;
};

class SrvEnhanceMock final : public SecCompSrvEnhanceInterface {
public:
    int32_t EnableInputEnhance() override { return CountAndReturn(); }
    int32_t DisableInputEnhance() override { return CountAndReturn(); }
    int32_t CheckAndUpdateExtraInfo(SecCompClickEvent&) override { return CountAndReturn(); }
    int32_t CheckComponentInfoEnhance(int32_t, std::shared_ptr<SecCompBase>&,
        const nlohmann::json&) override { return CountAndReturn(); }
    void StartEnhanceService() override { ++callCount_; }
    void ExitEnhanceService() override { ++callCount_; }
    void NotifyProcessDied(int32_t) override { ++callCount_; }
    void AddSecurityComponentProcess(int32_t) override { ++callCount_; }
    bool IsBypassPermitted(const std::string&) override
    {
        ++callCount_;
        return true;
    }
    bool EnhanceSrvSerialize(OHOS::MessageParcel&, SecCompRawdata&) override
    {
        ++callCount_;
        return true;
    }
    bool EnhanceSrvDeserialize(SecCompRawdata&, OHOS::MessageParcel&) override
    {
        ++callCount_;
        return true;
    }

    uint32_t callCount_ = 0;

private:
    int32_t CountAndReturn()
    {
        ++callCount_;
        return SC_OK;
    }
};

class ClientEnhanceMock final : public SecCompClientEnhanceInterface {
public:
    bool EnhanceDataPreprocess(const uintptr_t, std::string&) override { return CountAndReturn(); }
    bool EnhanceDataPreprocess(const uintptr_t, int32_t, std::string&) override { return CountAndReturn(); }
    bool EnhanceClientSerialize(const uintptr_t, OHOS::MessageParcel&, SecCompRawdata&) override
    {
        return CountAndReturn();
    }
    bool EnhanceClientDeserialize(const uintptr_t, SecCompRawdata&, OHOS::MessageParcel&) override
    {
        return CountAndReturn();
    }
    void RegisterScIdEnhance(const uintptr_t, int32_t) override { ++callCount_; }
    void UnregisterScIdEnhance(const uintptr_t, int32_t) override { ++callCount_; }
    void Update() override {}

    uint32_t callCount_ = 0;

private:
    bool CountAndReturn()
    {
        ++callCount_;
        return true;
    }
};

InputEnhanceMock g_inputEnhanceMock;
SrvEnhanceMock g_srvEnhanceMock;
ClientEnhanceMock g_clientEnhanceMock;

SecCompClientEnhanceInterface* GetClientInstanceMock()
{
    if (g_dynamicLoadMode.load() == DynamicLoadMode::NULL_CLIENT_INSTANCE) {
        return nullptr;
    }
    return &g_clientEnhanceMock;
}

void VerifyClientEnhanceInitialization()
{
    g_dynamicLoadMode.store(DynamicLoadMode::SYMBOL_FAILURE);
    SecCompEnhanceAdapter::InitEnhanceHandler(SEC_COMP_ENHANCE_CLIENT_INTERFACE);
    EXPECT_EQ(1U, g_dlcloseCount.load());

    g_dynamicLoadMode.store(DynamicLoadMode::NULL_CLIENT_INSTANCE);
    SecCompEnhanceAdapter::InitEnhanceHandler(SEC_COMP_ENHANCE_CLIENT_INTERFACE);
    EXPECT_EQ(2U, g_dlcloseCount.load());

    g_dynamicLoadMode.store(DynamicLoadMode::SUCCESS);
    SecCompEnhanceAdapter::InitEnhanceHandler(SEC_COMP_ENHANCE_CLIENT_INTERFACE);
    std::string componentInfo;
    OHOS::MessageParcel parcel;
    SecCompRawdata rawData;
    EXPECT_TRUE(SecCompEnhanceAdapter::EnhanceDataPreprocess(componentInfo));
    EXPECT_TRUE(SecCompEnhanceAdapter::EnhanceDataPreprocess(1, componentInfo));
    EXPECT_TRUE(SecCompEnhanceAdapter::EnhanceClientSerialize(parcel, rawData));
    EXPECT_TRUE(SecCompEnhanceAdapter::EnhanceClientDeserialize(rawData, parcel));
    SecCompEnhanceAdapter::RegisterScIdEnhance(1);
    SecCompEnhanceAdapter::UnregisterScIdEnhance(1);
    EXPECT_EQ(6U, g_clientEnhanceMock.callCount_);
    SecCompEnhanceAdapter::InitEnhanceHandler(SEC_COMP_ENHANCE_CLIENT_INTERFACE);
}

void VerifyInputEnhanceInitialization()
{
    g_dynamicLoadMode.store(DynamicLoadMode::SUCCESS);
    SecCompEnhanceAdapter::inputHandler = &g_inputEnhanceMock;
    SecCompEnhanceAdapter::InitEnhanceHandler(SEC_COMP_ENHANCE_INPUT_INTERFACE);
    uint8_t cfgData[SEC_COMP_ENHANCE_CFG_SIZE] = { 0 };
    uint8_t enhanceData[MAX_HMAC_SIZE] = { 0 };
    uint32_t enhanceDataLen = MAX_HMAC_SIZE;
    EXPECT_EQ(SC_OK, SecCompEnhanceAdapter::SetEnhanceCfg(cfgData, SEC_COMP_ENHANCE_CFG_SIZE));
    EXPECT_EQ(SC_OK, SecCompEnhanceAdapter::GetPointerEventEnhanceData(
        cfgData, SEC_COMP_ENHANCE_CFG_SIZE, enhanceData, enhanceDataLen));
    EXPECT_EQ(2U, g_inputEnhanceMock.callCount_);
    SecCompEnhanceAdapter::InitEnhanceHandler(SEC_COMP_ENHANCE_INPUT_INTERFACE);
}

void VerifySrvEnhanceInitialization()
{
    g_dynamicLoadMode.store(DynamicLoadMode::SUCCESS);
    SecCompEnhanceAdapter::srvHandler = &g_srvEnhanceMock;
    SecCompEnhanceAdapter::InitEnhanceHandler(SEC_COMP_ENHANCE_SRV_INTERFACE);
    SecCompClickEvent clickInfo = {};
    EXPECT_EQ(SC_SERVICE_ERROR_CLICK_EVENT_INVALID, SecCompEnhanceAdapter::CheckAndUpdateExtraInfo(clickInfo));
    uint8_t extraInfo = 0;
    clickInfo.extraInfo.dataSize = sizeof(extraInfo);
    EXPECT_EQ(SC_SERVICE_ERROR_CLICK_EVENT_INVALID, SecCompEnhanceAdapter::CheckAndUpdateExtraInfo(clickInfo));
    clickInfo.extraInfo = { sizeof(extraInfo), &extraInfo };
    EXPECT_EQ(SC_OK, SecCompEnhanceAdapter::CheckAndUpdateExtraInfo(clickInfo));
    EXPECT_EQ(SC_OK, SecCompEnhanceAdapter::EnableInputEnhance());
    EXPECT_EQ(SC_OK, SecCompEnhanceAdapter::DisableInputEnhance());
    SecCompEnhanceAdapter::StartEnhanceService();
    SecCompEnhanceAdapter::ExitEnhanceService();
    SecCompEnhanceAdapter::NotifyProcessDied(1);
    SecCompEnhanceAdapter::AddSecurityComponentProcess(1);
    EXPECT_TRUE(SecCompEnhanceAdapter::IsBypassPermitted("test.bundle"));
    OHOS::MessageParcel parcel;
    SecCompRawdata rawData;
    EXPECT_TRUE(SecCompEnhanceAdapter::EnhanceSrvSerialize(parcel, rawData));
    EXPECT_TRUE(SecCompEnhanceAdapter::EnhanceSrvDeserialize(rawData, parcel));
    std::shared_ptr<SecCompBase> compInfo;
    EXPECT_EQ(SC_OK, SecCompEnhanceAdapter::CheckComponentInfoEnhance(1, compInfo, nlohmann::json {}));
    EXPECT_EQ(11U, g_srvEnhanceMock.callCount_);
    SecCompEnhanceAdapter::InitEnhanceHandler(SEC_COMP_ENHANCE_SRV_INTERFACE);
}
}  // namespace

extern "C" void* DlopenMock(const char*, int) __asm__("dlopen");
extern "C" void* DlsymMock(void*, const char*) __asm__("dlsym");
extern "C" int DlcloseMock(void*) __asm__("dlclose");

extern "C" void* DlopenMock(const char*, int)
{
    return g_dynamicLoadMode.load() == DynamicLoadMode::OPEN_FAILURE ? nullptr : &g_fakeLibraryHandle;
}

extern "C" void* DlsymMock(void*, const char*)
{
    if (g_dynamicLoadMode.load() == DynamicLoadMode::SYMBOL_FAILURE) {
        return nullptr;
    }
    return reinterpret_cast<void*>(GetClientInstanceMock);
}

extern "C" int DlcloseMock(void*)
{
    ++g_dlcloseCount;
    return 0;
}

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
    g_dynamicLoadMode.store(DynamicLoadMode::OPEN_FAILURE);
    g_dlcloseCount.store(0);
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
HWTEST_F(SecCompEnhanceAdapterTest, EnhanceAdapter001, TestSize.Level0)
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
    SecCompClickEvent touchInfo = {};
    ASSERT_EQ(SC_ENHANCE_ERROR_NOT_EXIST_ENHANCE, SecCompEnhanceAdapter::CheckAndUpdateExtraInfo(touchInfo));

    SecCompEnhanceAdapter::isEnhanceClientHandlerInit = false;
    std::string componentInfo;
    SecCompEnhanceAdapter::EnhanceDataPreprocess(1, componentInfo);

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
    ASSERT_FALSE(SecCompEnhanceAdapter::IsBypassPermitted("test.bundle"));
}

/**
 * @tc.name: EnhanceAdapter002
 * @tc.desc: test enhance adapter serialize and component check fallback
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompEnhanceAdapterTest, EnhanceAdapter002, TestSize.Level0)
{
    OHOS::MessageParcel input;
    OHOS::MessageParcel output;
    SecCompRawdata inputData;
    SecCompRawdata outputData;
    SecCompEnhanceAdapter::isEnhanceClientHandlerInit = false;
    SecCompEnhanceAdapter::EnhanceClientSerialize(input, outputData);
    SecCompEnhanceAdapter::isEnhanceClientHandlerInit = false;
    SecCompEnhanceAdapter::EnhanceClientDeserialize(inputData, output);
    SecCompEnhanceAdapter::isEnhanceSrvHandlerInit = false;
    SecCompEnhanceAdapter::EnhanceSrvSerialize(input, outputData);
    SecCompEnhanceAdapter::isEnhanceSrvHandlerInit = false;
    SecCompEnhanceAdapter::EnhanceSrvDeserialize(inputData, output);
    std::shared_ptr<SecCompBase> compInfo;
    const nlohmann::json jsonComponent;
    ASSERT_EQ(SC_OK, SecCompEnhanceAdapter::CheckComponentInfoEnhance(0, compInfo, jsonComponent));
}

/**
 * @tc.name: EnhanceAdapter003
 * @tc.desc: test concurrent service enhance initialization fallback
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompEnhanceAdapterTest, EnhanceAdapter003, TestSize.Level0)
{
    constexpr size_t threadCount = 4;
    std::atomic_bool hasUnexpectedResult = false;
    std::vector<std::thread> threads;
    threads.reserve(threadCount);

    for (size_t index = 0; index < threadCount; ++index) {
        threads.emplace_back([&hasUnexpectedResult]() {
            if (SecCompEnhanceAdapter::EnableInputEnhance() != SC_ENHANCE_ERROR_NOT_EXIST_ENHANCE) {
                hasUnexpectedResult.store(true);
            }
        });
    }
    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_FALSE(hasUnexpectedResult.load());
}

/**
 * @tc.name: EnhanceAdapter004
 * @tc.desc: test enhance handler initialization failures and success paths
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompEnhanceAdapterTest, EnhanceAdapter004, TestSize.Level0)
{
    SecCompEnhanceAdapter::InitEnhanceHandler(static_cast<EnhanceInterfaceType>(-1));
    VerifyClientEnhanceInitialization();
    VerifyInputEnhanceInitialization();
    VerifySrvEnhanceInitialization();
}
