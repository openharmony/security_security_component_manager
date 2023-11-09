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
#ifndef SECURITY_COMPONENT_CLICK_INFO_PARCEL_H
#define SECURITY_COMPONENT_CLICK_INFO_PARCEL_H

#include "parcel.h"
#include "sec_comp_info.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
struct SecCompClickEventParcel final : public Parcelable {
    SecCompClickEventParcel() = default;

    ~SecCompClickEventParcel() override = default;

    bool MarshallingPointEvent(Parcel& out) const;
    bool MarshallingKeyEvent(Parcel& out) const;
    bool Marshalling(Parcel& out) const override;

    static bool UnmarshallingPointEvent(Parcel& in, SecCompClickEvent& clickInfo);
    static bool UnmarshallingKeyEvent(Parcel& in, SecCompClickEvent& clickInfo);
    static SecCompClickEventParcel* Unmarshalling(Parcel& in);

    SecCompClickEvent clickInfoParams_;
};
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
#endif  // SECURITY_COMPONENT_CLICK_INFO_PARCEL_H
