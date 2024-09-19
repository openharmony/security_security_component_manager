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
#include "first_use_dialog.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
void SecCompDialogSrvCallback::OnDialogClosed(int32_t result)
{
    scId_ = 0;
    return;
}

FirstUseDialog& FirstUseDialog::GetInstance()
{
    static FirstUseDialog instance;
    return instance;
}

bool FirstUseDialog::IsCfgDirExist(void)
{
    return true;
}

bool FirstUseDialog::IsCfgFileExist(void)
{
    return true;
}

bool FirstUseDialog::IsCfgFileValid(void)
{
    return true;
}

bool FirstUseDialog::ReadCfgContent(std::string& content)
{
    return true;
}

void FirstUseDialog::WriteCfgContent(const std::string& content)
{
    return;
}

bool FirstUseDialog::ParseRecord(nlohmann::json& jsonRes,
    AccessToken::AccessTokenID& id, uint64_t& type)
{
    return true;
}

void FirstUseDialog::ParseRecords(nlohmann::json& jsonRes)
{
    return;
}

void FirstUseDialog::LoadFirstUseRecord(void)
{
    return;
}

void FirstUseDialog::SaveFirstUseRecord(void)
{
    return;
}

void FirstUseDialog::RemoveDialogWaitEntitys(int32_t pid)
{
    return;
}

int32_t FirstUseDialog::GrantDialogWaitEntity(int32_t scId)
{
    return SC_OK;
}

void FirstUseDialog::StartDialogAbility(std::shared_ptr<SecCompEntity> entity,
    sptr<IRemoteObject> callerToken, sptr<IRemoteObject> dialogCallback)
{
    return;
}

void FirstUseDialog::SendSaveEventHandler(void)
{
    return;
}

bool FirstUseDialog::SetFirstUseMap(std::shared_ptr<SecCompEntity> entity)
{
    return true;
}

int32_t FirstUseDialog::NotifyFirstUseDialog(std::shared_ptr<SecCompEntity> entity,
    sptr<IRemoteObject> callerToken, sptr<IRemoteObject> dialogCallback)
{
    return SC_OK;
}

void FirstUseDialog::Init(std::shared_ptr<SecEventHandler> secHandler)
{
    secHandler_ = secHandler;
}
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS