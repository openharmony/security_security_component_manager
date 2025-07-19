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
#include "sec_comp_info_helper.h"

#include <iomanip>
#include <sstream>

#include "accesstoken_kit.h"
#include "display.h"
#include "display_info.h"
#include "display_manager.h"
#include "location_button.h"
#include "paste_button.h"
#include "save_button.h"
#include "sec_comp_err.h"
#include "sec_comp_info.h"
#include "sec_comp_log.h"
#include "sec_comp_tool.h"
#include "window_info_helper.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "SecCompInfoHelper"};
static constexpr double MAX_RECT_PERCENT = 0.3F; // 30%
static constexpr double ZERO_OFFSET = 0.0F;
const std::string OUT_OF_SCREEN = ", security component is out of screen, security component(x = ";
const std::string OUT_OF_WINDOW = ", security component is out of window, security component(x = ";
const std::string SECURITY_COMPONENT_IS_TOO_LARGER =
    ", security component is too larger, security component(width = ";
const int HEX_FIELD_WIDTH = 2;
const int NUMBER_TWO = 2;
const char HEX_FILL_CHAR = '0';
}

void SecCompInfoHelper::AdjustSecCompRect(SecCompBase* comp, float scale, bool isCompatScaleMode)
{
    comp->rect_.width_ *= scale;
    comp->rect_.height_ *= scale;
    if (!isCompatScaleMode) {
        // window scales towards the top-left corner
        comp->rect_.x_ = comp->windowRect_.x_ + (comp->rect_.x_ - comp->windowRect_.x_) * scale;
        comp->rect_.y_ = comp->windowRect_.y_ + (comp->rect_.y_ - comp->windowRect_.y_) * scale;
    } else {
        // window scales towards the center
        auto disX = comp->rect_.x_ - comp->windowRect_.x_;
        auto disY = comp->rect_.y_ - comp->windowRect_.y_;
        comp->windowRect_.x_ = comp->windowRect_.x_ + (1 - scale) * comp->windowRect_.width_ / NUMBER_TWO;
        comp->windowRect_.y_ = comp->windowRect_.y_ + (1 - scale) * comp->windowRect_.height_ / NUMBER_TWO;
        comp->rect_.x_ = comp->windowRect_.x_ + scale * disX;
        comp->rect_.y_ = comp->windowRect_.y_ + scale * disY;
    }
    SC_LOG_DEBUG(LABEL, "After adjust x %{public}f, y %{public}f, width %{public}f, height %{public}f",
        comp->rect_.x_, comp->rect_.y_, comp->rect_.width_, comp->rect_.height_);
    comp->windowRect_.width_ *= scale;
    comp->windowRect_.height_ *= scale;
}

SecCompBase* SecCompInfoHelper::ParseComponent(SecCompType type, const nlohmann::json& jsonComponent,
    std::string& message, bool isClicked)
{
    SecCompBase* comp = nullptr;
    message.clear();
    switch (type) {
        case LOCATION_COMPONENT:
            comp = ConstructComponent<LocationButton>(jsonComponent, message, isClicked);
            break;
        case PASTE_COMPONENT:
            comp = ConstructComponent<PasteButton>(jsonComponent, message, isClicked);
            break;
        case SAVE_COMPONENT:
            comp = ConstructComponent<SaveButton>(jsonComponent, message, isClicked);
            break;
        default:
            SC_LOG_ERROR(LABEL, "Parse component type unknown");
            break;
    }
    if (comp == nullptr) {
        SC_LOG_ERROR(LABEL, "Parse component failed");
        return comp;
    }

    comp->SetValid(CheckComponentValid(comp, message));
    return comp;
}

static bool GetScreenSize(double& width, double& height, const uint64_t displayId, const CrossAxisState crossAxisState)
{
    sptr<OHOS::Rosen::Display> display =
        OHOS::Rosen::DisplayManager::GetInstance().GetDisplayById(displayId);
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
    if (crossAxisState == CrossAxisState::STATE_CROSS) {
        height = static_cast<double>(info->GetPhysicalHeight());
    } else {
        height = static_cast<double>(info->GetHeight());
    }
    SC_LOG_DEBUG(LABEL, "display manager Screen width %{public}f height %{public}f",
        width, height);
    return true;
}

