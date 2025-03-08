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
#include "sec_comp_ui_register.h"

#include "sec_comp_caller_authorization.h"
#include "sec_comp_log.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
    LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "SecCompUiRegister"};
}  // namespace

ISecCompProbe* SecCompUiRegister::callbackProbe = nullptr;

SecCompUiRegister::SecCompUiRegister(std::vector<uintptr_t>& callerList, ISecCompProbe* probe)
{
    SC_LOG_INFO(LABEL, "Init");
    SecCompCallerAuthorization::GetInstance().RegisterSecCompKitCaller(callerList);
    callbackProbe = probe;
}
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS

