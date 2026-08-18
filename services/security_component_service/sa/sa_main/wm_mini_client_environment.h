/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#ifndef I_SECURITY_COMPONENT_WM_MINI_CLIENT_ENVIRONMENT_H
#define I_SECURITY_COMPONENT_WM_MINI_CLIENT_ENVIRONMENT_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include <iremote_object.h>
#include <message_parcel.h>
#include <refbase.h>

#include "wm_mini_client.h"

namespace OHOS {
namespace Rosen {
enum class WmMiniParcelField : int32_t {
    NONE = 0,
    MOCK_INTERFACE_TOKEN,
    MOCK_USER_ID,
    SESSION_INTERFACE_TOKEN,
    WINDOW_INFO_INTERFACE_TOKEN,
    UNRELIABLE_INTERFACE_TOKEN,
    UNRELIABLE_WINDOW_ID,
};

class WmMiniClientEnvironment {
public:
    static sptr<IRemoteObject> GetWindowManagerServiceRemote();
    static bool IsMultiInstanceEnabled();
    static bool WriteInterfaceToken(
        MessageParcel& parcel, const std::u16string& descriptor, WmMiniParcelField field);
    static bool WriteInt32(MessageParcel& parcel, int32_t value, WmMiniParcelField field);
    static Security::SecurityComponent::MiniAccessibilityWindowInfo* CreateWindowInfo();
    static Security::SecurityComponent::MiniUnreliableWindowInfo* CreateUnreliableWindowInfo();
    static bool IsParcelableVectorSizeValid(size_t size, size_t maxSize);
    static bool ResizeParcelableVector(
        std::vector<sptr<Security::SecurityComponent::MiniAccessibilityWindowInfo>>& infos, size_t size);
    static bool ResizeParcelableVector(
        std::vector<sptr<Security::SecurityComponent::MiniUnreliableWindowInfo>>& infos, size_t size);
};
}  // namespace Rosen
}  // namespace OHOS

#endif  // I_SECURITY_COMPONENT_WM_MINI_CLIENT_ENVIRONMENT_H
