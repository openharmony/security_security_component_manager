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

#include "sec_comp_dialog_callback.h"
#include "sec_comp_log.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
    LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "SecCompDialogCallback"
};

void SecCompDialogCallback::OnDialogClosed(int32_t result)
{
    if (callback_ == nullptr) {
        SC_LOG_ERROR(LABEL, "callback_ is nullptr");
        return;
    }

    SC_LOG_DEBUG(LABEL, "OnDialogClosed call");
    callback_(result);
}
} // namespace SecurityComponent
} // namespace Security
} // namespace OHOS
