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
#ifndef SECURITY_COMPONENT_MOCK_ABILITY_RUNTIME_APPLICATION_STATE_OBSERVER_STUB_H
#define SECURITY_COMPONENT_MOCK_ABILITY_RUNTIME_APPLICATION_STATE_OBSERVER_STUB_H

#include <map>
#include <mutex>

#include "iremote_stub.h"
#include "nocopyable.h"
#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {
enum class AppProcessState {
    APP_STATE_BEGIN = 0,
    APP_STATE_CREATE = APP_STATE_BEGIN,
    APP_STATE_READY,
    APP_STATE_FOREGROUND,
    APP_STATE_FOCUS,
    APP_STATE_BACKGROUND,
    APP_STATE_TERMINATED,
    APP_STATE_END,
};

struct ProcessData {
    std::string bundleName;
    int32_t pid = 0;
    int32_t uid = 0;
    AppProcessState state;
    bool isContinuousTask = false;
    bool isKeepAlive = false;
    bool isFocused = false;
    int32_t requestProcCode = 0;
};

struct AppStateData {
    std::string bundleName;
    int32_t pid = -1;
    int32_t uid = 0;
    int32_t state = 0;
    int32_t accessTokenId = 0;
    bool isFocused = false;
};

class IApplicationStateObserver : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.appexecfwk.IApplicationStateObserver");
    virtual void OnProcessStateChanged(const ProcessData &processData) {}
    virtual void OnProcessDied(const ProcessData &processData) = 0;
    virtual void OnAppCacheStateChanged(const AppStateData &appStateData) = 0;
};

class ApplicationStateObserverStub : public IRemoteStub<IApplicationStateObserver> {
public:
    ApplicationStateObserverStub() = default;
    virtual ~ApplicationStateObserverStub() = default;
    DISALLOW_COPY_AND_MOVE(ApplicationStateObserverStub);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // SECURITY_COMPONENT_MOCK_ABILITY_RUNTIME_APPLICATION_STATE_OBSERVER_STUB_H
