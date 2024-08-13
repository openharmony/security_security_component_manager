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
#include "i_sec_comp_service.h"
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
constexpr const char *SETTINGS_DATA_EXT_URI = "datashare:///com.ohos.settingsdata.DataAbility";
constexpr const char *SETTINGS_DATASHARE_URI =
    "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true";
constexpr const char *SETTINGS_DATASHARE_SEARCH_URI =
    "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?" \
    "Proxy=true&key=accessibility_screenreader_enabled";
constexpr const char *ADVANCED_DATA_COLUMN_KEYWORD = "KEYWORD";
constexpr const char *ADVANCED_DATA_COLUMN_VALUE = "VALUE";
constexpr const char *QUERY_KEYWORD = "accessibility_screenreader_enabled";
static bool IsScreenReadMode();
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
    if (!WindowInfoHelper::CheckOtherWindowCoverComp(componentInfo_->windowId_,
        componentInfo_->rect_)) {
        SC_LOG_ERROR(LABEL, "Component may be covered by other window");
        return SC_SERVICE_ERROR_CLICK_EVENT_INVALID;
    }

    int32_t res = SC_SERVICE_ERROR_CLICK_EVENT_INVALID;
    bool isScreenReadMode = IsScreenReadMode();
    if (clickInfo.type == ClickEventType::POINT_EVENT_TYPE && !isScreenReadMode) {
        res = CheckPointEvent(clickInfo);
    } else if (clickInfo.type == ClickEventType::POINT_EVENT_TYPE && isScreenReadMode) {
        SC_LOG_WARN(LABEL, "Device is in screen read mode, skip event check.");
        return SC_OK;
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

namespace {
static bool IsScreenReadMode()
{
    auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        SC_LOG_ERROR(LABEL, "Get sa manager is nullptr.");
        return false;
    }
    auto remoteObj = saManager->GetSystemAbility(SA_ID_SECURITY_COMPONENT_SERVICE);
    if (remoteObj == nullptr) {
        SC_LOG_ERROR(LABEL, "Get remoteObj is nullptr.");
        return false;
    }
    std::shared_ptr<DataShare::DataShareHelper> dataShareHelper =
        DataShare::DataShareHelper::Creator(remoteObj, SETTINGS_DATASHARE_URI, SETTINGS_DATA_EXT_URI);
    if (dataShareHelper == nullptr) {
        SC_LOG_ERROR(LABEL, "Get dataShareHelper is nullptr.");
        return false;
    }
    DataShare::DataSharePredicates predicates;
    std::vector<std::string> columns;
    predicates.EqualTo(ADVANCED_DATA_COLUMN_KEYWORD, QUERY_KEYWORD);
    OHOS::Uri uri(SETTINGS_DATASHARE_SEARCH_URI);
    auto result = dataShareHelper->Query(uri, predicates, columns);
    if (result == nullptr) {
        SC_LOG_ERROR(LABEL, "Query result is nullptr.");
        dataShareHelper->Release();
        return false;
    }
    if (result->GoToFirstRow() != DataShare::E_OK) {
        SC_LOG_ERROR(LABEL, "Query failed, GoToFirstRow error.");
        result->Close();
        dataShareHelper->Release();
        return false;
    }
    int32_t columnIndex;
    result->GetColumnIndex(ADVANCED_DATA_COLUMN_VALUE, columnIndex);
    std::string value;
    result->GetString(columnIndex, value);
    result->Close();
    dataShareHelper->Release();
    return value == "1";
}
}
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
