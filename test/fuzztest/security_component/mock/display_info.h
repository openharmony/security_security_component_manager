/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef SECURITY_COMPONENT_MANAGER_DISPLAY_INFO_MOCK_H
#define SECURITY_COMPONENT_MANAGER_DISPLAY_INFO_MOCK_H
#include <parcel.h>

namespace OHOS::Rosen {
namespace {
static const int32_t DEFAULT_SCREEN_SIZE = 128;
};

class DisplayInfo : public Parcelable {
public:
    bool Marshalling(Parcel &parcel) const override
    {
        return true;
    }

    int32_t GetWidth() const
    {
        return DEFAULT_SCREEN_SIZE;
    }

    int32_t GetHeight() const
    {
        return DEFAULT_SCREEN_SIZE;
    }
};
}
#endif // SECURITY_COMPONENT_MANAGER_DISPLAY_INFO_MOCK_H