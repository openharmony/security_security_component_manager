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

#ifndef SECURITY_COMPONENT_DIALOG_CALLBACK_H
#define SECURITY_COMPONENT_DIALOG_CALLBACK_H

#include "iremote_stub.h"
#include "nocopyable.h"
#include "sec_comp_dialog_callback_stub.h"
#include "sec_comp_err.h"
#include "sec_comp_info.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
class SecCompDialogCallback : public SecCompDialogCallbackStub {
public:
    explicit SecCompDialogCallback(OnFirstUseDialogCloseFunc&& callback)
    {
        callback_ = std::move(callback);
    };

    ~SecCompDialogCallback() override
    {
        if (callback_) {
            callback_(SC_SERVICE_ERROR_GRANT_CANCEL_FOR_DIALOG_CLOSE);
        }
    };

    void OnDialogClosed(int32_t result) override;
private:
    std::mutex callbackMutex_;
    OnFirstUseDialogCloseFunc callback_;
};
} // namespace SecurityComponent
} // namespace Security
} // namespace OHOS
#endif // I_SECURITY_COMPONENT_DIALOG_CALLBACK_STUB_H