double SecCompInfoHelper::GetDistance(DimensionT x1, DimensionT y1, DimensionT x2, DimensionT y2)
{
    return sqrt(pow(x1 - x2, NUMBER_TWO) + pow(y1 -y2, NUMBER_TWO));
}

bool SecCompInfoHelper::IsOutOfWatchScreen(const SecCompRect& rect, double radius, std::string& message)
{
    double leftTop = GetDistance(rect.x_ + rect.borderRadius_.leftTop,
        rect.y_ + rect.borderRadius_.leftTop, radius, radius);
    double leftBottom = GetDistance(rect.x_ + rect.borderRadius_.leftBottom,
        rect.y_ + rect.height_ - rect.borderRadius_.leftBottom, radius, radius);
    double rightTop = GetDistance(rect.x_ + rect.width_ - rect.borderRadius_.rightTop,
        rect.y_ + rect.borderRadius_.rightTop, radius, radius);
    double rightBottom = GetDistance(rect.x_ + rect.width_ - rect.borderRadius_.rightBottom,
        rect.y_ + rect.height_ - rect.borderRadius_.rightBottom, radius, radius);
    if (GreatNotEqual(leftTop, radius - rect.borderRadius_.leftTop + 1.0) ||
        GreatNotEqual(leftBottom, radius - rect.borderRadius_.leftBottom + 1.0) ||
        GreatNotEqual(rightTop, radius - rect.borderRadius_.rightTop + 1.0) ||
        GreatNotEqual(rightBottom, radius - rect.borderRadius_.rightBottom + 1.0)) {
        SC_LOG_ERROR(LABEL, "SecurityComponentCheckFail: security component is out of screen");
        message = OUT_OF_SCREEN + std::to_string(rect.x_) + ", y = " + std::to_string(rect.y_) +
            ", width = " + std::to_string(rect.width_) + ", height = " + std::to_string(rect.height_) +
            "), current screen(width = " + std::to_string(radius * NUMBER_TWO) +
            ", height = " + std::to_string(radius * NUMBER_TWO) + ")";
        return true;
    }
    return false;
}

bool SecCompInfoHelper::IsOutOfScreen(const SecCompRect& rect, double curScreenWidth, double curScreenHeight,
    std::string& message, bool isWearable)
{
    if (isWearable) {
        if (IsOutOfWatchScreen(rect, curScreenHeight / NUMBER_TWO, message)) {
            return true;
        }
    } else {
        if (GreatNotEqual(ZERO_OFFSET, rect.x_) || GreatNotEqual(ZERO_OFFSET, rect.y_) ||
            GreatNotEqual(rect.x_, curScreenWidth) || GreatNotEqual(rect.y_, curScreenHeight)) {
            SC_LOG_ERROR(LABEL, "SecurityComponentCheckFail: security component is out of screen");
            message = OUT_OF_SCREEN + std::to_string(rect.x_) + ", y = " + std::to_string(rect.y_) +
                ", width = " + std::to_string(rect.width_) + ", height = " + std::to_string(rect.height_) +
                "), current screen(width = " + std::to_string(curScreenWidth) +
                ", height = " + std::to_string(curScreenHeight) + ")";
            return true;
        }

        if (GreatOrEqual((rect.x_ + rect.width_), curScreenWidth + 1.0) ||
            GreatOrEqual((rect.y_ + rect.height_), curScreenHeight + 1.0)) {
            SC_LOG_ERROR(LABEL, "SecurityComponentCheckFail: security component is out of screen");
            message = OUT_OF_SCREEN + std::to_string(rect.x_) + ", y = " + std::to_string(rect.y_) +
                ", width = " + std::to_string(rect.width_) + ", height = " + std::to_string(rect.height_) +
                "), current screen(width = " + std::to_string(curScreenWidth) +
                ", height = " + std::to_string(curScreenHeight) + ")";
            return true;
        }
    }

    return false;
}

