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
#include "sec_comp_info_helper.h"

#include "display.h"
#include "display_info.h"
#include "display_manager.h"
#include "location_button.h"
#include "paste_button.h"
#include "save_button.h"
#include "sec_comp_err.h"
#include "sec_comp_log.h"
#include "sec_comp_service.h"
#include "sec_comp_tool.h"
#include "window_info_helper.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "SecCompInfoHelper"};
static constexpr double MAX_RECT_PERCENT = 0.1F; // 10%
static constexpr double ZERO_OFFSET = 0.0F;
static std::mutex g_renderLock;
}

void SecCompInfoHelper::AdjustSecCompRect(SecCompBase* comp, float scale)
{
    comp->rect_.width_ *= scale;
    comp->rect_.height_ *= scale;
    comp->rect_.x_ = comp->windowRect_.x_ + (comp->rect_.x_ - comp->windowRect_.x_) * scale;
    comp->rect_.y_ = comp->windowRect_.y_ + (comp->rect_.y_ - comp->windowRect_.y_) * scale;

    SC_LOG_DEBUG(LABEL, "After adjust x %{public}lf, y %{public}lf, width %{public}lf, height %{public}lf",
        comp->rect_.x_, comp->rect_.y_, comp->rect_.width_, comp->rect_.height_);
}

SecCompBase* SecCompInfoHelper::ParseComponent(SecCompType type, const nlohmann::json& jsonComponent)
{
    SecCompBase* comp = nullptr;
    switch (type) {
        case LOCATION_COMPONENT:
            comp = ConstructComponent<LocationButton>(jsonComponent);
            break;
        case PASTE_COMPONENT:
            comp = ConstructComponent<PasteButton>(jsonComponent);
            break;
        case SAVE_COMPONENT:
            comp = ConstructComponent<SaveButton>(jsonComponent);
            break;
        default:
            SC_LOG_ERROR(LABEL, "Parse component type unknown");
            break;
    }
    if (comp == nullptr) {
        SC_LOG_ERROR(LABEL, "Parse component failed");
        return comp;
    }

    comp->SetValid(CheckComponentValid(comp));
    return comp;
}

static bool GetScreenSize(double& width, double& height)
{
    sptr<OHOS::Rosen::Display> display =
        OHOS::Rosen::DisplayManager::GetInstance().GetDefaultDisplaySync();
    if (display == nullptr) {
        SC_LOG_ERROR(LABEL, "Get display manager failed");
        return false;
    }

    auto info = display->GetDisplayInfo();
    if (info == nullptr) {
        SC_LOG_ERROR(LABEL, "Get display info failed");
        return false;
    }

    width = static_cast<double>(info->GetWidth());
    height = static_cast<double>(info->GetHeight());
    SC_LOG_DEBUG(LABEL, "display manager Screen width %{public}f height %{public}f",
        width, height);
    return true;
}

bool SecCompInfoHelper::CheckRectValid(const SecCompRect& rect, const SecCompRect& windowRect)
{
    double curScreenWidth = 0.0F;
    double curScreenHeight = 0.0F;
    if (!GetScreenSize(curScreenWidth, curScreenHeight)) {
        SC_LOG_ERROR(LABEL, "Get screen size is invalid");
        return false;
    }

    if (GreatOrEqual(0.0, rect.width_) || GreatOrEqual(0.0, rect.height_)) {
        SC_LOG_ERROR(LABEL, "width or height is <= 0");
        return false;
    }

    if (GreatNotEqual(ZERO_OFFSET, rect.x_) || GreatNotEqual(ZERO_OFFSET, rect.y_) ||
        GreatNotEqual(rect.x_, curScreenWidth) || GreatNotEqual(rect.y_, curScreenHeight)) {
        SC_LOG_ERROR(LABEL, "SecurityComponentCheckFail: security component is out of screen");
        return false;
    }

    if (GreatOrEqual((rect.x_ + rect.width_), curScreenWidth) ||
        GreatOrEqual((rect.y_ + rect.height_), curScreenHeight)) {
        SC_LOG_ERROR(LABEL, "SecurityComponentCheckFail: security component is out of screen");
        return false;
    }

    if (GreatNotEqual(windowRect.x_, rect.x_) || GreatNotEqual(windowRect.y_, rect.y_) ||
        GreatNotEqual(rect.x_ + rect.width_, windowRect.x_ + windowRect.width_) ||
        GreatNotEqual(rect.y_ + rect.height_, windowRect.y_ + windowRect.height_)) {
        SC_LOG_ERROR(LABEL, "SecurityComponentCheckFail: security component is out of window");
        return false;
    }

    // check rect > 10%
    if (GreatOrEqual((rect.width_ * rect.height_), (curScreenWidth * curScreenHeight * MAX_RECT_PERCENT))) {
        SC_LOG_ERROR(LABEL, "SecurityComponentCheckFail: security component is larger than 10 percent of screen");
        return false;
    }
    SC_LOG_DEBUG(LABEL, "check component rect success.");
    return true;
}

