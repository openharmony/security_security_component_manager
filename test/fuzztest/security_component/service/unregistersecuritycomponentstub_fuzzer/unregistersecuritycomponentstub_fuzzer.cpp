/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include "accesstoken_kit.h"
#include "fuzz_common.h"
#include "isec_comp_service.h"
#include "sec_comp_enhance_adapter.h"
#include "sec_comp_info.h"
#include "sec_comp_service.h"
#include "securec.h"
#include "token_setproc.h"
#include "unregistersecuritycomponentstub_fuzzer.h"

using namespace OHOS::Security::SecurityComponent;
using namespace OHOS::Security::AccessToken;
namespace OHOS {
static int32_t RegisterSecurityComponentStub(uint32_t type, const std::string& compoInfo)
{
    uint32_t code =
        static_cast<uint32_t>(ISecCompServiceIpcCode::COMMAND_REGISTER_SECURITY_COMPONENT);
    MessageParcel rawData;
    MessageParcel input;
    SecCompRawdata inputData;
    MessageParcel reply;
    SecCompRawdata replyData;

    if (!input.WriteInterfaceToken(ISecCompService::GetDescriptor())) {
        return 0;
    }
    if (!rawData.WriteUint32(type)) {
        return 0;
    }

    if (!rawData.WriteString(compoInfo)) {
        return 0;
    }
    SecCompEnhanceAdapter::EnhanceClientSerialize(rawData, inputData);
    input.WriteUint32(inputData.size);
    input.WriteRawData(inputData.data, inputData.size);
    MessageOption option(MessageOption::TF_SYNC);
    auto service =
        std::make_shared<SecCompService>(SA_ID_SECURITY_COMPONENT_SERVICE, true);
    service->OnRemoteRequest(code, input, reply, option);
    if (!reply.ReadUint32(replyData.size)) {
        return 0;
    }
    auto readRawReply = reply.ReadRawData(replyData.size);
    if (readRawReply == nullptr) {
        return 0;
    }
    int32_t res = replyData.RawDataCpy(readRawReply);
    if (res != SC_OK) {
        return 0;
    }
    MessageParcel deserializedReply;
    SecCompEnhanceAdapter::EnhanceClientDeserialize(replyData, deserializedReply);
    if (!deserializedReply.ReadInt32(res)) {
        return 0;
    }
    int32_t scId = 0;
    if (!deserializedReply.ReadInt32(scId)) {
        return 0;
    }
    return scId;
}

static void UnregisterSecurityComponentStubFuzzTest(const uint8_t *data, size_t size)
{
    uint32_t code = static_cast<uint32_t>(ISecCompServiceIpcCode::COMMAND_UNREGISTER_SECURITY_COMPONENT);
    MessageParcel rawData;
    MessageParcel input;
    SecCompRawdata inputData;
    MessageParcel reply;
    CompoRandomGenerator generator(data, size);
    uint32_t type = generator.GetScType();
    std::string compoInfo = generator.GenerateRandomCompoStr(type);
    int32_t scId = RegisterSecurityComponentStub(type, compoInfo);
    if (!input.WriteInterfaceToken(ISecCompService::GetDescriptor())) {
        return;
    }
    if (!rawData.WriteInt32(scId)) {
        return;
    }
    SecCompEnhanceAdapter::EnhanceClientSerialize(rawData, inputData);
    input.WriteUint32(inputData.size);
    input.WriteRawData(inputData.data, inputData.size);

    MessageOption option(MessageOption::TF_SYNC);
    auto service = std::make_shared<SecCompService>(SA_ID_SECURITY_COMPONENT_SERVICE, true);
    service->OnRemoteRequest(code, input, reply, option);
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::UnregisterSecurityComponentStubFuzzTest(data, size);
    return 0;
}