bool SecCompInfoHelper::CheckRectValid(const SecCompRect& rect, const SecCompRect& windowRect,
    const ScreenInfo& screenInfo, std::string& message)
{
    double curScreenWidth = 0.0F;
    double curScreenHeight = 0.0F;
    if (!GetScreenSize(curScreenWidth, curScreenHeight, screenInfo.displayId, screenInfo.crossAxisState)) {
        SC_LOG_ERROR(LABEL, "Get screen size is invalid");
        return false;
    }

    if (GreatOrEqual(0.0, rect.width_) || GreatOrEqual(0.0, rect.height_)) {
        SC_LOG_ERROR(LABEL, "width or height is <= 0");
        return false;
    }

    message.clear();
    if (IsOutOfScreen(rect, curScreenWidth, curScreenHeight, message, screenInfo.isWearable)) {
        return false;
    }

    if (GreatNotEqual(windowRect.x_, rect.x_ + 1.0) || GreatNotEqual(windowRect.y_, rect.y_ + 1.0) ||
        GreatNotEqual(rect.x_ + rect.width_, windowRect.x_ + windowRect.width_ + 1.0) ||
        GreatNotEqual(rect.y_ + rect.height_, windowRect.y_ + windowRect.height_ + 1.0)) {
        SC_LOG_ERROR(LABEL, "SecurityComponentCheckFail: security component is out of window");
        message = OUT_OF_WINDOW + std::to_string(rect.x_) + ", y = " + std::to_string(rect.y_) +
            ", width = " + std::to_string(rect.width_) + ", height = " + std::to_string(rect.height_) +
            "), current window(x = " + std::to_string(windowRect.x_) + ", y = " + std::to_string(windowRect.y_) +
            ", width = " + std::to_string(windowRect.width_) + ", height = " +
            std::to_string(windowRect.height_) + ")";
        return false;
    }

    // check rect > 30%
    if (GreatOrEqual((rect.width_ * rect.height_), (curScreenWidth * curScreenHeight * MAX_RECT_PERCENT))) {
        SC_LOG_INFO(LABEL, "security component is larger than 30 percent of screen");
    }
    SC_LOG_DEBUG(LABEL, "check component rect success.");
    return true;
}

std::string ColorToHexString(const SecCompColor& color)
{
    std::stringstream ss;
    ss << std::hex;
    ss << std::setw(HEX_FIELD_WIDTH) << std::setfill(HEX_FILL_CHAR) << static_cast<int>(color.argb.alpha);
    ss << std::setw(HEX_FIELD_WIDTH) << std::setfill(HEX_FILL_CHAR) << static_cast<int>(color.argb.red);
    ss << std::setw(HEX_FIELD_WIDTH) << std::setfill(HEX_FILL_CHAR) << static_cast<int>(color.argb.green);
    ss << std::setw(HEX_FIELD_WIDTH) << std::setfill(HEX_FILL_CHAR) << static_cast<int>(color.argb.blue);

    return "#" + ss.str();
}

static bool CheckSecCompBaseButtonColorsimilar(const SecCompBase* comp, std::string& message)
{
    if ((comp->bg_ != SecCompBackground::NO_BG_TYPE) && !IsColorFullTransparent(comp->bgColor_) &&
        (comp->icon_ != NO_ICON) && IsColorSimilar(comp->iconColor_, comp->bgColor_)) {
        SC_LOG_INFO(LABEL, "SecurityComponentCheckFail: iconColor is similar with backgroundColor.");
        message = ", icon color is similar with background color, icon color = " +
            ColorToHexString(comp->iconColor_) + ", background color = " + ColorToHexString(comp->bgColor_);
        return false;
    }

    if ((comp->bg_ != SecCompBackground::NO_BG_TYPE) && !IsColorFullTransparent(comp->bgColor_) &&
        (comp->text_ != NO_TEXT) && IsColorSimilar(comp->fontColor_, comp->bgColor_)) {
        SC_LOG_INFO(LABEL, "SecurityComponentCheckFail: fontColor is similar with backgroundColor.");
        message = ", font color is similar with background color, font color = " +
            ColorToHexString(comp->fontColor_) + ", background color = " + ColorToHexString(comp->bgColor_);
        return false;
    }

    if (comp->bg_ == SecCompBackground::NO_BG_TYPE &&
        ((comp->padding_.top != MIN_PADDING_WITHOUT_BG) || (comp->padding_.right != MIN_PADDING_WITHOUT_BG) ||
        (comp->padding_.bottom != MIN_PADDING_WITHOUT_BG) || (comp->padding_.left != MIN_PADDING_WITHOUT_BG))) {
        SC_LOG_INFO(LABEL, "padding can not change without background.");
        message = ", padding can not change without background";
        return false;
    }

    return true;
}

