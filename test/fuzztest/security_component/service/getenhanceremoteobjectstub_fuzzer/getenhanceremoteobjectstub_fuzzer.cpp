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
#include "i_sec_comp_service.h"
#include "securec.h"
#include "sec_comp_enhance_adapter.h"
#include "sec_comp_service.h"
#include "token_setproc.h"
#include "getenhanceremoteobjectstub_fuzzer.h"

using namespace OHOS::Security::SecurityComponent;
using namespace OHOS::Security::AccessToken;
namespace OHOS {
static void GetEnhanceRemoteObjectStubFuzzTest(const uint8_t *data, size_t size)
{
    uint32_t code = SecurityComponentServiceInterfaceCode::GET_SECURITY_COMPONENT_ENHANCE_OBJECT;
    MessageParcel rawData;
    MessageParcel input;
    MessageParcel reply;

    if (!input.WriteInterfaceToken(ISecCompService::GetDescriptor())) {
        return;
    }
    SecCompEnhanceAdapter::EnhanceClientSerialize(rawData, input);
    MessageOption option(MessageOption::TF_SYNC);
    auto service = std::make_shared<SecCompService>(SA_ID_SECURITY_COMPONENT_SERVICE, true);
    service->OnRemoteRequest(code, input, reply, option);
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::GetEnhanceRemoteObjectStubFuzzTest(data, size);
    return 0;
}
