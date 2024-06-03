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

#include <chrono>
#include "hisysevent.h"
#include "ipc_skeleton.h"
#include "sec_comp_err.h"
#include "sec_comp_enhance_adapter.h"
#include "sec_comp_info_helper.h"
#include "sec_comp_log.h"
#include "window_info_helper.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "SecCompEntity"};
static constexpr uint64_t MAX_TOUCH_INTERVAL = 1000000L; // 1000ms
static constexpr uint64_t TIME_CONVERSION_UNIT = 1000;
}

int32_t SecCompEntity::GrantTempPermission()
{
    isGrant_ = true;
    return SecCompInfoHelper::GrantTempPermission(tokenId_, componentInfo_);
}

bool SecCompEntity::CompareComponentBasicInfo(SecCompBase* other, bool isRectCheck) const
{
    return componentInfo_->CompareComponentBasicInfo(other, isRectCheck);
}

int32_t SecCompEntity::CheckPointEvent(const SecCompClickEvent& clickInfo) const
{
    auto current = static_cast<uint64_t>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count()) / TIME_CONVERSION_UNIT;
    if (clickInfo.point.timestamp < current - MAX_TOUCH_INTERVAL || clickInfo.point.timestamp > current) {
        SC_LOG_ERROR(LABEL, "touch timestamp invalid clickInfo. timestamp: %{public}llu, current: %{public}llu",
            static_cast<unsigned long long>(clickInfo.point.timestamp), static_cast<unsigned long long>(current));
        return SC_SERVICE_ERROR_CLICK_EVENT_INVALID;
    }

    if (!componentInfo_->rect_.IsInRect(clickInfo.point.touchX, clickInfo.point.touchY)) {
        SC_LOG_ERROR(LABEL, "touch point is not in component rect, %{public}lf, %{public}lf",
            clickInfo.point.touchX, clickInfo.point.touchY);
        return SC_SERVICE_ERROR_CLICK_EVENT_INVALID;
    }
    return SC_OK;
}

int32_t SecCompEntity::CheckKeyEvent(const SecCompClickEvent& clickInfo) const
{
    auto current = static_cast<uint64_t>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count()) / TIME_CONVERSION_UNIT;
    if (clickInfo.key.timestamp < current - MAX_TOUCH_INTERVAL || clickInfo.key.timestamp > current) {
        SC_LOG_ERROR(LABEL, "keyboard timestamp invalid clickInfo. timestamp: %{public}llu, current: %{public}llu",
            static_cast<unsigned long long>(clickInfo.key.timestamp), static_cast<unsigned long long>(current));
        return SC_SERVICE_ERROR_CLICK_EVENT_INVALID;
    }
    if ((clickInfo.key.keyCode != KEY_SPACE) && (clickInfo.key.keyCode != KEY_ENTER) &&
        (clickInfo.key.keyCode != KEY_NUMPAD_ENTER)) {
        SC_LOG_ERROR(LABEL, "keyboard keyCode invalid. keyCode: %{public}d", clickInfo.key.keyCode);
        return SC_SERVICE_ERROR_CLICK_EVENT_INVALID;
    }

    return SC_OK;
}

int32_t SecCompEntity::CheckClickInfo(const SecCompClickEvent& clickInfo) const
{
    int32_t res = SC_SERVICE_ERROR_CLICK_EVENT_INVALID;
    if (clickInfo.type == ClickEventType::POINT_EVENT_TYPE) {
        res = CheckPointEvent(clickInfo);
    } else if (clickInfo.type == ClickEventType::KEY_EVENT_TYPE) {
        res = CheckKeyEvent(clickInfo);
    }
    if (res != SC_OK) {
        return res;
    }

    res = SecCompEnhanceAdapter::CheckExtraInfo(clickInfo);
    if (res == SC_SERVICE_ERROR_CLICK_EVENT_INVALID) {
        SC_LOG_ERROR(LABEL, "Click ExtraInfo is invalid");
        return res;
    }

    if ((res != SC_OK) && (res != SC_ENHANCE_ERROR_NOT_EXIST_ENHANCE)) {
        SC_LOG_ERROR(LABEL, "HMAC checkout failed");
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::SEC_COMPONENT, "CLICK_INFO_CHECK_FAILED",
            HiviewDFX::HiSysEvent::EventType::SECURITY, "CALLER_UID", IPCSkeleton::GetCallingUid(),
            "CALLER_PID", IPCSkeleton::GetCallingPid(), "SC_ID", scId_, "SC_TYPE", componentInfo_->type_);
        return SC_ENHANCE_ERROR_CLICK_EXTRA_CHECK_FAIL;
    }
    return SC_OK;
}
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
