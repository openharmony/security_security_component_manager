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
#include "sec_comp_log.h"
#include "sec_comp_err.h"

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
 * @tc.name: NotifyFirstUseDialog001
 * @tc.desc: Test NotifyFirstUseDialog
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FirstUseDialogTest, NotifyFirstUseDialog001, TestSize.Level1)
{
    FirstUseDialog diag;
    diag.secHandler_ = nullptr;
    // no handler
    ASSERT_FALSE(diag.NotifyFirstUseDialog(0, LOCATION_COMPONENT, nullptr));

    // no calltoken
    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create(true);
    ASSERT_NE(nullptr, runner);
    std::shared_ptr<SecEventHandler> handler = std::make_shared<SecEventHandler>(runner);
    diag.secHandler_ = handler;
    ASSERT_FALSE(diag.NotifyFirstUseDialog(0, LOCATION_COMPONENT, nullptr));

    // type invalid
    sptr<TestRemoteObject> testRemoteObject = new TestRemoteObject(std::u16string());
    ASSERT_FALSE(diag.NotifyFirstUseDialog(0, PASTE_COMPONENT, testRemoteObject));

    // first use location button
    ASSERT_TRUE(diag.NotifyFirstUseDialog(0, LOCATION_COMPONENT, testRemoteObject));
    ASSERT_EQ(LOCATION_BUTTON_FIRST_USE, static_cast<uint64_t>(diag.firstUseMap_[0]));

    // first use save button
    ASSERT_TRUE(diag.NotifyFirstUseDialog(0, SAVE_COMPONENT, testRemoteObject));
    ASSERT_EQ(LOCATION_BUTTON_FIRST_USE | SAVE_BUTTON_FIRST_USE, static_cast<uint64_t>(diag.firstUseMap_[0]));

    // second use save button
    ASSERT_TRUE(diag.NotifyFirstUseDialog(0, SAVE_COMPONENT, testRemoteObject));
    ASSERT_EQ(LOCATION_BUTTON_FIRST_USE | SAVE_BUTTON_FIRST_USE, static_cast<uint64_t>(diag.firstUseMap_[0]));

    diag.StartDialogAbility(PASTE_COMPONENT, nullptr);

    // wait for event handler done
    sleep(3);
}
