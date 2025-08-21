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
#include "window_info_helper.h"

#include <vector>
#include "sec_comp_info_helper.h"
#include "sec_comp_log.h"
#include "window_manager.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "WindowInfoHelper"};
constexpr int32_t INVALID_WINDOW_LAYER = -1;
constexpr uint32_t UI_EXTENSION_MASK = 0x40000000;
}

Scales WindowInfoHelper::GetWindowScale(int32_t windowId, bool& isCompatScaleMode, SecCompRect& scaleRect)
{
    Scales scales;
    scales.floatingScale = FULL_SCREEN_SCALE;
    std::vector<sptr<Rosen::AccessibilityWindowInfo>> infos;
    if (Rosen::WindowManager::GetInstance().GetAccessibilityWindowInfo(infos) != Rosen::WMError::WM_OK) {
        SC_LOG_ERROR(LABEL, "Get AccessibilityWindowInfo failed");
        return scales;
    }
    auto iter = std::find_if(infos.begin(), infos.end(), [windowId](const sptr<Rosen::AccessibilityWindowInfo> info) {
        return windowId == info->wid_;
    });
    if ((iter == infos.end()) || (*iter == nullptr)) {
        SC_LOG_WARN(LABEL, "Cannot find AccessibilityWindowInfo, return default scale");
        return scales;
    }
    isCompatScaleMode = (*iter)->isCompatScaleMode_;
    scales.floatingScale = (*iter)->scaleVal_;
    scales.scaleX = (*iter)->scaleX_;
    scales.scaleY = (*iter)->scaleY_;
    scaleRect.x_ = (*iter)->scaleRect_.posX_;
    scaleRect.y_ = (*iter)->scaleRect_.posY_;
    scaleRect.width_ = (*iter)->scaleRect_.width_;
    scaleRect.height_ = (*iter)->scaleRect_.height_;
    SC_LOG_INFO(LABEL, "Get floatingScale = %{public}f, scaleX = %{public}f, scaleY = %{public}f, \
        isCompatScaleMode = %{public}d", scales.floatingScale, scales.scaleX, scales.scaleY, isCompatScaleMode);
    return scales;
}

std::string GetSecCompWindowMsg(int32_t compWinId, const SecCompRect& secRect,
    int32_t coverWindowId)
{
    std::string message = ", the id of the window where the security component is located is " +
        std::to_string(compWinId) + ", security component is covered by the window(id = " +
        std::to_string(coverWindowId) + "), the size of security component is (x = " +
        std::to_string(secRect.x_) + ", y = " + std::to_string(secRect.y_) +
        ", width = " + std::to_string(secRect.width_) +
        ", height = " + std::to_string(secRect.height_) + ")";
    return message;
}

std::string GetCoveredWindowMsg(const Rosen::Rect& windowRect)
{
    std::string coveredWindowMessage = ", the size of window covering security component is (x = " +
        std::to_string(windowRect.posX_) + ", y = " + std::to_string(windowRect.posY_) +
        ", width = " + std::to_string(windowRect.width_) + ", height = " + std::to_string(windowRect.height_) + ")";
    return coveredWindowMessage;
}

