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

#ifndef SEC_COMP_DIALOG_CALLBACK_PROXY_H
#define SEC_COMP_DIALOG_CALLBACK_PROXY_H

#include "i_sec_comp_dialog_callback.h"

#include "iremote_proxy.h"
#include "nocopyable.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
class SecCompDialogCallbackProxy : public IRemoteProxy<ISecCompDialogCallback> {
public:
    explicit SecCompDialogCallbackProxy(const sptr<IRemoteObject>& impl);
    ~SecCompDialogCallbackProxy() override;

    void OnDialogClosed(int32_t result) override;
private:
    static inline BrokerDelegator<SecCompDialogCallbackProxy> delegator_;
};
} // namespace SecurityComponent
} // namespace Security
} // namespace OHOS
#endif // SEC_COMP_DIALOG_CALLBACK_PROXY_H