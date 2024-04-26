/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "registersecuritycomponent_fuzzer.h"

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include "accesstoken_kit.h"
#include "securec.h"
#include "token_setproc.h"
#include "nlohmann/json.hpp"
#include "sec_comp_base.h"

using namespace OHOS::Security::SecurityComponent;
using namespace OHOS::Security::AccessToken;
namespace OHOS {
const uint8_t *BASE_FUZZ_DATA = nullptr;
size_t g_baseFuzzSize = 0;
size_t g_baseFuzzPos;

template <class T> T GetData()
{
    T object{};
    size_t objectSize = sizeof(object);
    if (BASE_FUZZ_DATA == nullptr || objectSize > g_baseFuzzSize - g_baseFuzzPos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, BASE_FUZZ_DATA + g_baseFuzzPos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_baseFuzzPos += objectSize;
    return object;
}

void BuildLocationComponentInfo(std::string& componentInfo)
{
    nlohmann::json jsonComponent;
    jsonComponent[JsonTagConstants::JSON_SC_TYPE] = GetData<SecCompType>();
    jsonComponent[JsonTagConstants::JSON_RECT] = nlohmann::json {
        {JsonTagConstants::JSON_RECT_X, GetData<DimensionT>() },
        {JsonTagConstants::JSON_RECT_Y, GetData<SecCompType>() },
        {JsonTagConstants::JSON_RECT_WIDTH, GetData<SecCompType>() },
        {JsonTagConstants::JSON_RECT_HEIGHT, GetData<SecCompType>() }
    };
    jsonComponent[JsonTagConstants::JSON_NODE_ID] = 0;
    jsonComponent[JsonTagConstants::JSON_WINDOW_RECT] = nlohmann::json {
        {JsonTagConstants::JSON_RECT_X, GetData<DimensionT>() },
        {JsonTagConstants::JSON_RECT_Y, GetData<DimensionT>() },
        {JsonTagConstants::JSON_RECT_WIDTH, GetData<DimensionT>() },
        {JsonTagConstants::JSON_RECT_HEIGHT, GetData<DimensionT>() }
    };
    nlohmann::json jsonPadding = nlohmann::json {
        { JsonTagConstants::JSON_PADDING_TOP_TAG, GetData<DimensionT>() },
        { JsonTagConstants::JSON_PADDING_RIGHT_TAG, GetData<DimensionT>() },
        { JsonTagConstants::JSON_PADDING_BOTTOM_TAG, GetData<DimensionT>() },
        { JsonTagConstants::JSON_PADDING_LEFT_TAG, GetData<DimensionT>() },
    };

    jsonComponent[JsonTagConstants::JSON_SIZE_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_FONT_SIZE_TAG, GetData<DimensionT>() },
        { JsonTagConstants::JSON_ICON_SIZE_TAG, GetData<DimensionT>() },
        { JsonTagConstants::JSON_TEXT_ICON_PADDING_TAG, GetData<DimensionT>() },
        { JsonTagConstants::JSON_PADDING_SIZE_TAG, GetData<DimensionT>() },
    };

    jsonComponent[JsonTagConstants::JSON_COLORS_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_FONT_COLOR_TAG, GetData<uint32_t>() },
        { JsonTagConstants::JSON_ICON_COLOR_TAG, GetData<uint32_t>() },
        { JsonTagConstants::JSON_BG_COLOR_TAG, GetData<uint32_t>() }
    };

    jsonComponent[JsonTagConstants::JSON_BORDER_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_BORDER_WIDTH_TAG, GetData<DimensionT>() },
    };
    jsonComponent[JsonTagConstants::JSON_PARENT_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_PARENT_EFFECT_TAG, false },
    };
    jsonComponent[JsonTagConstants::JSON_STYLE_TAG] = nlohmann::json {
        { JsonTagConstants::JSON_TEXT_TAG, GetData<int32_t>() },
        { JsonTagConstants::JSON_ICON_TAG, GetData<int32_t>() },
        { JsonTagConstants::JSON_BG_TAG, GetData<SecCompBackground>() },
    };
    jsonComponent[JsonTagConstants::JSON_WINDOW_ID] = 0;
    componentInfo = jsonComponent.dump();
}

static void RegisterSecurityComponentFuzzTest(const uint8_t *data, size_t size)
{
    BASE_FUZZ_DATA = data;
    g_baseFuzzSize = size;
    g_baseFuzzPos = 0;
    std::string componentInfo;
    BuildLocationComponentInfo(componentInfo);
    int32_t scId = GetData<int32_t>();
    enum SecCompType type = GetData<SecCompType>();
    SecCompKit::RegisterSecurityComponent(type, componentInfo, scId);
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::RegisterSecurityComponentFuzzTest(data, size);
    return 0;
}