static bool IsRectInWindRect(const Rosen::Rect& windRect, const SecCompRect& secRect)
{
    // left or right
    if ((secRect.x_ + secRect.width_ <= windRect.posX_) ||
        (secRect.x_ >= windRect.posX_ + static_cast<int32_t>(windRect.width_))) {
        return false;
    }
    // top or bottom
    if ((secRect.y_ + secRect.height_ <= windRect.posY_) ||
        (secRect.y_ >= windRect.posY_ + static_cast<int32_t>(windRect.height_))) {
        return false;
    }
    if ((GreatOrEqual(windRect.posX_, secRect.x_ + secRect.width_ - secRect.borderRadius_.rightBottom) &&
        GreatOrEqual(windRect.posY_, secRect.y_ + secRect.height_ - secRect.borderRadius_.rightBottom))) {
        auto distance = SecCompInfoHelper::GetDistance(secRect.x_ + secRect.width_ - secRect.borderRadius_.rightBottom,
            secRect.y_ + secRect.height_ - secRect.borderRadius_.rightBottom, windRect.posX_, windRect.posY_);
        return !GreatNotEqual(distance, secRect.borderRadius_.rightBottom - 1.0);
    }
    if ((GreatOrEqual(secRect.x_ + secRect.borderRadius_.leftBottom, windRect.posX_ + windRect.width_) &&
        GreatOrEqual(windRect.posY_, secRect.y_ + secRect.height_ - secRect.borderRadius_.leftBottom))) {
        auto distance = SecCompInfoHelper::GetDistance(secRect.x_ + secRect.borderRadius_.leftBottom,
            secRect.y_ + secRect.height_ - secRect.borderRadius_.leftBottom, windRect.posX_ + windRect.width_,
            windRect.posY_);
        return !GreatNotEqual(distance, secRect.borderRadius_.leftBottom - 1.0);
    }
    if ((GreatOrEqual(windRect.posX_, secRect.x_ + secRect.width_ - secRect.borderRadius_.rightTop) &&
        GreatOrEqual(secRect.y_ + secRect.borderRadius_.rightTop, windRect.posY_ + windRect.height_))) {
        auto distance = SecCompInfoHelper::GetDistance(secRect.x_ + secRect.width_ - secRect.borderRadius_.rightTop,
            secRect.y_ + secRect.borderRadius_.rightTop, windRect.posX_, windRect.posY_ + windRect.height_);
        return !GreatNotEqual(distance, secRect.borderRadius_.rightTop - 1.0);
    }
    if ((GreatOrEqual(secRect.x_ + secRect.borderRadius_.leftTop, windRect.posX_ + windRect.width_) &&
        GreatOrEqual(secRect.y_ + secRect.borderRadius_.leftTop, windRect.posY_ + windRect.height_))) {
        auto distance = SecCompInfoHelper::GetDistance(secRect.x_ + secRect.borderRadius_.leftTop,
            secRect.y_ + secRect.borderRadius_.leftTop, windRect.posX_ + windRect.width_,
            windRect.posY_ + windRect.height_);
        return !GreatNotEqual(distance, secRect.borderRadius_.leftTop - 1.0);
    }

    return true;
}

static bool IsOtherWindowCoverComp(int32_t compWinId, const SecCompRect& secRect, int32_t compLayer,
    std::string& message, const std::vector<std::pair<int32_t, int32_t>>& layerList)
{
    if (compLayer == INVALID_WINDOW_LAYER) {
        SC_LOG_ERROR(LABEL, "windId %{public}d component layer is wrong", compWinId);
        return false;
    }

    if (layerList.size() == static_cast<size_t>(0)) {
        return true;
    }

    auto iter = std::find_if(layerList.begin(), layerList.end(), [compLayer](const std::pair<int32_t, int32_t> layer) {
        return layer.second >= compLayer;
    });
    if (iter != layerList.end()) {
        SC_LOG_ERROR(LABEL, "component window %{public}d is covered by %{public}d, click check failed",
            compWinId, iter->first);
        message = GetSecCompWindowMsg(compWinId, secRect, iter->first);
        return false;
    }
    return true;
}
bool WindowInfoHelper::CheckOtherWindowCoverComp(int32_t compWinId, const SecCompRect& secRect, std::string& message)
{
    if ((static_cast<uint32_t>(compWinId) & UI_EXTENSION_MASK) == UI_EXTENSION_MASK) {
        SC_LOG_INFO(LABEL, "UI extension can not check");
        return true;
    }
    std::vector<sptr<Rosen::UnreliableWindowInfo>> infos;
    if (Rosen::WindowManager::GetInstance().GetUnreliableWindowInfo(compWinId, infos) != Rosen::WMError::WM_OK) {
        SC_LOG_ERROR(LABEL, "Get AccessibilityWindowInfo failed");
        return false;
    }

    int32_t compLayer = INVALID_WINDOW_LAYER;
    std::string coveredWindowMsg;
    // {windowId, zOrder}
    std::vector<std::pair<int32_t, int32_t>> layerList;
    for (auto& info : infos) {
        if (info == nullptr) {
            continue;
        }

        if (info->windowId_ == compWinId) {
            compLayer = static_cast<int32_t>(info->zOrder_);
            continue;
        }
        if (info->floatingScale_ != 0.0) {
            info->windowRect_.width_ *= info->floatingScale_;
            info->windowRect_.height_ *= info->floatingScale_;
        }
        if (IsRectInWindRect(info->windowRect_, secRect)) {
            layerList.emplace_back(std::make_pair(info->windowId_, info->zOrder_));
            if (compLayer != INVALID_WINDOW_LAYER && static_cast<int32_t>(info->zOrder_) >= compLayer) {
                coveredWindowMsg = GetCoveredWindowMsg(info->windowRect_);
                break;
            }
        }
    }

    bool res = IsOtherWindowCoverComp(compWinId, secRect, compLayer, message, layerList);
    if (!message.empty()) {
        message += coveredWindowMsg;
    }
    return res;
}
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
