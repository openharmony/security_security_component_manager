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

#ifndef SEC_COMP_STUB_MOCK_TEST_H
#define SEC_COMP_STUB_MOCK_TEST_H

#include <gtest/gtest.h>
#define private public
#include "sec_comp_stub.h"
#undef private

namespace OHOS {
namespace Security {
namespace SecurityComponent {
// stub is abstract class
struct SecCompStubMock : public SecCompStub {
public:
    int32_t RegisterSecurityComponent(SecCompType type,
        const std::string& componentInfo, int32_t& scId) override
    {
        return 0;
    };

    int32_t UpdateSecurityComponent(int32_t scId, const std::string& componentInfo) override
    {
        return 0;
    };

    int32_t UnregisterSecurityComponent(int32_t scId) override
    {
        return 0;
    };

    int32_t ReportSecurityComponentClickEvent(int32_t scId, const std::string& componentInfo,
        const SecCompClickEvent& clickInfo, sptr<IRemoteObject> callerToken,
        sptr<IRemoteObject> dialogCall) override
    {
        return 0;
    };

    bool VerifySavePermission(AccessToken::AccessTokenID tokenId) override
    {
        return true;
    };

    sptr<IRemoteObject> GetEnhanceRemoteObject() override
    {
        return nullptr;
    };

    int32_t PreRegisterSecCompProcess() override
    {
        return 0;
    };
};

class SecCompStubMockTest : public testing::Test {
public:
    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp();

    void TearDown();

    sptr<SecCompStubMock> stub_ = nullptr;
};
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
#endif  // SEC_COMP_STUB_MOCK_TEST_H