static bool CheckSecCompBaseButton(const SecCompBase* comp)
{
    if ((comp->text_ < 0) && (comp->icon_ < 0)) {
        SC_LOG_INFO(LABEL, "both text and icon do not exist.");
        return false;
    }
    if (comp->text_ >= 0) {
        DimensionT minFontSize;
        if (comp->icon_ >= 0) {
            minFontSize = MIN_FONT_SIZE_WITH_ICON;
        } else {
            minFontSize = MIN_FONT_SIZE_WITHOUT_ICON;
        }

        if (comp->fontSize_ < minFontSize) {
            SC_LOG_INFO(LABEL, "SecurityComponentCheckFail: fontSize is too small.");
            return false;
        }
    }
    if ((comp->icon_ >= 0) && comp->iconSize_ < MIN_ICON_SIZE) {
        SC_LOG_INFO(LABEL, "SecurityComponentCheckFail: iconSize is too small.");
        return false;
    }

    if ((comp->bg_ != SecCompBackground::NO_BG_TYPE) && !IsColorFullTransparent(comp->bgColor_) &&
        (((comp->text_ != NO_TEXT) && (IsColorSimilar(comp->fontColor_, comp->bgColor_))) ||
        ((comp->icon_ != NO_ICON) && (IsColorSimilar(comp->iconColor_, comp->bgColor_))))) {
        SC_LOG_INFO(LABEL, "SecurityComponentCheckFail: fontColor or iconColor is similar with backgroundColor.");
        return false;
    }

    if (comp->bg_ == SecCompBackground::NO_BG_TYPE &&
        ((comp->padding_.top != MIN_PADDING_WITHOUT_BG) || (comp->padding_.right != MIN_PADDING_WITHOUT_BG) ||
        (comp->padding_.bottom != MIN_PADDING_WITHOUT_BG) || (comp->padding_.left != MIN_PADDING_WITHOUT_BG))) {
        SC_LOG_INFO(LABEL, "padding can not change without background.");
        return false;
    }

    return true;
}

static bool CheckSecCompBase(const SecCompBase* comp)
{
    if (comp->parentEffect_) {
        SC_LOG_ERROR(LABEL,
            "SecurityComponentCheckFail: the parents of security component have invalid effect.");
        return false;
    }

    if ((comp->padding_.top < MIN_PADDING_SIZE) || (comp->padding_.right < MIN_PADDING_SIZE) ||
        (comp->padding_.bottom < MIN_PADDING_SIZE) || (comp->padding_.left < MIN_PADDING_SIZE) ||
        (comp->textIconSpace_ < MIN_PADDING_SIZE)) {
        SC_LOG_ERROR(LABEL, "SecurityComponentCheckFail: padding or textIconSpace is too small.");
        return false;
    }

    if (((comp->text_ != NO_TEXT) && (IsColorTransparent(comp->fontColor_))) ||
        ((comp->icon_ != NO_ICON) && (IsColorTransparent(comp->iconColor_)))) {
        SC_LOG_ERROR(LABEL, "SecurityComponentCheckFail: fontColor or iconColor is too transparent.");
        return false;
    }
    if (!CheckSecCompBaseButton(comp)) {
        return false;
    }
    return true;
}

bool SecCompInfoHelper::CheckComponentValid(SecCompBase* comp)
{
    if ((comp == nullptr) || !IsComponentTypeValid(comp->type_)) {
        SC_LOG_INFO(LABEL, "comp is null or type is invalid.");
        return false;
    }

    float scale = WindowInfoHelper::GetWindowScale(comp->windowId_);
    if (!IsEqual(scale, WindowInfoHelper::FULL_SCREEN_SCALE) && !IsEqual(scale, 0.0)) {
        AdjustSecCompRect(comp, scale);
    }

    if (!CheckSecCompBase(comp)) {
        SC_LOG_INFO(LABEL, "SecComp base is invalid.");
        return false;
    }

    return true;
}

int32_t SecCompInfoHelper::GrantTempPermission(AccessToken::AccessTokenID tokenId,
    const std::shared_ptr<SecCompBase>& componentInfo)
{
    if ((tokenId <= 0) || (componentInfo == nullptr)) {
        SC_LOG_ERROR(LABEL, "Grant component is null");
        return SC_SERVICE_ERROR_PERMISSION_OPER_FAIL;
    }

    SecCompType type = componentInfo->type_;
    int32_t res;
    switch (type) {
        case LOCATION_COMPONENT:
            {
                res = SecCompPermManager::GetInstance().GrantAppPermission(tokenId,
                    "ohos.permission.APPROXIMATELY_LOCATION");
                if (res != SC_OK) {
                    return SC_SERVICE_ERROR_PERMISSION_OPER_FAIL;
                }
                res = SecCompPermManager::GetInstance().GrantAppPermission(tokenId, "ohos.permission.LOCATION");
                if (res != SC_OK) {
                    SecCompPermManager::GetInstance().RevokeAppPermission(
                        tokenId, "ohos.permission.APPROXIMATELY_LOCATION");
                    return SC_SERVICE_ERROR_PERMISSION_OPER_FAIL;
                }
                return SC_OK;
            }
        case PASTE_COMPONENT:
            res = SecCompPermManager::GetInstance().GrantAppPermission(tokenId, "ohos.permission.SECURE_PASTE");
            if (res != SC_OK) {
                return SC_SERVICE_ERROR_PERMISSION_OPER_FAIL;
            }
            SC_LOG_DEBUG(LABEL, "grant paste permission");
            return SC_OK;
        case SAVE_COMPONENT:
            SC_LOG_DEBUG(LABEL, "grant save permission");
            return SecCompPermManager::GetInstance().GrantTempSavePermission(tokenId);
        default:
            SC_LOG_ERROR(LABEL, "Parse component type unknown");
            break;
    }
    return SC_SERVICE_ERROR_PERMISSION_OPER_FAIL;
}
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
