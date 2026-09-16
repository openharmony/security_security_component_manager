/*
 * Copyright (c) 2025-2026 Huawei Device Co., Ltd.
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
#include "fold_screen_info.h"

namespace OHOS::Rosen {
/*
 * Configurable display manager lite mock: tests can switch GetDisplayById
 * between valid display, null display and null display info, and provide
 * fold crease rects, so every branch of the ui adapter impl is reachable.
 */
class DisplayManagerLite {
public:
    static DisplayManagerLite& GetInstance()
    {
        static DisplayManagerLite instance;
        return instance;
    }

    sptr<DisplayLite> GetDisplayById(DisplayId displayId)
    {
        if (displayId_ == DISPLAY_NULL) {
            return nullptr;
        }
        if (displayId_ == DISPLAY_INFO_NULL) {
            return sptr<DisplayLite>(new DisplayLite(true));
        }
        return sptr<DisplayLite>::MakeSptr();
    }

    sptr<FoldCreaseRegion> GetCurrentFoldCreaseRegion()
    {
        if (creaseMode_ == CREASE_NULL) {
            return nullptr;
        }
        return sptr<FoldCreaseRegion>::MakeSptr();
    }

    enum DisplayMockMode {
        DISPLAY_VALID = 0,
        DISPLAY_NULL,
        DISPLAY_INFO_NULL,
    };

    enum CreaseMockMode {
        CREASE_EMPTY = 0,
        CREASE_NULL,
        CREASE_VALID,
    };

    static void SetDisplayMode(DisplayMockMode mode)
    {
        displayId_ = mode;
    }

    static void SetCreaseMode(CreaseMockMode mode)
    {
        creaseMode_ = mode;
    }

    static void ResetMock()
    {
        displayId_ = DISPLAY_VALID;
        creaseMode_ = CREASE_EMPTY;
    }

private:
    static int32_t displayId_;
    static int32_t creaseMode_;
};
}  // namespace OHOS::Rosen

#endif // SECURITY_COMPONENT_MANAGER_DISPLAY_MANAGER_LITE_MOCK_H
