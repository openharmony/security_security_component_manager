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

#include "sec_comp_dialog_callback_stub.h"

#include "sec_comp_err.h"
#include "sec_comp_log.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
    LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "SecCompDialogCallbackStub"
};
}

int32_t SecCompDialogCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    SC_LOG_DEBUG(LABEL, "Entry, code: 0x%{public}x", code);
    std::u16string descriptor = data.ReadInterfaceToken();
    if (descriptor != ISecCompDialogCallback::GetDescriptor()) {
        SC_LOG_ERROR(LABEL, "Get unexpect descriptor");
        return SC_SERVICE_ERROR_IPC_REQUEST_FAIL;
    }
    int32_t msgCode = static_cast<int32_t>(code);
    if (msgCode == ISecCompDialogCallback::ON_DIALOG_CLOSED) {
        int32_t res;
        if (!data.ReadInt32(res)) {
            SC_LOG_ERROR(LABEL, "Read result failed.");
            return SC_SERVICE_ERROR_IPC_REQUEST_FAIL;
        }

        OnDialogClosed(res);
    } else {
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return 0;
}
} // namespace SecurityComponent
} // namespace Security
} // namespace OHOS
