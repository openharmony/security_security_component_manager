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
#include "sec_comp_perm_manager_test.h"

#include "accesstoken_kit.h"
#include "sec_comp_err.h"
#include "sec_comp_info_helper.h"
#include "sec_comp_log.h"
#include "service_test_common.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Security::SecurityComponent;
using namespace OHOS::Security::AccessToken;

namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
    LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "SecCompPermManagerTest"};
}

void SecCompPermManagerTest::SetUpTestCase()
{}

void SecCompPermManagerTest::TearDownTestCase()
{}

void SecCompPermManagerTest::SetUp()
{
    SC_LOG_INFO(LABEL, "setup");
}

void SecCompPermManagerTest::TearDown()
{
}

/**
 * @tc.name: DelaySaveRevokePermission001
 * @tc.desc: Test invalid params
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompPermManagerTest, DelaySaveRevokePermission001, TestSize.Level1)
{
    SecCompPermManager permMgr;
    permMgr.secHandler_ = nullptr;
    ASSERT_FALSE(permMgr.DelaySaveRevokePermission(static_cast<AccessTokenID>(0), ""));
}

/**
 * @tc.name: RevokeSavePermissionTask001
 * @tc.desc: Test invalid params
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompPermManagerTest, RevokeSavePermissionTask001, TestSize.Level1)
{
    SecCompPermManager permMgr;
    permMgr.secHandler_ = nullptr;
    ASSERT_FALSE(permMgr.RevokeSavePermissionTask(""));
}

/**
 * @tc.name: GrantTempSavePermission001
 * @tc.desc: Test invalid params
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompPermManagerTest, GrantTempSavePermission001, TestSize.Level1)
{
    SecCompPermManager permMgr;
    permMgr.secHandler_ = nullptr;
    ASSERT_EQ(permMgr.GrantTempSavePermission(static_cast<AccessTokenID>(0)),
        SC_SERVICE_ERROR_PERMISSION_OPER_FAIL);
}

/**
 * @tc.name: RevokeTempSavePermissionCount001
 * @tc.desc: Test invalid params
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompPermManagerTest, RevokeTempSavePermissionCount001, TestSize.Level1)
{
    SecCompPermManager permMgr;
    permMgr.secHandler_ = nullptr;
    AccessTokenID id = 0;
    permMgr.RevokeTempSavePermissionCount(id);

    permMgr.applySaveCountMap_[id] = 1;
    permMgr.RevokeTempSavePermissionCount(static_cast<AccessTokenID>(0));
    ASSERT_EQ(permMgr.applySaveCountMap_[id], 1);
}

/**
 * @tc.name: RevokeTempSavePermission001
 * @tc.desc: Test invalid params
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompPermManagerTest, RevokeTempSavePermission001, TestSize.Level1)
{
    SecCompPermManager permMgr;
    permMgr.secHandler_ = nullptr;
    AccessTokenID id = 0;
    permMgr.saveTaskDequeMap_[id].push_back("test");
    permMgr.RevokeTempSavePermission(id);
    ASSERT_EQ(permMgr.saveTaskDequeMap_[id].size(), static_cast<size_t>(0));

    permMgr.secHandler_ = std::make_shared<SecEventHandler>(nullptr);
    permMgr.saveTaskDequeMap_[id].push_back("test1");
    permMgr.RevokeTempSavePermission(id);
    permMgr.CancelAppRevokingPermisions(id);
    ASSERT_EQ(permMgr.saveTaskDequeMap_[id].size(), static_cast<size_t>(0));
}

/**
 * @tc.name: RemoveAppGrantPermissionRecord001
 * @tc.desc: Test invalid params
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompPermManagerTest, RemoveAppGrantPermissionRecord001, TestSize.Level1)
{
    SecCompPermManager permMgr;
    AccessTokenID id = 0;
    permMgr.RemoveAppGrantPermissionRecord(id, "test");

    permMgr.grantMap_[id].insert("test");
    permMgr.RemoveAppGrantPermissionRecord(id, "test");
    ASSERT_EQ(permMgr.grantMap_[id].size(), static_cast<size_t>(0));
}

/**
 * @tc.name: RevokeAppPermission001
 * @tc.desc: Test invalid params
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompPermManagerTest, RevokeAppPermission001, TestSize.Level1)
{
    SecCompPermManager permMgr;
    AccessTokenID id = 0;
    ASSERT_EQ(permMgr.RevokeAppPermission(id, "test"), 0);
}

/**
 * @tc.name: VerifyPermission001
 * @tc.desc: Test VerifyPermission
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompPermManagerTest, VerifyPermission001, TestSize.Level1)
{
    SecCompPermManager permMgr;
    AccessTokenID id = 1000;
    ASSERT_FALSE(permMgr.VerifyPermission(id, LOCATION_COMPONENT));
    ASSERT_FALSE(permMgr.VerifyPermission(id, PASTE_COMPONENT));
    ASSERT_FALSE(permMgr.VerifyPermission(id, SAVE_COMPONENT));
    ASSERT_FALSE(permMgr.VerifyPermission(id, static_cast<SecCompType>(-1)));
}

/**
 * @tc.name: DLP-GrantTempPermission001
 * @tc.desc: Test DLP sandbox app grant save button
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompPermManagerTest, GrantTempPermission001, TestSize.Level1)
{
    OHOS::Security::AccessToken::AccessTokenIDInner tokenInner = {
        .tokenUniqueID = 0x00001,
        .res = 1,
        .dlpFlag = 1,
        .type = OHOS::Security::AccessToken::TOKEN_HAP,
        .version = 0,
    };
    OHOS::Security::AccessToken::AccessTokenID *tokenid =
        reinterpret_cast<OHOS::Security::AccessToken::AccessTokenID *>(&tokenInner);
    ASSERT_NE(0, *tokenid);
    SecCompPermManager permMgr;
    nlohmann::json jsonComponent;
    ServiceTestCommon::BuildSaveComponentJson(jsonComponent);
    std::string message;
    SecCompBase* comp = SecCompInfoHelper::ParseComponent(SAVE_COMPONENT, jsonComponent, message);
    ASSERT_NE(nullptr, comp);
    std::shared_ptr<SecCompBase> shared_comp(comp);

    EXPECT_EQ(SC_SERVICE_ERROR_PERMISSION_OPER_FAIL,
        permMgr.GrantTempPermission(*tokenid, shared_comp));
}