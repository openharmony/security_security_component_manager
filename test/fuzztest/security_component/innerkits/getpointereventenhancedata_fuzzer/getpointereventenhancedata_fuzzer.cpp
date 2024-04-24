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
#include "securec.h"
#include "token_setproc.h"
#include "getpointereventenhancedata_fuzzer.h"

using namespace OHOS::Security::SecurityComponent;
using namespace OHOS::Security::AccessToken;
namespace OHOS {
static void GetPointerEventEnhanceDataFuzzTest(const uint8_t *data, size_t size)
{
    uint8_t *data1 = const_cast<uint8_t *>(data);
    uint8_t  enhanceData[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    uint32_t enhancedataLen = 10;
    SecCompEnhanceKit::GetPointerEventEnhanceData(data1, size, enhanceData, enhancedataLen);
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::GetPointerEventEnhanceDataFuzzTest(data, size);
    return 0;
}
