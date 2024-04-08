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

#include "sec_comp_dialog_callback_proxy.h"

#include "sec_comp_log.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
    LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "SecCompDialogCallbackProxy"
};
}

SecCompDialogCallbackProxy::SecCompDialogCallbackProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<ISecCompDialogCallback>(impl) {
}

SecCompDialogCallbackProxy::~SecCompDialogCallbackProxy()
{}

void SecCompDialogCallbackProxy::OnDialogClosed(int32_t result)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(ISecCompDialogCallback::GetDescriptor())) {
        SC_LOG_ERROR(LABEL, "Write descriptor failed.");
        return;
    }
    if (!data.WriteInt32(result)) {
        SC_LOG_ERROR(LABEL, "Write int32 failed.");
        return;
    }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        SC_LOG_ERROR(LABEL, "Remote service is null.");
        return;
    }
    int32_t requestResult = remote->SendRequest(
        static_cast<uint32_t>(ISecCompDialogCallback::ON_DIALOG_CLOSED), data, reply, option);
    if (requestResult != NO_ERROR) {
        SC_LOG_ERROR(LABEL, "send request fail, result: %{public}d", requestResult);
        return;
    }

    SC_LOG_INFO(LABEL, "SendRequest success");
}
} // namespace SecurityComponent
} // namespace Security
} // namespace OHOS
