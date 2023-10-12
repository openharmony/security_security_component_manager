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
#include "save_button.h"

#include <tuple>
#include "sec_comp_log.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "SaveButton"};
}

bool SaveButton::IsTextIconTypeValid()
{
    if ((text_ <= UNKNOWN_TEXT) || (static_cast<SaveDesc>(text_) >= SaveDesc::MAX_LABEL_TYPE) ||
        (icon_ <= UNKNOWN_ICON) || (static_cast<SaveIcon>(icon_) >= SaveIcon::MAX_ICON_TYPE)) {
        return false;
    }

    return true;
}

bool SaveButton::IsCorrespondenceType()
{
    return (type_ == SAVE_COMPONENT);
}

bool SaveButton::CompareComponentBasicInfo(SecCompBase *other, bool isRectCheck) const
{
    if (!SecCompBase::CompareComponentBasicInfo(other, isRectCheck)) {
        SC_LOG_ERROR(LABEL, "SecComp base not equal.");
        return false;
    }
    SaveButton* otherSaveButton = reinterpret_cast<SaveButton *>(other);
    if (otherSaveButton == nullptr) {
        SC_LOG_ERROR(LABEL, "other is not save button.");
        return false;
    }
    return (icon_ == otherSaveButton->icon_) && (text_ == otherSaveButton->text_) &&
        (bg_ == otherSaveButton->bg_);
}
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
