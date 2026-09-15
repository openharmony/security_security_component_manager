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
#ifndef SEC_COMP_GRANT_ADAPTER_MOCK_H
#define SEC_COMP_GRANT_ADAPTER_MOCK_H

namespace OHOS {
namespace Security {
namespace SecurityComponent {
// test helpers backed by mock/src/sec_comp_grant_adapter.cpp
int32_t GetGrantAdapterLastUserId();
void ResetGrantAdapterLastUserId();

// switch every SecCompGrantAdapter entry between success and failure so that
// unit tests can drive both the normal path and the adapter load-failure
// degradation path without a real dlopen.
void SetGrantAdapterAlwaysFail(bool alwaysFail);
bool GetGrantAdapterAlwaysFail();

// clear the cached lookup state recorded by the mock
void ResetGrantAdapterCallState();

// how many times the warmup entry InitGrantAdapter has been invoked
uint32_t GetGrantAdapterInitCallCount();
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
#endif  // SEC_COMP_GRANT_ADAPTER_MOCK_H
