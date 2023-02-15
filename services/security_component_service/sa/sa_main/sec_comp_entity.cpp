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
#include "sec_comp_entity.h"
#include "sec_comp_err.h"
#include "sec_comp_info_helper.h"
#include "sec_comp_log.h"
#include "sec_comp_tool.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "SecCompEntity"};
static constexpr uint64_t MAX_TOUCH_INTERVAL = 1000L; // 1000ms
}

int32_t SecCompEntity::RevokeTempPermission()
{
    if (!isGrant_) {
        SC_LOG_ERROR(LABEL, "security component is not granted");
        return SC_OK;
    }
    isGrant_ = false;
    return SecCompInfoHelper::RevokeTempPermission(tokenId_, componentInfo_);
}

int32_t SecCompEntity::GrantTempPermission()
{
    if (isGrant_) {
        SC_LOG_ERROR(LABEL, "security component is already granted");
        return SC_OK;
    }
    isGrant_ = true;
    return SecCompInfoHelper::GrantTempPermission(tokenId_, componentInfo_);
}

bool SecCompEntity::CompareComponentInfo(const SecCompBase* other) const
{
    return SecCompInfoHelper::CompareSecCompInfo(componentInfo_.get(), other);
}

bool SecCompEntity::CheckTouchInfo(const SecCompClickEvent& touchInfo) const
{
    uint64_t current = GetCurrentTime();
    if (touchInfo.timestamp < current - MAX_TOUCH_INTERVAL || touchInfo.timestamp > current) {
        SC_LOG_ERROR(LABEL, "touch timestamp invalid");
        return false;
    }

    if (!componentInfo_->rect_.IsInRect(touchInfo.touchX, touchInfo.touchY)) {
        SC_LOG_ERROR(LABEL, "touch point is not in component rect");
        return false;
    }
    return true;
}

bool SecCompEntity::IsRectOverlaped(const SecCompEntity& other) const
{
    std::shared_ptr<SecCompBase> otherInfo = other.GetComponentInfo();
    if (otherInfo == nullptr) {
        SC_LOG_ERROR(LABEL, "component info is null");
        return true;
    }

    return componentInfo_->rect_.IsRectOverlaped(otherInfo->rect_);
}
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS