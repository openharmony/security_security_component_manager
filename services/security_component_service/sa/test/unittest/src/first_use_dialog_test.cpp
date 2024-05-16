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
#include "first_use_dialog_test.h"

#include "accesstoken_kit.h"
#include "location_button.h"
#include "save_button.h"
#include "sec_comp_log.h"
#include "sec_comp_err.h"
#include "sec_event_handler.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Security::SecurityComponent;
using namespace OHOS::Security::AccessToken;

namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
    LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "FirstUseDialogTest"};
static const std::string SEC_COMP_SRV_CFG_PATH = "/data/service/el1/public/security_component_service";
static const std::string SEC_COMP_SRV_CFG_FILE = SEC_COMP_SRV_CFG_PATH + "/" + "first_use_record.json";
static const std::string SEC_COMP_SRV_CFG_BACK_FILE = SEC_COMP_SRV_CFG_PATH + "/" + "first_use_record.json.bak";
constexpr uint64_t LOCATION_BUTTON_FIRST_USE = 1 << 0;
constexpr uint64_t SAVE_BUTTON_FIRST_USE = 1 << 1;
}

void FirstUseDialogTest::SetUpTestCase()
{
}

void FirstUseDialogTest::TearDownTestCase()
{
}

void FirstUseDialogTest::SetUp()
{
    SC_LOG_INFO(LABEL, "setup");
    struct stat fstat = {};
    if (stat(SEC_COMP_SRV_CFG_FILE.c_str(), &fstat) != 0) {
        return;
    }
    std::string cmdline = "mv " + SEC_COMP_SRV_CFG_FILE + " " + SEC_COMP_SRV_CFG_BACK_FILE;
    system(cmdline.c_str());
}

void FirstUseDialogTest::TearDown()
{
    struct stat fstat = {};
    if (stat(SEC_COMP_SRV_CFG_BACK_FILE.c_str(), &fstat) != 0) {
        return;
    }
    std::string cmdline = "mv " + SEC_COMP_SRV_CFG_BACK_FILE + " " + SEC_COMP_SRV_CFG_FILE;
    system(cmdline.c_str());
}

