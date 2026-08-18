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
#ifndef SECURITY_COMPONENT_WM_MINI_CLIENT_TEST_H
#define SECURITY_COMPONENT_WM_MINI_CLIENT_TEST_H

#include <cstdint>

#include <iremote_object.h>
#include <refbase.h>

namespace OHOS {
namespace Rosen {
class WmMiniClientTest {
public:
    static void SetWmsRemote(const sptr<IRemoteObject>& remote);
    static void SetMultiInstanceEnabled(bool enabled);
    static void SetWriteFailurePoint(int32_t failPoint);
    static void SetMiniInfoAllocFail(bool forceFail);
    static void SetParcelableVectorSizeOverLimit(bool forceFail);
    static void SetParcelableVectorResizeFail(bool forceFail);
};
}  // namespace Rosen
}  // namespace OHOS

#endif  // SECURITY_COMPONENT_WM_MINI_CLIENT_TEST_H
