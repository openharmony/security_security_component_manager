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

#include "sec_comp_enhance_kit.h"

#include "sec_comp_enhance_adapter.h"

extern "C" __attribute__((visibility("default"))) void InitSecCompClientEnhance()
{
    OHOS::Security::SecurityComponent::SecCompEnhanceKit::InitClientEnhance();
}

namespace OHOS {
namespace Security {
namespace SecurityComponent {
void SecCompEnhanceKit::InitClientEnhance(void)
{
    SecCompEnhanceAdapter::InitEnhanceHandler(SEC_COMP_ENHANCE_CLIENT_INTERFACE);
}

int32_t SecCompEnhanceKit::SetEnhanceCfg(uint8_t* cfg, uint32_t cfgLen)
{
    return SecCompEnhanceAdapter::SetEnhanceCfg(cfg, cfgLen);
}

int32_t SecCompEnhanceKit::GetPointerEventEnhanceData(void* data, uint32_t dataLen,
    uint8_t* enhanceData, uint32_t& enHancedataLen)
{
    return SecCompEnhanceAdapter::GetPointerEventEnhanceData(data, dataLen, enhanceData, enHancedataLen);
}
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
