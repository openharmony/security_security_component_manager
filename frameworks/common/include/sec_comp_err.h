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
#ifndef FRAMEWORKS_COMMON_SECURITY_COMPONENT_ERR_H
#define FRAMEWORKS_COMMON_SECURITY_COMPONENT_ERR_H

#include <inttypes.h>

namespace OHOS {
namespace Security {
namespace SecurityComponent {
enum SCErrCode : int32_t {
    SC_OK = 0,

    SC_SERVICE_ERROR_VALUE_INVALID = -50,
    SC_SERVICE_ERROR_PARCEL_OPERATE_FAIL = -51,
    SC_SERVICE_ERROR_MEMORY_OPERATE_FAIL = -52,
    SC_SERVICE_ERROR_IPC_REQUEST_FAIL = -54,
    SC_SERVICE_ERROR_SERVICE_NOT_EXIST = -55,
    SC_SERVICE_ERROR_COMPONENT_INFO_INVALID = -56,
    SC_SERVICE_ERROR_COMPONENT_RECT_OVERLAP = -57,
    SC_SERVICE_ERROR_COMPONENT_NOT_EXIST = -58,
    SC_SERVICE_ERROR_PERMISSION_OPER_FAIL = -59,
    SC_SERVICE_ERROR_CLICK_EVENT_INVALID = -60,
    SC_SERVICE_ERROR_COMPONENT_INFO_NOT_EQUAL = -61,
    SC_SERVICE_ERROR_CALLER_INVALID = -62,
    SC_SERVICE_ERROR_WAIT_FOR_DIALOG_CLOSE = -63,

    SC_ENHANCE_ERROR_NOT_EXIST_ENHANCE = -100,
    SC_ENHANCE_ERROR_VALUE_INVALID = -101,
    SC_ENHANCE_ERROR_OPER_FAIL = -102,
    SC_ENHANCE_ERROR_CALLBACK_REDIRECT = -103,
    SC_ENHANCE_ERROR_CALLBACK_REGIST_FAIL = -104,
    SC_ENHANCE_ERROR_CALLBACK_HAS_EXIST = -105,
    SC_ENHANCE_ERROR_CALLBACK_NOT_EXIST = -106,
    SC_ENHANCE_ERROR_CALLBACK_OPER_FAIL = -107,
    SC_ENHANCE_ERROR_CALLBACK_CHECK_FAIL = -108,
    SC_ENHANCE_ERROR_IN_MALICIOUS_LIST = -109,
    SC_ENHANCE_ERROR_CHALLENGE_CHECK_FAIL = -110,
    SC_ENHANCE_ERROR_CLICK_EXTRA_CHECK_FAIL = -111,
};
} // namespace SecurityComponent
} // namespace Security
} // namespace OHOS
#endif // FRAMEWORKS_COMMON_SECURITY_COMPONENT_ERR_H