static bool CheckSecCompBaseButton(const SecCompBase* comp, std::string& message)
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
            message = ", font size is too small, font size = " +
                std::to_string(comp->fontSize_);
            return false;
        }
    }
    if ((comp->icon_ >= 0) && comp->iconSize_ < MIN_ICON_SIZE) {
        SC_LOG_INFO(LABEL, "SecurityComponentCheckFail: iconSize is too small.");
        message = ", icon size is too small, icon size = " +
            std::to_string(comp->iconSize_);
        return false;
    }

    return CheckSecCompBaseButtonColorsimilar(comp, message);
}

static bool CheckSecCompBase(const SecCompBase* comp, std::string& message)
{
    if (comp->parentEffect_) {
        SC_LOG_ERROR(LABEL,
            "SecurityComponentCheckFail: the parents of security component have invalid effect.");
        message = "PARENT_HAVE_INVALID_EFFECT";
        return false;
    }

    if ((comp->padding_.top < MIN_PADDING_SIZE) || (comp->padding_.right < MIN_PADDING_SIZE) ||
        (comp->padding_.bottom < MIN_PADDING_SIZE) || (comp->padding_.left < MIN_PADDING_SIZE)) {
        SC_LOG_ERROR(LABEL, "SecurityComponentCheckFail: padding is too small.");
        message = ", padding is too small, padding(top = " + std::to_string(comp->padding_.top) +
            ", bottom = " + std::to_string(comp->padding_.bottom) +
            ", left = " + std::to_string(comp->padding_.left) +
            ", right = " + std::to_string(comp->padding_.right) + ")";
        return false;
    }

    if ((comp->textIconSpace_ < MIN_PADDING_SIZE)) {
        SC_LOG_ERROR(LABEL, "SecurityComponentCheckFail: textIconSpace is too small.");
        message = ", textIconSpace is too small, textIconSpace = " + std::to_string(comp->textIconSpace_);
        return false;
    }

    if ((comp->text_ != NO_TEXT) && (IsColorTransparent(comp->fontColor_))) {
        SC_LOG_ERROR(LABEL, "SecurityComponentCheckFail: fontColor is too transparent.");
        message = ", font color is too transparent, font color = " + ColorToHexString(comp->fontColor_);
        return false;
    }

    if ((comp->icon_ != NO_ICON) && (IsColorTransparent(comp->iconColor_))) {
        SC_LOG_ERROR(LABEL, "SecurityComponentCheckFail: iconColor is too transparent.");
        message = ", icon color is too transparent, icon color = " + ColorToHexString(comp->iconColor_);
        return false;
    }

    if (!CheckSecCompBaseButton(comp, message)) {
        return false;
    }
    return true;
}

bool SecCompInfoHelper::CheckComponentValid(SecCompBase* comp, std::string& message)
{
    if ((comp == nullptr) || !IsComponentTypeValid(comp->type_)) {
        SC_LOG_INFO(LABEL, "comp is null or type is invalid.");
        return false;
    }

    bool isCompatScaleMode = false;
    float scale = WindowInfoHelper::GetWindowScale(comp->windowId_, isCompatScaleMode);
    SC_LOG_DEBUG(LABEL, "WindowScale = %{public}f", scale);
    if (!IsEqual(scale, WindowInfoHelper::FULL_SCREEN_SCALE) && !IsEqual(scale, 0.0)) {
        AdjustSecCompRect(comp, scale, isCompatScaleMode);
    }

    if (!CheckSecCompBase(comp, message)) {
        SC_LOG_INFO(LABEL, "SecComp base is invalid.");
        return false;
    }

    return true;
}
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
