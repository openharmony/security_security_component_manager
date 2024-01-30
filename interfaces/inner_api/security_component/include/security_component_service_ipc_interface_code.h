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

#ifndef SECURITY_COMPONENT_IPC_INTERFACE_CODE_H
#define SECURITY_COMPONENT_IPC_INTERFACE_CODE_H

/* SAID: 3506 */
namespace OHOS {
namespace Security {
namespace SecurityComponent {
enum SecurityComponentServiceInterfaceCode {
    REGISTER_SECURITY_COMPONENT = 0,
    UPDATE_SECURITY_COMPONENT = 1,
    UNREGISTER_SECURITY_COMPONENT = 2,
    REPORT_SECURITY_COMPONENT_CLICK_EVENT = 3,
    GET_SECURITY_COMPONENT_ENHANCE_OBJECT = 4,
    VERIFY_TEMP_SAVE_PERMISSION = 5,
    PRE_REGISTER_PROCESS = 6,
};

enum EnhanceInterfaceCode {
    REGISTER_CHECK_CALLBACK = 0,
};

enum EnhanceCallbackCheckCode {
    GET_CALLER_SECURITY_COMPONENT_INFO = 0,
    SET_WINDOW_ENHANCE_CFG = 1,
};
} // namespace SecurityComponent
} // namespace Security
} // namespace OHOS
#endif // SECURITY_COMPONENT_IPC_INTERFACE_CODE_H
