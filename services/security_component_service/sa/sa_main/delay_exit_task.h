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

#ifndef SECURITY_COMPONENT_DELAY_EXIT_TASK_H
#define SECURITY_COMPONENT_DELAY_EXIT_TASK_H
#include <memory>
#include <string>

#include "nocopyable.h"
#include "sec_event_handler.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
class DelayExitTask {
public:
    static DelayExitTask& GetInstance();
    virtual ~DelayExitTask() = default;

    void Init(const std::shared_ptr<SecEventHandler>& secHandler);
    void Start();
    void Stop();
private:
    DelayExitTask();
    std::shared_ptr<SecEventHandler> secHandler_;

    DISALLOW_COPY_AND_MOVE(DelayExitTask);
};
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
#endif  // SECURITY_COMPONENT_DELAY_EXIT_TASK_H

