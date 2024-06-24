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

#ifndef SECURITY_COMPONENT_MANAGER_FUZZ_COMMON_H
#define SECURITY_COMPONENT_MANAGER_FUZZ_COMMON_H
#include <string>
#include "nlohmann/json.hpp"
#include "securec.h"
#include "sec_comp_info.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
class CompoRandomGenerator {
public:
    CompoRandomGenerator(const uint8_t *data, const size_t size)
        : data_(data), dataLenth_(size)
    {}

    uint32_t GetScType();
    std::string GenerateRandomCompoStr(uint32_t type);
    std::string ConstructLocationJson();
    std::string ConstructSaveJson();
    std::string ConstructPasteJson();
    template <class T> T GetData()
    {
        T object{};
        size_t objectSize = sizeof(object);
        if (data_ == nullptr || objectSize > dataLenth_ - basePos) {
            basePos = 0; // reset read point
            return object; // return empty obj
        }
        errno_t ret = memcpy_s(&object, objectSize, data_ + basePos, objectSize);
        if (ret != EOK) {
            return {};
        }
        basePos += objectSize;
        return object;
    }
private:
    void ConstructRandomRect(SecCompRect &rect);
    void ConstructButtonRect(
        SecCompRect &window, PaddingSize &padding, SecCompRect &buttonRect);
    void ConstructWindowJson(nlohmann::json &jsonComponent,
        SecCompRect &window, PaddingSize &padding, SecCompRect &buttonRect);

    const uint8_t *data_;
    const size_t dataLenth_;
    size_t basePos = 0;
};
}
}
}
#endif // SECURITY_COMPONENT_MANAGER_FUZZ_COMMON_H