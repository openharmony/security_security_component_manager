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

float WindowInfoHelper::GetWindowScale(int32_t windowId)
{
    float scale = FULL_SCREEN_SCALE;
    std::vector<sptr<Rosen::AccessibilityWindowInfo>> infos;
    if (Rosen::WindowManager::GetInstance().GetAccessibilityWindowInfo(infos) != Rosen::WMError::WM_OK) {
        SC_LOG_ERROR(LABEL, "Get AccessibilityWindowInfo failed");
        return scale;
    }
    auto iter = std::find_if(infos.begin(), infos.end(), [windowId](const sptr<Rosen::AccessibilityWindowInfo> info) {
        return windowId == info->wid_;
    });
    if ((iter == infos.end()) || (*iter == nullptr)) {
        SC_LOG_WARN(LABEL, "Cannot find AccessibilityWindowInfo, return default scale");
        return scale;
    }
    scale = (*iter)->scaleVal_;
    SC_LOG_INFO(LABEL, "Get scale %{public}f", scale);
    return scale;
}

static bool IsRectInWindRect(const Rosen::Rect& windRect, const SecCompRect& secRect)
{
    // left or right
    if ((secRect.x_ + secRect.width_ <= windRect.posX_) || (secRect.x_ >= windRect.posX_ + windRect.width_)) {
        return false;
    }
    // top or bottom
    if ((secRect.y_ + secRect.height_ <= windRect.posY_) || (secRect.y_ >= windRect.posY_ + windRect.height_)) {
        return false;
    }

    return true;
}

bool WindowInfoHelper::CheckOtherWindowCoverComp(int32_t compWinId, const SecCompRect& secRect)
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
        }
    }

    if (compLayer == INVALID_WINDOW_LAYER) {
        SC_LOG_ERROR(LABEL, "windId %{public}d component layer is wrong", compWinId);
        return false;
    }

    if (layerList.size() == static_cast<size_t>(0)) {
        return true;
    }

    auto iter = std::find_if(layerList.begin(), layerList.end(),
        [compLayer](const std::pair<int32_t, int32_t> layer) {
        return layer.second >= compLayer;
    });
    if (iter != layerList.end()) {
        SC_LOG_ERROR(LABEL, "component window %{public}d is covered by %{public}d, click check failed",
            compWinId, iter->first);
        return false;
    }
    return true;
}
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
