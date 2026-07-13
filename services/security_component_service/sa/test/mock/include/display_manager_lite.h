/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef SECURITY_COMPONENT_MANAGER_DISPLAY_MANAGER_LITE_MOCK_H
#define SECURITY_COMPONENT_MANAGER_DISPLAY_MANAGER_LITE_MOCK_H

#include "display_info.h"
#include "display_lite.h"
#include "dm_common.h"

namespace OHOS::Rosen {
class DisplayManagerLite {
public:
    static DisplayManagerLite& GetInstance()
    {
        static DisplayManagerLite instance;
        return instance;
    }

    sptr<DisplayLite> GetDisplayById(uint64_t displayId)
    {
        return sptr<DisplayLite>::MakeSptr();
    }

    sptr<DisplayInfo> GetCurrentFoldCreaseRegion()
    {
        return sptr<DisplayInfo>::MakeSptr();
    }
};
}

#endif // SECURITY_COMPONENT_MANAGER_DISPLAY_MANAGER_LITE_MOCK_H
