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
#ifndef INTERFACES_INNER_API_SECURITY_COMPONENT_KIT_H
#define INTERFACES_INNER_API_SECURITY_COMPONENT_KIT_H

#include <string>
#include "sec_comp_info.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
class SecCompKit {
public:
    static int32_t RegisterSecurityComponent(SecCompType type, const std::string& componentInfo, int32_t& scId);
    static int32_t UpdateSecurityComponent(int32_t scId, const std::string& componentInfo);
    static int32_t UnregisterSecurityComponent(int32_t scId);
    static int32_t ReportSecurityComponentClickEvent(int32_t scId,
        const std::string& componentInfo, const SecCompClickEvent& touchInfo);
};
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
#endif  // INTERFACES_INNER_API_SECURITY_COMPONENT_KIT_H