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
#include "bundle_mgr_client.h"
#include "datashare_helper.h"
#include "hisysevent.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "isec_comp_service.h"
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
static constexpr uint32_t FOLD_VIRTUAL_DISPLAY_ID = 999;
}

int32_t SecCompEntity::GrantTempPermission()
{
    isGrant_ = true;
    return SecCompPermManager::GetInstance().GrantTempPermission(tokenId_, componentInfo_);
}

bool SecCompEntity::CompareComponentBasicInfo(SecCompBase* other, bool isRectCheck) const
{
    return componentInfo_->CompareComponentBasicInfo(other, isRectCheck);
}

int32_t SecCompEntity::CheckPointEvent(SecCompClickEvent& clickInfo, int32_t superFoldOffsetY,
    const CrossAxisState crossAxisState) const
{
    auto current = static_cast<uint64_t>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count()) / TIME_CONVERSION_UNIT;
    if (clickInfo.point.timestamp < current - MAX_TOUCH_INTERVAL || clickInfo.point.timestamp > current) {
        SC_LOG_ERROR(LABEL, "touch timestamp invalid clickInfo. timestamp: %{public}llu, current: %{public}llu",
            static_cast<unsigned long long>(clickInfo.point.timestamp), static_cast<unsigned long long>(current));
        return SC_SERVICE_ERROR_CLICK_EVENT_INVALID;
    }

    if (!componentInfo_->rect_.IsInRect(clickInfo.point.touchX, clickInfo.point.touchY)) {
        if ((crossAxisState == CrossAxisState::STATE_CROSS) &&
            componentInfo_->rect_.IsInRect(clickInfo.point.touchX, clickInfo.point.touchY + superFoldOffsetY)) {
            clickInfo.point.touchY += superFoldOffsetY;
            SC_LOG_INFO(LABEL, "Fold PC cross state and component is in PC virtual screen.");
            return SC_OK;
        }
        SC_LOG_ERROR(LABEL, "touch point is not in component rect = (%{public}f, %{public}f)" \
            "left top point of component rect = (%{public}f, %{public}f)" \
            "right bottom point of component rect = (%{public}f, %{public}f)",
            clickInfo.point.touchX, clickInfo.point.touchY, componentInfo_->rect_.x_, componentInfo_->rect_.y_,
            componentInfo_->rect_.x_ + componentInfo_->rect_.width_,
            componentInfo_->rect_.y_ + componentInfo_->rect_.height_);
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
        SC_LOG_ERROR(LABEL, "keyboard keyCode invalid.");
        return SC_SERVICE_ERROR_CLICK_EVENT_INVALID;
    }

    return SC_OK;
}

bool SecCompEntity::IsInPCVirtualScreen(const CrossAxisState crossAxisState) const
{
    bool isInPCVirtualScreen = false;
    if (componentInfo_->displayId_ == FOLD_VIRTUAL_DISPLAY_ID) {
        if (crossAxisState == CrossAxisState::STATE_NO_CROSS) {
            isInPCVirtualScreen = true;
        } else {
            SC_LOG_WARN(LABEL, "Security component maybe in PC virtual screen, the cross axis state is %{public}d",
                static_cast<int32_t>(crossAxisState));
        }
    }
    return isInPCVirtualScreen;
}

int32_t SecCompEntity::CheckClickInfo(SecCompClickEvent& clickInfo, int32_t superFoldOffsetY,
    const CrossAxisState crossAxisState, std::string& message)
{
    bool isInPCVirtualScreen = IsInPCVirtualScreen(crossAxisState);
    SC_LOG_INFO(LABEL, "The cross axis state: %{public}d, the fold offset y: %{public}d.",
        static_cast<int32_t>(crossAxisState), superFoldOffsetY);
    if (isInPCVirtualScreen && clickInfo.type == ClickEventType::POINT_EVENT_TYPE) {
        clickInfo.point.touchY += superFoldOffsetY;
        componentInfo_->rect_.y_ += superFoldOffsetY;
    }
    message.clear();
    if ((GetType() != SecCompType::SAVE_COMPONENT) &&
        !WindowInfoHelper::CheckOtherWindowCoverComp(componentInfo_->windowId_, componentInfo_->rect_, message)) {
        SC_LOG_ERROR(LABEL, "Component may be covered by other window");
        if (!AllowToBypassSecurityCheck(message)) {
            return SC_SERVICE_ERROR_CLICK_EVENT_INVALID;
        }
    }

    int32_t res = SC_SERVICE_ERROR_CLICK_EVENT_INVALID;
    if (clickInfo.type == ClickEventType::POINT_EVENT_TYPE) {
        res = CheckPointEvent(clickInfo, superFoldOffsetY, crossAxisState);
    } else if (clickInfo.type == ClickEventType::ACCESSIBILITY_EVENT_TYPE) {
        SC_LOG_WARN(LABEL, "Device is in screen read mode.");
        res = SC_OK;
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
        int32_t uid = IPCSkeleton::GetCallingUid();
        OHOS::AppExecFwk::BundleMgrClient bmsClient;
        std::string bundleName = "";
        bmsClient.GetNameForUid(uid, bundleName);
        HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::SEC_COMPONENT, "CLICK_INFO_CHECK_FAILED",
            HiviewDFX::HiSysEvent::EventType::SECURITY, "CALLER_UID", uid, "CALLER_BUNDLE_NAME", bundleName,
            "CALLER_PID", IPCSkeleton::GetCallingPid(), "SC_ID", scId_, "SC_TYPE", componentInfo_->type_);
        return SC_ENHANCE_ERROR_CLICK_EXTRA_CHECK_FAIL;
    }
    return SC_OK;
}
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
