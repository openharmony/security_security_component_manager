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
#include "reportsecuritycomponentclickevent_fuzzer.h"

using namespace OHOS::Security::SecurityComponent;
using namespace OHOS::Security::AccessToken;
namespace OHOS {
static void ReportSecurityComponentClickEventFuzzTest(const uint8_t *data, size_t size)
{
    std::string componentInfo(reinterpret_cast<const char *>(data), size);
    uint8_t *data1 = const_cast<uint8_t *>(data);
    const double touchX = size;
    const double touchY = size;
    const uint64_t timesStamp = size;
    struct SecCompClickEvent clickInfo;
    clickInfo.type = ClickEventType::POINT_EVENT_TYPE,
    clickInfo.point.touchX = touchX;
    clickInfo.point.touchY = touchY;
    clickInfo.point.timestamp = timesStamp;
    clickInfo.extraInfo.data = data1;
    clickInfo.extraInfo.dataSize = size;
    SecCompKit::ReportSecurityComponentClickEvent(size, componentInfo, clickInfo);
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::ReportSecurityComponentClickEventFuzzTest(data, size);
    return 0;
}
