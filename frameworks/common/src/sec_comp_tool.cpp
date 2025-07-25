/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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
#include "sec_comp_tool.h"

#include <cmath>
#include <cstdint>
#include "sec_comp_log.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
namespace {
static constexpr uint8_t MAX_TRANSPARENT = 0x99; // 60%

constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "SecCompTool"};
}

bool IsColorTransparent(const SecCompColor& color)
{
    SC_LOG_DEBUG(LABEL, "Color %{public}x alpha %{public}x", color.value, color.argb.alpha);
    return color.argb.alpha < MAX_TRANSPARENT;
}

bool IsColorFullTransparent(const SecCompColor& color)
{
    return color.argb.alpha == 0;
}
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
