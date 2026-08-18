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
#include "wm_mini_client_adapter.h"

#include <new>

#if defined(TDD_ENABLE)
#include "window_manager.h"
#elif defined(WM_MINI_CLIENT_IPC_TEST)
#include "wm_mini_client_environment.h"
#include "wm_mini_client_test.h"
#endif

namespace OHOS {
namespace Rosen {
using Security::SecurityComponent::MiniAccessibilityWindowInfo;
using Security::SecurityComponent::MiniRect;
using Security::SecurityComponent::MiniUnreliableWindowInfo;
using Security::SecurityComponent::MiniWMError;

// Each test target selects exactly one implementation mode.
#if defined(TDD_ENABLE)
namespace {
void CopyRect(const Rect& source, MiniRect& target)
{
    target.posX_ = source.posX_;
    target.posY_ = source.posY_;
    target.width_ = source.width_;
    target.height_ = source.height_;
}

bool ConvertWindowInfo(const sptr<AccessibilityWindowInfo>& source,
    std::vector<sptr<MiniAccessibilityWindowInfo>>& infos)
{
    if (source == nullptr) {
        infos.emplace_back(nullptr);
        return true;
    }
    sptr<MiniAccessibilityWindowInfo> target = new (std::nothrow) MiniAccessibilityWindowInfo();
    if (target == nullptr) {
        return false;
    }
    target->wid_ = source->wid_;
    target->innerWid_ = source->innerWid_;
    target->uiNodeId_ = source->uiNodeId_;
    CopyRect(source->windowRect_, target->windowRect_);
    target->focused_ = source->focused_;
    target->isDecorEnable_ = source->isDecorEnable_;
    target->displayId_ = source->displayId_;
    target->layer_ = source->layer_;
    target->mode_ = static_cast<uint32_t>(source->mode_);
    target->type_ = static_cast<uint32_t>(source->type_);
    target->scaleVal_ = source->scaleVal_;
    target->scaleX_ = source->scaleX_;
    target->scaleY_ = source->scaleY_;
    target->isCompatScaleMode_ = source->isCompatScaleMode_;
    CopyRect(source->scaleRect_, target->scaleRect_);
    target->bundleName_ = source->bundleName_;
    target->touchHotAreas_.reserve(source->touchHotAreas_.size());
    for (const auto& sourceRect : source->touchHotAreas_) {
        MiniRect targetRect;
        CopyRect(sourceRect, targetRect);
        target->touchHotAreas_.emplace_back(targetRect);
    }
    infos.emplace_back(target);
    return true;
}

bool ConvertUnreliableWindowInfo(const sptr<UnreliableWindowInfo>& source,
    std::vector<sptr<MiniUnreliableWindowInfo>>& infos)
{
    if (source == nullptr) {
        infos.emplace_back(nullptr);
        return true;
    }
    sptr<MiniUnreliableWindowInfo> target = new (std::nothrow) MiniUnreliableWindowInfo();
    if (target == nullptr) {
        return false;
    }
    target->windowId_ = source->windowId_;
    CopyRect(source->windowRect_, target->windowRect_);
    target->zOrder_ = source->zOrder_;
    target->floatingScale_ = source->floatingScale_;
    target->scaleX_ = source->scaleX_;
    target->scaleY_ = source->scaleY_;
    infos.emplace_back(target);
    return true;
}
}

MiniWMError WmMiniClientAdapter::GetWindowInfo(int32_t userId,
    std::vector<sptr<MiniAccessibilityWindowInfo>>& infos)
{
    std::vector<sptr<AccessibilityWindowInfo>> sourceInfos;
    auto ret = WindowManager::GetInstance(userId).GetAccessibilityWindowInfo(sourceInfos);
    infos.clear();
    infos.reserve(sourceInfos.size());
    for (const auto& source : sourceInfos) {
        if (!ConvertWindowInfo(source, infos)) {
            infos.clear();
            return MiniWMError::WM_ERROR_NO_MEM;
        }
    }
    return static_cast<MiniWMError>(ret);
}

MiniWMError WmMiniClientAdapter::GetUnreliableWindowInfo(int32_t windowId, int32_t userId,
    std::vector<sptr<MiniUnreliableWindowInfo>>& infos)
{
    std::vector<sptr<UnreliableWindowInfo>> sourceInfos;
    auto ret = WindowManager::GetInstance(userId).GetUnreliableWindowInfo(windowId, sourceInfos);
    infos.clear();
    infos.reserve(sourceInfos.size());
    for (const auto& source : sourceInfos) {
        if (!ConvertUnreliableWindowInfo(source, infos)) {
            infos.clear();
            return MiniWMError::WM_ERROR_NO_MEM;
        }
    }
    return static_cast<MiniWMError>(ret);
}

MiniAccessibilityWindowInfo* WmMiniClientAdapter::CreateWindowInfo()
{
    return new (std::nothrow) MiniAccessibilityWindowInfo();
}

MiniUnreliableWindowInfo* WmMiniClientAdapter::CreateUnreliableWindowInfo()
{
    return new (std::nothrow) MiniUnreliableWindowInfo();
}
#elif defined(FUZZ_ENABLE)
namespace {
constexpr uint32_t FUZZ_WINDOW_COVERAGE_MASK = 0x20000000;
constexpr int32_t FUZZ_COVERED_WINDOW_ID_OFFSET = 1;
constexpr uint32_t FUZZ_COVERED_WINDOW_LAYER = 1;
constexpr uint32_t FUZZ_COVERED_WINDOW_RECT_SIZE = 96;
constexpr float FUZZ_COVERED_WINDOW_FLOATING_SCALE = 0.5F;

sptr<MiniUnreliableWindowInfo> CreateMiniUnreliableWindowInfo(int32_t windowId)
{
    sptr<MiniUnreliableWindowInfo> info = new (std::nothrow) MiniUnreliableWindowInfo();
    if (info == nullptr) {
        return nullptr;
    }
    info->windowId_ = windowId;
    return info;
}

sptr<MiniUnreliableWindowInfo> CreateCoveredWindowInfo(int32_t windowId)
{
    sptr<MiniUnreliableWindowInfo> info = CreateMiniUnreliableWindowInfo(windowId + FUZZ_COVERED_WINDOW_ID_OFFSET);
    if (info == nullptr) {
        return nullptr;
    }
    info->zOrder_ = FUZZ_COVERED_WINDOW_LAYER;
    info->windowRect_.width_ = FUZZ_COVERED_WINDOW_RECT_SIZE;
    info->windowRect_.height_ = FUZZ_COVERED_WINDOW_RECT_SIZE;
    info->floatingScale_ = FUZZ_COVERED_WINDOW_FLOATING_SCALE;
    return info;
}
}

MiniWMError WmMiniClientAdapter::GetWindowInfo(int32_t userId,
    std::vector<sptr<MiniAccessibilityWindowInfo>>& infos)
{
    (void)userId;
    infos.clear();
    sptr<MiniAccessibilityWindowInfo> info = new (std::nothrow) MiniAccessibilityWindowInfo();
    if (info == nullptr) {
        return MiniWMError::WM_ERROR_NO_MEM;
    }
    info->scaleVal_ = 1.0F;
    info->scaleX_ = 1.0F;
    info->scaleY_ = 1.0F;
    infos.emplace_back(info);
    return MiniWMError::WM_OK;
}

MiniWMError WmMiniClientAdapter::GetUnreliableWindowInfo(int32_t windowId, int32_t userId,
    std::vector<sptr<MiniUnreliableWindowInfo>>& infos)
{
    (void)userId;
    infos.clear();
    sptr<MiniUnreliableWindowInfo> componentWindow = CreateMiniUnreliableWindowInfo(windowId);
    if (componentWindow == nullptr) {
        return MiniWMError::WM_ERROR_NO_MEM;
    }
    infos.emplace_back(componentWindow);

    if ((static_cast<uint32_t>(windowId) & FUZZ_WINDOW_COVERAGE_MASK) == 0) {
        return MiniWMError::WM_OK;
    }

    sptr<MiniUnreliableWindowInfo> coveredWindow = CreateCoveredWindowInfo(windowId);
    if (coveredWindow == nullptr) {
        return MiniWMError::WM_ERROR_NO_MEM;
    }
    infos.emplace_back(coveredWindow);
    return MiniWMError::WM_OK;
}

MiniAccessibilityWindowInfo* WmMiniClientAdapter::CreateWindowInfo()
{
    return new (std::nothrow) MiniAccessibilityWindowInfo();
}

MiniUnreliableWindowInfo* WmMiniClientAdapter::CreateUnreliableWindowInfo()
{
    return new (std::nothrow) MiniUnreliableWindowInfo();
}
#elif defined(WM_MINI_CLIENT_IPC_TEST)
namespace {
constexpr int32_t WRITE_FAIL_NONE = 0;
sptr<IRemoteObject> g_wmsRemoteForTest = nullptr;
bool g_multiInstanceEnabledForTest = false;
int32_t g_writeFailurePointForTest = WRITE_FAIL_NONE;
bool g_forceMiniInfoAllocFailForTest = false;
bool g_forceParcelableVectorSizeOverLimitForTest = false;
bool g_forceParcelableVectorResizeFailForTest = false;
}

sptr<IRemoteObject> WmMiniClientEnvironment::GetWindowManagerServiceRemote()
{
    return g_wmsRemoteForTest;
}

bool WmMiniClientEnvironment::IsMultiInstanceEnabled()
{
    return g_multiInstanceEnabledForTest;
}

bool WmMiniClientEnvironment::WriteInterfaceToken(
    MessageParcel& parcel, const std::u16string& descriptor, WmMiniParcelField field)
{
    if (g_writeFailurePointForTest == static_cast<int32_t>(field)) {
        return false;
    }
    return parcel.WriteInterfaceToken(descriptor);
}

bool WmMiniClientEnvironment::WriteInt32(MessageParcel& parcel, int32_t value, WmMiniParcelField field)
{
    if (g_writeFailurePointForTest == static_cast<int32_t>(field)) {
        return false;
    }
    return parcel.WriteInt32(value);
}

MiniAccessibilityWindowInfo* WmMiniClientEnvironment::CreateWindowInfo()
{
    if (g_forceMiniInfoAllocFailForTest) {
        return nullptr;
    }
    return new (std::nothrow) MiniAccessibilityWindowInfo();
}

MiniUnreliableWindowInfo* WmMiniClientEnvironment::CreateUnreliableWindowInfo()
{
    if (g_forceMiniInfoAllocFailForTest) {
        return nullptr;
    }
    return new (std::nothrow) MiniUnreliableWindowInfo();
}

bool WmMiniClientEnvironment::IsParcelableVectorSizeValid(size_t size, size_t maxSize)
{
    return !g_forceParcelableVectorSizeOverLimitForTest && size <= maxSize;
}

bool WmMiniClientEnvironment::ResizeParcelableVector(
    std::vector<sptr<MiniAccessibilityWindowInfo>>& infos, size_t size)
{
    if (g_forceParcelableVectorResizeFailForTest) {
        infos.clear();
        return false;
    }
    infos.resize(size);
    return infos.size() >= size;
}

bool WmMiniClientEnvironment::ResizeParcelableVector(
    std::vector<sptr<MiniUnreliableWindowInfo>>& infos, size_t size)
{
    if (g_forceParcelableVectorResizeFailForTest) {
        infos.clear();
        return false;
    }
    infos.resize(size);
    return infos.size() >= size;
}

void WmMiniClientTest::SetWmsRemote(const sptr<IRemoteObject>& remote)
{
    g_wmsRemoteForTest = remote;
}

void WmMiniClientTest::SetMultiInstanceEnabled(bool enabled)
{
    g_multiInstanceEnabledForTest = enabled;
}

void WmMiniClientTest::SetWriteFailurePoint(int32_t failPoint)
{
    g_writeFailurePointForTest = failPoint;
}

void WmMiniClientTest::SetMiniInfoAllocFail(bool forceFail)
{
    g_forceMiniInfoAllocFailForTest = forceFail;
}

void WmMiniClientTest::SetParcelableVectorSizeOverLimit(bool forceFail)
{
    g_forceParcelableVectorSizeOverLimitForTest = forceFail;
}

void WmMiniClientTest::SetParcelableVectorResizeFail(bool forceFail)
{
    g_forceParcelableVectorResizeFailForTest = forceFail;
}
#endif
}  // namespace Rosen
}  // namespace OHOS
