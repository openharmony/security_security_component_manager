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
#ifndef I_SECURITY_COMPONENT_LOCATION_BUTTON_H
#define I_SECURITY_COMPONENT_LOCATION_BUTTON_H

#include <string>
#include "nlohmann/json.hpp"
#include "sec_comp_base.h"
#include "sec_comp_info.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
enum class LocationDesc : int32_t {
    UNKNOWN_TEXT = -2,
    NO_TEXT = -1,
    CURRENT_LOCATION  = 0,
    ADD_LOCATION = 4,
    SELECT_LOCATION = 1,
    SHARE_LOCATION = 2,
    SEND_LOCATION = 3,
    LOCATING = 5,
    LOCATION = 6,
    SEND_CURRENT_LOCATION = 7,
    RELOCATION = 8,
    PUNCH_IN = 9,
    CURRENT_POSITION = 10,
    MAX_LABEL_TYPE
};

enum class LocationIcon : int32_t {
    UNKNOWN_ICON = -2,
    NO_ICON = -1,
    FILLED_ICON = 0,
    LINE_ICON = 1,
    MAX_ICON_TYPE
};

class LocationButton : public SecCompBase {
public:
    virtual bool IsParamValid() override;
    virtual bool CompareComponentBasicInfo(SecCompBase *other) const override;
private:
    bool ParseStyle(const nlohmann::json& json, const std::string& tag);
};
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
#endif  // I_SECURITY_COMPONENT_LOCATION_BUTTON_H
