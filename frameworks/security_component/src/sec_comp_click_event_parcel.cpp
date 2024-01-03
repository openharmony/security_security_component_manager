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
#include "sec_comp_click_event_parcel.h"

#include "sec_comp_log.h"
#include "securec.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
    LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "SecCompClickEventParcel"};
}

bool SecCompClickEventParcel::MarshallingPointEvent(Parcel& out) const
{
    if (!(out.WriteDouble(this->clickInfoParams_.point.touchX)) ||
        !(out.WriteDouble(this->clickInfoParams_.point.touchY))) {
        SC_LOG_ERROR(LABEL, "Write touch xy pointer fail");
        return false;
    }

    if (!(out.WriteUint64(this->clickInfoParams_.point.timestamp))) {
        SC_LOG_ERROR(LABEL, "Write touch timestamp fail");
        return false;
    }
    return true;
}

bool SecCompClickEventParcel::MarshallingKeyEvent(Parcel& out) const
{
    if (!(out.WriteUint64(this->clickInfoParams_.key.timestamp))) {
        SC_LOG_ERROR(LABEL, "Write key timestamp fail");
        return false;
    }

    if (!(out.WriteInt32(this->clickInfoParams_.key.keyCode))) {
        SC_LOG_ERROR(LABEL, "Write key code fail");
        return false;
    }
    return true;
}

bool SecCompClickEventParcel::Marshalling(Parcel& out) const
{
    if (!(out.WriteInt32(static_cast<int32_t>(this->clickInfoParams_.type)))) {
        SC_LOG_ERROR(LABEL, "Write click type fail");
        return false;
    }

    if (this->clickInfoParams_.type == ClickEventType::POINT_EVENT_TYPE) {
        if (!MarshallingPointEvent(out)) {
            return false;
        }
    } else if (this->clickInfoParams_.type == ClickEventType::KEY_EVENT_TYPE) {
        if (!MarshallingKeyEvent(out)) {
            return false;
        }
    } else {
        SC_LOG_ERROR(LABEL, "click type %{public}d is error", this->clickInfoParams_.type);
        return false;
    }

    if (!(out.WriteUint32(this->clickInfoParams_.extraInfo.dataSize))) {
        SC_LOG_ERROR(LABEL, "Write extraInfo dataSize fail");
        return false;
    }
    if (this->clickInfoParams_.extraInfo.dataSize != 0 &&
        !(out.WriteBuffer(this->clickInfoParams_.extraInfo.data, this->clickInfoParams_.extraInfo.dataSize))) {
        SC_LOG_ERROR(LABEL, "Write click extraInfo data fail");
        return false;
    }

    return true;
}

bool SecCompClickEventParcel::UnmarshallingPointEvent(Parcel& in, SecCompClickEvent& clickInfo)
{
    if (!in.ReadDouble(clickInfo.point.touchX) || !in.ReadDouble(clickInfo.point.touchY)) {
        SC_LOG_ERROR(LABEL, "Read touch xy porinter fail");
        return false;
    }

    if (!in.ReadUint64(clickInfo.point.timestamp)) {
        SC_LOG_ERROR(LABEL, "Read timestamp fail");
        return false;
    }
    return true;
}

bool SecCompClickEventParcel::UnmarshallingKeyEvent(Parcel& in, SecCompClickEvent& clickInfo)
{
    if (!in.ReadUint64(clickInfo.key.timestamp)) {
        SC_LOG_ERROR(LABEL, "Read timestamp fail");
        return false;
    }

    if (!in.ReadInt32(clickInfo.key.keyCode)) {
        SC_LOG_ERROR(LABEL, "Read keyCode fail");
        return false;
    }
    return true;
}

SecCompClickEventParcel* SecCompClickEventParcel::Unmarshalling(Parcel& in)
{
    SecCompClickEventParcel* clickInfoParcel = new (std::nothrow) SecCompClickEventParcel();
    if (clickInfoParcel == nullptr) {
        return nullptr;
    }

    SecCompClickEvent clickInfo;
    int32_t type;
    if (!in.ReadInt32(type)) {
        SC_LOG_ERROR(LABEL, "Read click type fail");
        delete clickInfoParcel;
        return nullptr;
    }
    clickInfo.type = static_cast<ClickEventType>(type);

    if (clickInfo.type == ClickEventType::POINT_EVENT_TYPE) {
        if (!UnmarshallingPointEvent(in, clickInfo)) {
            delete clickInfoParcel;
            return nullptr;
        }
    } else if (clickInfo.type == ClickEventType::KEY_EVENT_TYPE) {
        if (!UnmarshallingKeyEvent(in, clickInfo)) {
            delete clickInfoParcel;
            return nullptr;
        }
    } else {
        SC_LOG_ERROR(LABEL, "click type %{public}d is error", clickInfo.type);
        delete clickInfoParcel;
        return nullptr;
    }

    if (!in.ReadUint32(clickInfo.extraInfo.dataSize)) {
        SC_LOG_ERROR(LABEL, "Read extraInfo data size fail");
        delete clickInfoParcel;
        return nullptr;
    }

    if (clickInfo.extraInfo.dataSize == 0) {
        clickInfoParcel->clickInfoParams_ = clickInfo;
        return clickInfoParcel;
    } else if (clickInfo.extraInfo.dataSize > MAX_EXTRA_SIZE) {
        SC_LOG_ERROR(LABEL, "Read extraInfo data size invalid");
        delete clickInfoParcel;
        return nullptr;
    }

    clickInfo.extraInfo.data = const_cast<uint8_t*>(in.ReadBuffer(clickInfo.extraInfo.dataSize));
    if (clickInfo.extraInfo.data == nullptr) {
        SC_LOG_ERROR(LABEL, "Read extraInfo data failed");
        delete clickInfoParcel;
        return nullptr;
    }

    clickInfoParcel->clickInfoParams_ = clickInfo;
    return clickInfoParcel;
}
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
