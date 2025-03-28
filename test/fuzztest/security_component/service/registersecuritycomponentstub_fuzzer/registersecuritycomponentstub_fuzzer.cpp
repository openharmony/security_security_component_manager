/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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
#include "registersecuritycomponentstub_fuzzer.h"

#include <fcntl.h>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include "accesstoken_kit.h"
#include "fuzz_common.h"
#include "isec_comp_service.h"
#include "sec_comp_enhance_adapter.h"
#include "sec_comp_info.h"
#include "sec_comp_service.h"
#include "securec.h"
#include "token_setproc.h"
#include "sec_comp_log.h"

using namespace OHOS::Security::SecurityComponent;
using namespace OHOS::Security::AccessToken;
namespace OHOS {
static void RegisterSecurityComponentStubFuzzTest(const uint8_t *data, size_t size)
{
    uint32_t code =
        static_cast<uint32_t>(ISecCompServiceIpcCode::COMMAND_REGISTER_SECURITY_COMPONENT);
    MessageParcel rawData;
    MessageParcel input;
    SecCompRawdata inputData;
    MessageParcel reply;
    CompoRandomGenerator generator(data, size);

    if (!input.WriteInterfaceToken(ISecCompService::GetDescriptor())) {
        return;
    }
    uint32_t type = generator.GetScType();
    if (!rawData.WriteUint32(type)) {
        return;
    }

    std::string compoInfo = generator.GenerateRandomCompoStr(type);
    if (!rawData.WriteString(compoInfo)) {
        return;
    }
    SecCompEnhanceAdapter::EnhanceClientSerialize(rawData, inputData);
    input.WriteUint32(inputData.size);
    input.WriteRawData(inputData.data, inputData.size);
    MessageOption option(MessageOption::TF_SYNC);
    auto service =
        std::make_shared<SecCompService>(SA_ID_SECURITY_COMPONENT_SERVICE, true);
    service->OnRemoteRequest(code, input, reply, option);
    auto fd = open("/dev/null", O_WRONLY);
    std::vector<std::u16string>inputstr = {u""};
    service->Dump(fd, inputstr);
    close(fd);
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::RegisterSecurityComponentStubFuzzTest(data, size);
    return 0;
}
