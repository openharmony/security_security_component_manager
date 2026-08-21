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
#include <cstdlib>
#include <sys/wait.h>
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
static constexpr int32_t MAX_INIT_RETRY_TIMES = 3;
enum class ClientInitScenario : int32_t {
    DLSYM_FAILED = 0,
    INSTANCE_NULL,
    INSTANCE_VALID,
};

class MockClientEnhance final : public SecCompClientEnhanceInterface {
public:
    bool EnhanceDataPreprocess(const uintptr_t, std::string&) override
    {
        return true;
    }

    bool EnhanceDataPreprocess(const uintptr_t, int32_t, std::string&) override
    {
        return true;
    }

    bool EnhanceClientSerialize(const uintptr_t, OHOS::MessageParcel&, SecCompRawdata&) override
    {
        return true;
    }

    bool EnhanceClientDeserialize(const uintptr_t, SecCompRawdata&, OHOS::MessageParcel&) override
    {
        return true;
    }

    void RegisterScIdEnhance(const uintptr_t, int32_t) override
    {}

    void UnregisterScIdEnhance(const uintptr_t, int32_t) override
    {}

    void Update() override
    {}
};

std::atomic_bool g_dlopenSucceed = false;
std::atomic_int g_dlopenCallCount = 0;
std::atomic_int g_dlsymCallCount = 0;
std::atomic_int g_getClientInstanceCallCount = 0;
std::atomic<ClientInitScenario> g_clientInitScenario = ClientInitScenario::DLSYM_FAILED;
MockClientEnhance g_mockClientEnhance;
}  // namespace

extern "C" void* DlopenMock(const char*, int) __asm__("dlopen");
extern "C" void* DlsymMock(void*, const char*) __asm__("dlsym");
extern "C" SecCompClientEnhanceInterface* GetClientInstanceMock();

extern "C" void* DlopenMock(const char*, int)
{
    g_dlopenCallCount.fetch_add(1);
    if (g_dlopenSucceed.load()) {
        return &g_mockClientEnhance;
    }
    return nullptr;
}

extern "C" void* DlsymMock(void*, const char*)
{
    g_dlsymCallCount.fetch_add(1);
    if (g_clientInitScenario.load() == ClientInitScenario::DLSYM_FAILED) {
        return nullptr;
    }
    return reinterpret_cast<void*>(GetClientInstanceMock);
}

extern "C" SecCompClientEnhanceInterface* GetClientInstanceMock()
{
    g_getClientInstanceCallCount.fetch_add(1);
    if (g_clientInitScenario.load() == ClientInitScenario::INSTANCE_NULL) {
        return nullptr;
    }
    return &g_mockClientEnhance;
}

namespace {
bool RunClientInitScenario(ClientInitScenario scenario)
{
    pid_t pid = fork();
    if (pid < 0) {
        return false;
    }
    if (pid == 0) {
        g_clientInitScenario.store(scenario);
        g_dlopenSucceed.store(true);
        g_dlopenCallCount.store(0);
        g_dlsymCallCount.store(0);
        g_getClientInstanceCallCount.store(0);
        SecCompEnhanceAdapter::clientHandler = nullptr;
        SecCompEnhanceAdapter::isEnhanceClientHandlerInit = false;

        SecCompEnhanceAdapter::InitEnhanceHandler(SEC_COMP_ENHANCE_CLIENT_INTERFACE);
        SecCompEnhanceAdapter::InitEnhanceHandler(SEC_COMP_ENHANCE_CLIENT_INTERFACE);
        const bool isEnhanceEnabled = g_dlopenCallCount.load() > 0;
        bool result = SecCompEnhanceAdapter::isEnhanceClientHandlerInit;
        if (isEnhanceEnabled) {
            const int32_t expectedInstanceCalls = scenario == ClientInitScenario::DLSYM_FAILED ? 0 : 1;
            const bool expectValidInstance = scenario == ClientInitScenario::INSTANCE_VALID;
            result = result && g_dlopenCallCount.load() == 1 && g_dlsymCallCount.load() == 1 &&
                g_getClientInstanceCallCount.load() == expectedInstanceCalls &&
                (SecCompEnhanceAdapter::clientHandler != nullptr) == expectValidInstance;
            if (expectValidInstance) {
                std::string componentInfo;
                result = result && SecCompEnhanceAdapter::EnhanceDataPreprocess(componentInfo);
            }
        } else {
            result = result && g_dlsymCallCount.load() == 0 && g_getClientInstanceCallCount.load() == 0 &&
                SecCompEnhanceAdapter::clientHandler == nullptr;
        }

        if (scenario == ClientInitScenario::DLSYM_FAILED) {
            const int32_t dlopenCallCount = g_dlopenCallCount.load();
            SecCompEnhanceAdapter::inputHandler = nullptr;
            SecCompEnhanceAdapter::isEnhanceInputHandlerInit = false;
            SecCompEnhanceAdapter::InitEnhanceHandler(SEC_COMP_ENHANCE_INPUT_INTERFACE);
            SecCompEnhanceAdapter::InitEnhanceHandler(SEC_COMP_ENHANCE_INPUT_INTERFACE);
            const int32_t expectedDlopenCalls = dlopenCallCount + (isEnhanceEnabled ? 1 : 0);
            result = result && SecCompEnhanceAdapter::isEnhanceInputHandlerInit &&
                g_dlopenCallCount.load() == expectedDlopenCalls;
        }
        std::exit(result ? EXIT_SUCCESS : EXIT_FAILURE);
    }

    int32_t status = 0;
    if (waitpid(pid, &status, 0) != pid) {
        return false;
    }
    return WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS;
}  // namespace
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
}

void SecCompEnhanceAdapterTest::TearDown()
{
    SC_LOG_INFO(LABEL, "TearDown.");
}

/**
 * @tc.name: InitEnhanceHandler_001
 * @tc.desc: symbol failure stops initialization and dlopen failure retries at most three times
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompEnhanceAdapterTest, InitEnhanceHandler_001, TestSize.Level0)
{
    constexpr size_t threadCount = 4;
    g_dlopenCallCount.store(0);
    SecCompEnhanceAdapter::InitEnhanceHandler(static_cast<EnhanceInterfaceType>(-1));
    EXPECT_EQ(0, g_dlopenCallCount.load());

    EXPECT_TRUE(RunClientInitScenario(ClientInitScenario::DLSYM_FAILED));
    EXPECT_TRUE(RunClientInitScenario(ClientInitScenario::INSTANCE_NULL));
    EXPECT_TRUE(RunClientInitScenario(ClientInitScenario::INSTANCE_VALID));

    g_dlopenCallCount.store(0);
    g_dlopenSucceed.store(false);
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
    const bool isEnhanceEnabled = g_dlopenCallCount.load() > 0;
    EXPECT_EQ(isEnhanceEnabled ? MAX_INIT_RETRY_TIMES : 0, g_dlopenCallCount.load());
    EXPECT_TRUE(SecCompEnhanceAdapter::isEnhanceSrvHandlerInit);
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
