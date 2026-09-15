/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include "sec_comp_grant_adapter.h"

#include <map>

#include "ability_manager_client.h"
#include "display_info.h"
#include "display_lite.h"
#include "display_manager_lite.h"
#include "sec_comp_info.h"
#include "sec_comp_log.h"
#include "want.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
    LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "SecCompGrantAdapterImpl"};
static const std::string GRANT_ABILITY_BUNDLE_NAME = "com.ohos.permissionmanager";
static const std::string GRANT_ABILITY_ABILITY_NAME = "com.ohos.permissionmanager.SecurityExtAbility";
static const std::string TYPE_KEY = "ohos.user.security.type";
static const std::string TOKEN_KEY = "ohos.ability.params.token";
static const std::string CALLBACK_KEY = "ohos.ability.params.callback";
static const std::string WINDOW_ID_KEY = "ohos.ability.params.windowId";
static const std::string CALLER_UID_KEY = "ohos.caller.uid";
static const std::string DISPLAY_WIDTH = "ohos.display.width";
static const std::string DISPLAY_HEIGHT = "ohos.display.height";
static const std::string DIALOG_OFFSET = "ohos.dialog.offset";
static const std::string NOTIFY_TYPE = "ohos.ability.notify.type";
constexpr int32_t DISPLAY_HALF_RATIO = 2;

bool SetDisplayParams(const SecCompGrantParams& params, AAFwk::Want& want)
{
    sptr<OHOS::Rosen::DisplayLite> display =
        OHOS::Rosen::DisplayManagerLite::GetInstance().GetDisplayById(params.displayId);
    if (display == nullptr) {
        SC_LOG_ERROR(LABEL, "Get display manager failed");
        return false;
    }
    auto info = display->GetDisplayInfo();
    if (info == nullptr) {
        SC_LOG_ERROR(LABEL, "Get display info failed");
        return false;
    }
    /* crossAxisState is INVALID or NO_CROSS */
    int32_t width = info->GetWidth();
    int32_t height = info->GetHeight();
    int32_t offset = 0;
    /* crossAxisState is CROSS */
    if (params.crossAxisState == CrossAxisState::STATE_CROSS) {
        height = info->GetPhysicalHeight();
        offset = static_cast<int32_t>(info->GetAvailableHeight()) / DISPLAY_HALF_RATIO;
    }
    SC_LOG_INFO(LABEL, "Display info width %{public}d height %{public}d, dialog offset %{public}d",
        width, height, offset);
    want.SetParam(DISPLAY_WIDTH, width);
    want.SetParam(DISPLAY_HEIGHT, height);
    want.SetParam(DIALOG_OFFSET, offset);
    return true;
}
}

class SecCompGrantInterfaceImpl : public SecCompGrantInterface {
public:
    SecCompGrantInterfaceImpl() = default;
    ~SecCompGrantInterfaceImpl() override = default;

    bool GetDisplaySize(uint64_t displayId, int32_t crossAxisState,
        int32_t& width, int32_t& height, bool& isRoundScreen) override
    {
        sptr<OHOS::Rosen::DisplayLite> display =
            OHOS::Rosen::DisplayManagerLite::GetInstance().GetDisplayById(displayId);
        if (display == nullptr) {
            SC_LOG_ERROR(LABEL, "Get display manager failed");
            return false;
        }
        auto info = display->GetDisplayInfo();
        if (info == nullptr) {
            SC_LOG_ERROR(LABEL, "Get display info failed");
            return false;
        }
        isRoundScreen = info->GetScreenShape() == OHOS::Rosen::ScreenShape::ROUND;
        width = info->GetWidth();
        if (crossAxisState == CrossAxisState::STATE_CROSS) {
            height = info->GetPhysicalHeight();
        } else {
            height = info->GetHeight();
        }
        SC_LOG_DEBUG(LABEL, "Display size width %{public}d height %{public}d, round %{public}d",
            width, height, static_cast<int32_t>(isRoundScreen));
        return true;
    }

    bool GetFoldCreaseBottomY(int32_t& bottomY) override
    {
        auto foldCreaseRegion = OHOS::Rosen::DisplayManagerLite::GetInstance().GetCurrentFoldCreaseRegion();
        if (foldCreaseRegion == nullptr) {
            SC_LOG_ERROR(LABEL, "foldCreaseRegion is nullptr");
            return false;
        }
        const auto& creaseRects = foldCreaseRegion->GetCreaseRects();
        if (creaseRects.empty()) {
            SC_LOG_ERROR(LABEL, "CreaseRects is empty");
            return false;
        }
        const auto& rect = creaseRects.front();
        bottomY = rect.height_ + rect.posY_;
        SC_LOG_INFO(LABEL, "Crease height: %{public}d, posY: %{public}d", rect.height_, rect.posY_);
        return true;
    }

    bool StartGrantAbility(const SecCompGrantParams& params,
        const sptr<IRemoteObject>& callerToken, const sptr<IRemoteObject>& srvCallback) override
    {
        AAFwk::Want want;
        want.SetElementName(GRANT_ABILITY_BUNDLE_NAME, GRANT_ABILITY_ABILITY_NAME);
        want.SetParam(TYPE_KEY, params.typeNum);
        want.SetParam(TOKEN_KEY, callerToken);
        want.SetParam(CALLBACK_KEY, srvCallback);
        want.SetParam(WINDOW_ID_KEY, params.windowId);
        want.SetParam(NOTIFY_TYPE, NotifyType::DIALOG);
        want.SetParam(CALLER_UID_KEY, params.callerUid);
        if (!SetDisplayParams(params, want)) {
            SC_LOG_ERROR(LABEL, "Set display params failed.");
            return false;
        }
        int32_t startRes = AAFwk::AbilityManagerClient::GetInstance()->StartExtensionAbility(
            want, callerToken, params.userId);
        SC_LOG_INFO(LABEL, "Start grant ability res %{public}d", startRes);
        return startRes == 0;
    }
};
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS

namespace OHOS {
namespace Security {
namespace SecurityComponent {
using UiInterfacePtr = SecCompGrantInterface*;
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS

extern "C" OHOS::Security::SecurityComponent::UiInterfacePtr GetGrantAdapterInstance(void)
{
    static OHOS::Security::SecurityComponent::SecCompGrantInterfaceImpl gUiInterfaceImpl;
    return &gUiInterfaceImpl;
}
