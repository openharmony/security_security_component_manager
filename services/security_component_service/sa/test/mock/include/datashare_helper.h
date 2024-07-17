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

#ifndef SEC_COMP_MOCK_DATASHARE_HELPER_H
#define SEC_COMP_MOCK_DATASHARE_HELPER_H
#include <string>
#include "if_system_ability_manager.h"
#include "iremote_object.h"
#include "uri.h"

namespace OHOS {
namespace DataShare {
static const int32_t E_OK = 0;

class DataShareResultSet {
public:
    int32_t GoToFirstRow()
    {
        return DataShare::E_OK;
    }

    int32_t GetColumnIndex(const std::string &columnName, int32_t &columnIndex)
    {
        return DataShare::E_OK;
    }

    int32_t GetString(int32_t columnIndex, std::string &value)
    {
        value = "0";
        return DataShare::E_OK;
    }

    void Close()
    {
        return;
    }
};

class DataSharePredicates {
public:
    int32_t EqualTo(const std::string &keyWord, const std::string &value)
    {
        return DataShare::E_OK;
    }
};

class DataShareHelper {
public:
    static std::shared_ptr<DataShareHelper> Creator(
        const sptr<MockIRemoteObject> &token, const std::string &strUri, const std::string &extUri = "")
    {
        return std::make_shared<DataShareHelper>();
    }

    std::shared_ptr<DataShareResultSet> Query(Uri &uri, const DataSharePredicates &predicates,
        std::vector<std::string> &columns)
    {
        return std::make_shared<DataShareResultSet>();
    }

    void Release()
    {
        return;
    }
};
}
}
#endif // SEC_COMP_MOCK_DATASHARE_HELPER_H