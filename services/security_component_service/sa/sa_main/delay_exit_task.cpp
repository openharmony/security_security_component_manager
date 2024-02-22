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
#include "delay_exit_task.h"

#include "sec_comp_log.h"
#include "sec_comp_manager.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "DelayExitTask"};
static const std::string DELAY_EXIT_TASK = "DelayExitTask";
static const int32_t DELAY_EXIT_MILLISECONDS = 120 * 1000; // 2m
}

DelayExitTask::DelayExitTask()
{
}

DelayExitTask& DelayExitTask::GetInstance()
{
    static DelayExitTask instance;
    return instance;
}

void DelayExitTask::Init(const std::shared_ptr<SecEventHandler>& secHandler)
{
    secHandler_ = secHandler;
}

void DelayExitTask::Start()
{
    if (secHandler_ == nullptr) {
        SC_LOG_ERROR(LABEL, "fail to get EventHandler");
        return;
    }

    std::function<void()> delayed = ([]() {
        SecCompManager::GetInstance().ExitSaProcess();
    });

    SC_LOG_INFO(LABEL, "Delay exit service after %{public}d ms", DELAY_EXIT_MILLISECONDS);
    secHandler_->ProxyPostTask(delayed, DELAY_EXIT_TASK, DELAY_EXIT_MILLISECONDS);
}

void DelayExitTask::Stop()
{
    if (secHandler_ == nullptr) {
        SC_LOG_ERROR(LABEL, "fail to get EventHandler");
        return;
    }

    SC_LOG_INFO(LABEL, "service delay exit handler stop");
    secHandler_->ProxyRemoveTask(DELAY_EXIT_TASK);
}
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