/**
 * @tc.name: LoadFirstUseRecord001
 * @tc.desc: Test cfg file not exist
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FirstUseDialogTest, LoadFirstUseRecord001, TestSize.Level1)
{
    FirstUseDialog diag;
    diag.LoadFirstUseRecord();
    ASSERT_EQ(0, static_cast<int32_t>(diag.firstUseMap_.size()));
}

/**
 * @tc.name: LoadFirstUseRecord002
 * @tc.desc: Test cfg file too large
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FirstUseDialogTest, LoadFirstUseRecord002, TestSize.Level1)
{
    std::string cmdline = "dd if=/dev/random of=" + SEC_COMP_SRV_CFG_FILE + " bs=101k count=1";
    system(cmdline.c_str());
    FirstUseDialog diag;
    diag.LoadFirstUseRecord();
    ASSERT_EQ(0, static_cast<int32_t>(diag.firstUseMap_.size()));
}

/**
 * @tc.name: LoadFirstUseRecord003
 * @tc.desc: Test cfg file is not json
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FirstUseDialogTest, LoadFirstUseRecord003, TestSize.Level1)
{
    std::string cmdline = "echo {x=\\\' > " + SEC_COMP_SRV_CFG_FILE;
    system(cmdline.c_str());
    FirstUseDialog diag;
    diag.LoadFirstUseRecord();
    ASSERT_EQ(0, static_cast<int32_t>(diag.firstUseMap_.size()));
}

/**
 * @tc.name: LoadFirstUseRecord004
 * @tc.desc: Test cfg file has no tag named FIRST_USE_RECORD_TAG
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FirstUseDialogTest, LoadFirstUseRecord004, TestSize.Level1)
{
    std::string cmdline = "echo {\\\"x\\\":1} > " + SEC_COMP_SRV_CFG_FILE;
    system(cmdline.c_str());
    FirstUseDialog diag;
    diag.LoadFirstUseRecord();
    ASSERT_EQ(0, static_cast<int32_t>(diag.firstUseMap_.size()));
}

/**
 * @tc.name: LoadFirstUseRecord005
 * @tc.desc: Test cfg file has no tag named TOKEN_ID_TAG
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FirstUseDialogTest, LoadFirstUseRecord005, TestSize.Level1)
{
    std::string cmdline = "echo {\\\"FirstUseRecord\\\":[{\\\"CompType\\\":1}]} > "
         + SEC_COMP_SRV_CFG_FILE;
    system(cmdline.c_str());
    FirstUseDialog diag;
    diag.LoadFirstUseRecord();
    ASSERT_EQ(0, static_cast<int32_t>(diag.firstUseMap_.size()));
}

/**
 * @tc.name: LoadFirstUseRecord006
 * @tc.desc: Test cfg file TOKEN_ID_TAG is not number
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FirstUseDialogTest, LoadFirstUseRecord006, TestSize.Level1)
{
    std::string cmdline = "echo {\\\"FirstUseRecord\\\":[{\\\"CompType\\\":1,\\\"TokenId\\\":\\\"kk\\\"}]} > "
         + SEC_COMP_SRV_CFG_FILE;
    system(cmdline.c_str());
    FirstUseDialog diag;
    diag.LoadFirstUseRecord();
    ASSERT_EQ(0, static_cast<int32_t>(diag.firstUseMap_.size()));
}

/**
 * @tc.name: LoadFirstUseRecord007
 * @tc.desc: Test cfg file TOKEN_ID_TAG value is 0
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FirstUseDialogTest, LoadFirstUseRecord007, TestSize.Level1)
{
    std::string cmdline = "echo {\\\"FirstUseRecord\\\":[{\\\"CompType\\\":1,\\\"TokenId\\\":0}]} > "
         + SEC_COMP_SRV_CFG_FILE;
    system(cmdline.c_str());
    FirstUseDialog diag;
    diag.LoadFirstUseRecord();
    ASSERT_EQ(0, static_cast<int32_t>(diag.firstUseMap_.size()));
}

/**
 * @tc.name: LoadFirstUseRecord008
 * @tc.desc: Test cfg file COMP_TYPE_TAG is not exist
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FirstUseDialogTest, LoadFirstUseRecord008, TestSize.Level1)
{
    std::string cmdline = "echo {\\\"FirstUseRecord\\\":[{\\\"TokenId\\\":1}]} > "
         + SEC_COMP_SRV_CFG_FILE;
    system(cmdline.c_str());
    FirstUseDialog diag;
    diag.LoadFirstUseRecord();
    ASSERT_EQ(0, static_cast<int32_t>(diag.firstUseMap_.size()));
}

/**
 * @tc.name: LoadFirstUseRecord009
 * @tc.desc: Test cfg file COMP_TYPE_TAG is number
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FirstUseDialogTest, LoadFirstUseRecord009, TestSize.Level1)
{
    std::string cmdline = "echo {\\\"FirstUseRecord\\\":[{\\\"CompType\\\":\\\"k\\\",\\\"TokenId\\\":1}]} > "
         + SEC_COMP_SRV_CFG_FILE;
    system(cmdline.c_str());
    FirstUseDialog diag;
    diag.LoadFirstUseRecord();
    ASSERT_EQ(0, static_cast<int32_t>(diag.firstUseMap_.size()));
}

/**
 * @tc.name: LoadFirstUseRecord010
 * @tc.desc: Test cfg file is success
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FirstUseDialogTest, LoadFirstUseRecord010, TestSize.Level1)
{
    std::string cmdline = "echo {\\\"FirstUseRecord\\\":[{\\\"CompType\\\":1,\\\"TokenId\\\":1}]} > "
         + SEC_COMP_SRV_CFG_FILE;
    system(cmdline.c_str());
    FirstUseDialog diag;
    diag.LoadFirstUseRecord();
    ASSERT_EQ(1, static_cast<int32_t>(diag.firstUseMap_.size()));
    ASSERT_EQ(1, static_cast<int32_t>(diag.firstUseMap_[1]));
}

/**
 * @tc.name: SaveFirstUseRecord001
 * @tc.desc: Test cfg dir is not exist
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FirstUseDialogTest, SaveFirstUseRecord001, TestSize.Level1)
{
    std::string cmdline = "rm " + SEC_COMP_SRV_CFG_PATH + " -rf";
    system(cmdline.c_str());
    FirstUseDialog diag;
    diag.firstUseMap_[1] = 1;
    diag.SaveFirstUseRecord();
    diag.firstUseMap_.clear();
    diag.LoadFirstUseRecord();
    EXPECT_EQ(0, static_cast<int32_t>(diag.firstUseMap_.size()));
    std::string cmdline1 = "mkdir " + SEC_COMP_SRV_CFG_PATH + " 0750 security_component security_component";
    system(cmdline1.c_str());
}

/**
 * @tc.name: SaveFirstUseRecord002
 * @tc.desc: Test cfg dir is not dir
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FirstUseDialogTest, SaveFirstUseRecord002, TestSize.Level1)
{
    std::string cmdline = "rm " + SEC_COMP_SRV_CFG_PATH + " -rf && touch " + SEC_COMP_SRV_CFG_PATH;
    system(cmdline.c_str());
    FirstUseDialog diag;
    diag.firstUseMap_[1] = 1;
    diag.SaveFirstUseRecord();
    diag.firstUseMap_.clear();
    diag.LoadFirstUseRecord();
    EXPECT_EQ(0, static_cast<int32_t>(diag.firstUseMap_.size()));
    std::string cmdline1 = "rm " + SEC_COMP_SRV_CFG_PATH +  " && mkdir " +
        SEC_COMP_SRV_CFG_PATH + " 0750 security_component security_component";
    system(cmdline1.c_str());
}

/**
 * @tc.name: SaveFirstUseRecord003
 * @tc.desc: Test cfg file is not exist, tokenid is invalid.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FirstUseDialogTest, SaveFirstUseRecord003, TestSize.Level1)
{
    FirstUseDialog diag;
    OHOS::Security::AccessToken::AccessTokenKit::getHapTokenInfoRes = -1;
    diag.firstUseMap_[1] = 1;
    diag.SaveFirstUseRecord();
    diag.firstUseMap_.clear();
    diag.LoadFirstUseRecord();
    EXPECT_EQ(0, static_cast<int32_t>(diag.firstUseMap_.size()));
    OHOS::Security::AccessToken::AccessTokenKit::getHapTokenInfoRes = 0;
}

/**
 * @tc.name: SaveFirstUseRecord004
 * @tc.desc: Test cfg file is exist, save ok
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FirstUseDialogTest, SaveFirstUseRecord004, TestSize.Level1)
{
    FirstUseDialog diag;
    diag.firstUseMap_[1] = 1;
    diag.SaveFirstUseRecord();
    diag.firstUseMap_.clear();
    diag.LoadFirstUseRecord();
    EXPECT_EQ(1, static_cast<int32_t>(diag.firstUseMap_.size()));
    EXPECT_EQ(1, static_cast<int32_t>(diag.firstUseMap_[1]));
}

class TestRemoteObject : public IRemoteObject {
public:
    explicit TestRemoteObject(std::u16string descriptor) : IRemoteObject(descriptor)
    {
    };

    ~TestRemoteObject() = default;

    bool IsProxyObject() const override
    {
        return false;
    };

    int32_t GetObjectRefCount() override
    {
        return 0;
    };

    int Dump(int fd, const std::vector<std::u16string>& args) override
    {
        return 0;
    };

    int SendRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) override
    {
        return -1;
    };

    bool AddDeathRecipient(const sptr<DeathRecipient>& recipient) override
    {
        return false;
    };

    bool RemoveDeathRecipient(const sptr<DeathRecipient>& recipient) override
    {
        return false;
    };
};

/*
 * @tc.name: SetFirstUseMap001
 * @tc.desc: Test SetFirstUseMap
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FirstUseDialogTest, SetFirstUseMap001, TestSize.Level1)
{
    FirstUseDialog diag;
    std::shared_ptr<SecCompEntity> entity = std::make_shared<SecCompEntity>(nullptr, 0, 0, 0, 0);

    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create(true);
    ASSERT_NE(nullptr, runner);
    std::shared_ptr<SecEventHandler> handler = std::make_shared<SecEventHandler>(runner);
    diag.secHandler_ = handler;

    // no entity
    EXPECT_EQ(diag.SetFirstUseMap(nullptr), false);

    // type invalid
    EXPECT_EQ(diag.SetFirstUseMap(entity), false);

    // first use location button
    entity->componentInfo_ = std::make_shared<LocationButton>();
    entity->componentInfo_->type_ = LOCATION_COMPONENT;
    entity->tokenId_ = 0;
    EXPECT_EQ(diag.SetFirstUseMap(entity), true);
    EXPECT_EQ(LOCATION_BUTTON_FIRST_USE, static_cast<uint64_t>(diag.firstUseMap_[0]));

    // first use save button
    entity->componentInfo_->type_ = SAVE_COMPONENT;
    EXPECT_EQ(diag.SetFirstUseMap(entity), true);
    EXPECT_EQ(LOCATION_BUTTON_FIRST_USE | SAVE_BUTTON_FIRST_USE, static_cast<uint64_t>(diag.firstUseMap_[0]));

    // second use save button
    EXPECT_EQ(diag.SetFirstUseMap(entity), true);
    EXPECT_EQ(LOCATION_BUTTON_FIRST_USE | SAVE_BUTTON_FIRST_USE, static_cast<uint64_t>(diag.firstUseMap_[0]));

    // wait for event handler done
    sleep(3);
}

/*
 * @tc.name: NotifyFirstUseDialog001
 * @tc.desc: Test NotifyFirstUseDialog
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FirstUseDialogTest, NotifyFirstUseDialog001, TestSize.Level1)
{
    FirstUseDialog diag;
    diag.secHandler_ = nullptr;

    // no entity
    EXPECT_EQ(diag.NotifyFirstUseDialog(nullptr, nullptr, nullptr), SC_SERVICE_ERROR_VALUE_INVALID);

    std::shared_ptr<SecCompEntity> entity = std::make_shared<SecCompEntity>(nullptr, 0, 0, 0, 0);
    // no handler
    EXPECT_EQ(diag.NotifyFirstUseDialog(entity, nullptr, nullptr), SC_SERVICE_ERROR_VALUE_INVALID);

    // no calltoken
    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create(true);
    ASSERT_NE(nullptr, runner);
    std::shared_ptr<SecEventHandler> handler = std::make_shared<SecEventHandler>(runner);
    diag.secHandler_ = handler;
    EXPECT_EQ(diag.NotifyFirstUseDialog(entity, nullptr, nullptr), SC_SERVICE_ERROR_VALUE_INVALID);

    // no dialogCallback
    sptr<TestRemoteObject> testRemoteObject = new TestRemoteObject(std::u16string());
    EXPECT_EQ(diag.NotifyFirstUseDialog(entity, testRemoteObject, nullptr), SC_SERVICE_ERROR_VALUE_INVALID);

    // type invalid
    EXPECT_EQ(diag.NotifyFirstUseDialog(entity, testRemoteObject, testRemoteObject), SC_OK);

    // first use location button
    entity->componentInfo_ = std::make_shared<LocationButton>();
    entity->componentInfo_->type_ = LOCATION_COMPONENT;
    entity->tokenId_ = 0;
    EXPECT_EQ(diag.NotifyFirstUseDialog(entity, testRemoteObject, testRemoteObject),
        SC_SERVICE_ERROR_WAIT_FOR_DIALOG_CLOSE);
    EXPECT_EQ(0, static_cast<uint64_t>(diag.firstUseMap_[0]));

    // first use save button
    entity->componentInfo_->type_ = SAVE_COMPONENT;
    EXPECT_EQ(diag.NotifyFirstUseDialog(entity, testRemoteObject, testRemoteObject),
        SC_SERVICE_ERROR_WAIT_FOR_DIALOG_CLOSE);
    EXPECT_EQ(0, static_cast<uint64_t>(diag.firstUseMap_[0]));

    // second use save button
    EXPECT_EQ(diag.NotifyFirstUseDialog(entity, testRemoteObject, testRemoteObject),
        SC_SERVICE_ERROR_WAIT_FOR_DIALOG_CLOSE);
    EXPECT_EQ(0, static_cast<uint64_t>(diag.firstUseMap_[0]));

    diag.StartDialogAbility(entity, testRemoteObject, testRemoteObject);

    // wait for event handler done
    sleep(3);
}

/*
 * @tc.name: GrantDialogWaitEntity001
 * @tc.desc: Test GrantDialogWaitEntity
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FirstUseDialogTest, GrantDialogWaitEntity001, TestSize.Level1)
{
    FirstUseDialog diag;
    EXPECT_EQ(diag.GrantDialogWaitEntity(0), SC_SERVICE_ERROR_COMPONENT_NOT_EXIST);

    diag.dialogWaitMap_[0] = nullptr;
    EXPECT_EQ(diag.GrantDialogWaitEntity(0), SC_SERVICE_ERROR_COMPONENT_NOT_EXIST);

    std::shared_ptr<SecCompEntity> entity = std::make_shared<SecCompEntity>(nullptr, 1, 0, 0, 0);
    diag.dialogWaitMap_[0] = entity;
    EXPECT_EQ(diag.GrantDialogWaitEntity(0), SC_SERVICE_ERROR_PERMISSION_OPER_FAIL);

    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create(true);
    ASSERT_NE(runner, nullptr);

    std::shared_ptr<SecEventHandler> handler = std::make_shared<SecEventHandler>(runner);
    SecCompPermManager::GetInstance().InitEventHandler(handler);
    entity->componentInfo_ = std::make_shared<SaveButton>();
    entity->componentInfo_->type_ = SAVE_COMPONENT;
    diag.dialogWaitMap_[1] = entity;
    EXPECT_EQ(diag.GrantDialogWaitEntity(1), SC_OK);
}

/*
 * @tc.name: RemoveDialogWaitEntitys001
 * @tc.desc: Test RemoveDialogWaitEntitys
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FirstUseDialogTest, RemoveDialogWaitEntitys001, TestSize.Level1)
{
    FirstUseDialog diag;
    std::shared_ptr<SecCompEntity> entity = std::make_shared<SecCompEntity>(nullptr, 0, 0, 0, 0);
    std::shared_ptr<SecCompEntity> entity1 = std::make_shared<SecCompEntity>(nullptr, 0, 0, 1, 0);
    diag.dialogWaitMap_[0] = entity;
    diag.dialogWaitMap_[1] = entity1;
    diag.RemoveDialogWaitEntitys(1);
    EXPECT_EQ(diag.dialogWaitMap_.count(1), 0);
}

/*
 * @tc.name: SecCompDialogSrvCallback001
 * @tc.desc: Test SecCompDialogSrvCallback
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FirstUseDialogTest, SecCompDialogSrvCallback001, TestSize.Level1)
{
    SecCompDialogSrvCallback *call = new (std::nothrow)SecCompDialogSrvCallback(0, nullptr, nullptr);
    sptr<SecCompDialogSrvCallback> srvCallback = call;

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    EXPECT_EQ(srvCallback->OnRemoteRequest(ISecCompDialogCallback::ON_DIALOG_CLOSED, data, reply, option),
        SC_SERVICE_ERROR_IPC_REQUEST_FAIL);

    data.WriteInterfaceToken(ISecCompDialogCallback::GetDescriptor());
    EXPECT_EQ(srvCallback->OnRemoteRequest(ISecCompDialogCallback::ON_DIALOG_CLOSED, data, reply, option),
        SC_SERVICE_ERROR_IPC_REQUEST_FAIL);

    MessageParcel data1;
    data1.WriteInterfaceToken(ISecCompDialogCallback::GetDescriptor());
    data1.WriteInt32(0);
    EXPECT_EQ(srvCallback->OnRemoteRequest(ISecCompDialogCallback::ON_DIALOG_CLOSED, data1, reply, option),
        0);

    MessageParcel data2;
    data2.WriteInterfaceToken(ISecCompDialogCallback::GetDescriptor());
    EXPECT_NE(srvCallback->OnRemoteRequest(-1, data2, reply, option), 0);
}
