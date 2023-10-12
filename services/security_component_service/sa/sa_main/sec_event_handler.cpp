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
#include "sec_event_handler.h"

#include "sec_comp_log.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
SecEventHandler::SecEventHandler(
    const std::shared_ptr<AppExecFwk::EventRunner>& runner)
    : AppExecFwk::EventHandler(runner) {}

SecEventHandler::~SecEventHandler() = default;

bool SecEventHandler::ProxyPostTask(
    const Callback &callback, const std::string &name, int64_t delayTime)
{
    return AppExecFwk::EventHandler::PostTask(callback, name, delayTime);
}

void SecEventHandler::ProxyRemoveTask(const std::string &name)
{
    AppExecFwk::EventHandler::RemoveTask(name);
}
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
