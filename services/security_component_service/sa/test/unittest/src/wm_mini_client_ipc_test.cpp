/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>

#include "iremote_object.h"
#include "ipc_object_stub.h"
#include "message_option.h"
#include "message_parcel.h"
#include "wm_mini_client.h"
#include "wm_mini_client_test.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Rosen;
using namespace OHOS::Security::SecurityComponent;

namespace {
constexpr char16_t WINDOW_MANAGER_DESCRIPTOR[] = u"OHOS.IWindowManager";
constexpr char16_t MOCK_SESSION_MANAGER_DESCRIPTOR[] = u"OHOS.IMockSessionManager";
constexpr char16_t SESSION_MANAGER_SERVICE_DESCRIPTOR[] = u"OHOS.ISessionManagerService";
constexpr char16_t SCENE_SESSION_MANAGER_DESCRIPTOR[] = u"OHOS.ISceneSessionManager";
constexpr uint32_t TRANS_ID_GET_SESSION_MANAGER_SERVICE = 0;
constexpr uint32_t TRANS_ID_GET_SESSION_MANAGER_SERVICE_BY_USER_ID = 1;
constexpr uint32_t TRANS_ID_GET_SCENE_SESSION_MANAGER = 0;
constexpr uint32_t TRANS_ID_GET_WINDOW_INFO = 12;
constexpr uint32_t TRANS_ID_GET_UNRELIABLE_WINDOW_INFO = 80;
constexpr uint32_t TRANS_ID_GET_LEGACY_WINDOW_INFO = 20;
constexpr uint32_t TRANS_ID_GET_LEGACY_UNRELIABLE_WINDOW_INFO = 53;
constexpr int32_t TEST_ROOT_USER_ID = 0;
constexpr int32_t TEST_USER_ID = 100;
constexpr int32_t TEST_MISSING_USER_ID = 101;
constexpr int32_t TEST_WINDOW_ID = 200;
constexpr int32_t IPC_OK = 0;
constexpr int32_t IPC_FAILED = -1;
constexpr int32_t WRITE_FAIL_NONE = 0;
constexpr int32_t WRITE_FAIL_MOCK_INTERFACE_TOKEN = 1;
constexpr int32_t WRITE_FAIL_MOCK_USER_ID = 2;
constexpr int32_t WRITE_FAIL_SESSION_INTERFACE_TOKEN = 3;
constexpr int32_t WRITE_FAIL_WINDOW_INFO_INTERFACE_TOKEN = 4;
constexpr int32_t WRITE_FAIL_UNRELIABLE_INTERFACE_TOKEN = 5;
constexpr int32_t WRITE_FAIL_UNRELIABLE_WINDOW_ID = 6;
constexpr uint32_t MAX_TOUCH_HOT_AREAS_LOCAL = 10000;
constexpr int32_t PARCELABLE_PRESENT = 1;
constexpr int32_t TEST_WINDOW_INFO_ID = 10;
constexpr int32_t TEST_INNER_WINDOW_ID = 20;
constexpr int32_t TEST_UI_NODE_ID = 30;
constexpr uint32_t TEST_WINDOW_INFO_RECT_WIDTH = 40;
constexpr uint32_t TEST_WINDOW_INFO_RECT_HEIGHT = 50;
constexpr int32_t TEST_WINDOW_INFO_RECT_POS_X = 60;
constexpr int32_t TEST_WINDOW_INFO_RECT_POS_Y = 70;
constexpr uint64_t TEST_DISPLAY_ID = 80;
constexpr uint32_t TEST_WINDOW_LAYER = 90;
constexpr uint32_t TEST_SCALE_RECT_WIDTH = 100;
constexpr uint32_t TEST_SCALE_RECT_HEIGHT = 110;
constexpr int32_t TEST_SCALE_RECT_POS_X = 120;
constexpr int32_t TEST_SCALE_RECT_POS_Y = 130;
constexpr uint32_t TEST_WINDOW_MODE = 140;
constexpr uint32_t TEST_WINDOW_TYPE = 150;
constexpr int32_t TEST_TOUCH_HOT_AREA_POS_X = 160;
constexpr int32_t TEST_TOUCH_HOT_AREA_POS_Y = 170;
constexpr uint32_t TEST_TOUCH_HOT_AREA_WIDTH = 180;
constexpr uint32_t TEST_TOUCH_HOT_AREA_HEIGHT = 190;
constexpr uint32_t TEST_UNRELIABLE_WINDOW_RECT_WIDTH = 10;
constexpr uint32_t TEST_UNRELIABLE_WINDOW_RECT_HEIGHT = 20;
constexpr int32_t TEST_UNRELIABLE_WINDOW_RECT_POS_X = 30;
constexpr int32_t TEST_UNRELIABLE_WINDOW_RECT_POS_Y = 40;
constexpr uint32_t TEST_UNRELIABLE_WINDOW_Z_ORDER = 50;

class WmMiniRemoteObject : public IPCObjectStub {
public:
    explicit WmMiniRemoteObject(const std::u16string& descriptor) : IPCObjectStub(descriptor) {}
    ~WmMiniRemoteObject() override = default;
};

enum class SceneReplyType {
    WINDOW_INFO,
    UNRELIABLE_WINDOW,
    BROKEN_VECTOR,
    VECTOR_NO_READABLE_BYTES,
    VECTOR_ONE_BYTE_ENTRY,
    PARTIAL_PARCELABLE,
    NO_REPLY_ERR_CODE,
    ERROR_CODE,
};

void WriteWindowInfoFixedFields(MessageParcel& reply)
{
    ASSERT_TRUE(reply.WriteInt32(TEST_WINDOW_INFO_ID));
    ASSERT_TRUE(reply.WriteInt32(TEST_INNER_WINDOW_ID));
    ASSERT_TRUE(reply.WriteInt32(TEST_UI_NODE_ID));
    ASSERT_TRUE(reply.WriteUint32(TEST_WINDOW_INFO_RECT_WIDTH));
    ASSERT_TRUE(reply.WriteUint32(TEST_WINDOW_INFO_RECT_HEIGHT));
    ASSERT_TRUE(reply.WriteInt32(TEST_WINDOW_INFO_RECT_POS_X));
    ASSERT_TRUE(reply.WriteInt32(TEST_WINDOW_INFO_RECT_POS_Y));
    ASSERT_TRUE(reply.WriteBool(true));
    ASSERT_TRUE(reply.WriteBool(false));
    ASSERT_TRUE(reply.WriteUint64(TEST_DISPLAY_ID));
    ASSERT_TRUE(reply.WriteUint32(TEST_WINDOW_LAYER));
    ASSERT_TRUE(reply.WriteFloat(1.0F));
    ASSERT_TRUE(reply.WriteFloat(0.5F));
    ASSERT_TRUE(reply.WriteFloat(0.75F));
    ASSERT_TRUE(reply.WriteBool(true));
    ASSERT_TRUE(reply.WriteUint32(TEST_SCALE_RECT_WIDTH));
    ASSERT_TRUE(reply.WriteUint32(TEST_SCALE_RECT_HEIGHT));
    ASSERT_TRUE(reply.WriteInt32(TEST_SCALE_RECT_POS_X));
    ASSERT_TRUE(reply.WriteInt32(TEST_SCALE_RECT_POS_Y));
}

void WriteWindowInfoTailFields(MessageParcel& reply)
{
    ASSERT_TRUE(reply.WriteUint32(TEST_WINDOW_MODE));
    ASSERT_TRUE(reply.WriteUint32(TEST_WINDOW_TYPE));
    ASSERT_TRUE(reply.WriteString("bundle"));
}

void WriteWindowInfoPayload(MessageParcel& reply, uint32_t touchHotAreasCnt = 1,
    bool writeTouchHotAreas = true)
{
    WriteWindowInfoFixedFields(reply);
    WriteWindowInfoTailFields(reply);
    ASSERT_TRUE(reply.WriteUint32(touchHotAreasCnt));
    if (!writeTouchHotAreas) {
        return;
    }
    for (uint32_t i = 0; i < touchHotAreasCnt; ++i) {
        ASSERT_TRUE(reply.WriteInt32(TEST_TOUCH_HOT_AREA_POS_X));
        ASSERT_TRUE(reply.WriteInt32(TEST_TOUCH_HOT_AREA_POS_Y));
        ASSERT_TRUE(reply.WriteUint32(TEST_TOUCH_HOT_AREA_WIDTH));
        ASSERT_TRUE(reply.WriteUint32(TEST_TOUCH_HOT_AREA_HEIGHT));
    }
}

void WriteWindowInfoReply(MessageParcel& reply, bool writeErrCode = true,
    MiniWMError errCode = MiniWMError::WM_OK, uint32_t touchHotAreasCnt = 1)
{
    ASSERT_TRUE(reply.WriteInt32(1));
    ASSERT_TRUE(reply.WriteInt32(PARCELABLE_PRESENT));
    WriteWindowInfoPayload(reply, touchHotAreasCnt);
    if (writeErrCode) {
        ASSERT_TRUE(reply.WriteInt32(static_cast<int32_t>(errCode)));
    }
}

void WriteUnreliableWindowPayload(MessageParcel& reply)
{
    ASSERT_TRUE(reply.WriteInt32(TEST_WINDOW_ID));
    ASSERT_TRUE(reply.WriteUint32(TEST_UNRELIABLE_WINDOW_RECT_WIDTH));
    ASSERT_TRUE(reply.WriteUint32(TEST_UNRELIABLE_WINDOW_RECT_HEIGHT));
    ASSERT_TRUE(reply.WriteInt32(TEST_UNRELIABLE_WINDOW_RECT_POS_X));
    ASSERT_TRUE(reply.WriteInt32(TEST_UNRELIABLE_WINDOW_RECT_POS_Y));
    ASSERT_TRUE(reply.WriteUint32(TEST_UNRELIABLE_WINDOW_Z_ORDER));
    ASSERT_TRUE(reply.WriteFloat(1.0F));
    ASSERT_TRUE(reply.WriteFloat(0.5F));
    ASSERT_TRUE(reply.WriteFloat(0.75F));
}

void WriteUnreliableWindowReply(MessageParcel& reply, bool writeErrCode = true,
    MiniWMError errCode = MiniWMError::WM_OK)
{
    ASSERT_TRUE(reply.WriteInt32(1));
    ASSERT_TRUE(reply.WriteInt32(PARCELABLE_PRESENT));
    WriteUnreliableWindowPayload(reply);
    if (writeErrCode) {
        ASSERT_TRUE(reply.WriteInt32(static_cast<int32_t>(errCode)));
    }
}

class FakeSceneSessionManagerRemote final : public WmMiniRemoteObject {
public:
    FakeSceneSessionManagerRemote() : WmMiniRemoteObject(SCENE_SESSION_MANAGER_DESCRIPTOR) {}
    ~FakeSceneSessionManagerRemote() override = default;

    int SendRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) override
    {
        (void)option;
        lastCode_ = code;
        lastToken_ = data.ReadInterfaceToken();
        if (code == TRANS_ID_GET_UNRELIABLE_WINDOW_INFO) {
            (void)data.ReadInt32(lastWindowId_);
        }
        if (sendRequestRet_ != IPC_OK) {
            return sendRequestRet_;
        }
        if (replyType_ == SceneReplyType::BROKEN_VECTOR) {
            if (!reply.WriteInt32(-1)) {
                return IPC_FAILED;
            }
            return IPC_OK;
        }
        if (replyType_ == SceneReplyType::VECTOR_NO_READABLE_BYTES) {
            if (!reply.WriteInt32(1)) {
                return IPC_FAILED;
            }
            return IPC_OK;
        }
        if (replyType_ == SceneReplyType::VECTOR_ONE_BYTE_ENTRY) {
            if (!reply.WriteInt32(1) || !reply.WriteInt8Unaligned(1)) {
                return IPC_FAILED;
            }
            return IPC_OK;
        }
        if (replyType_ == SceneReplyType::PARTIAL_PARCELABLE) {
            if (!reply.WriteInt32(1) || !reply.WriteInt32(PARCELABLE_PRESENT)) {
                return IPC_FAILED;
            }
            return IPC_OK;
        }
        const bool writeErrCode = replyType_ != SceneReplyType::NO_REPLY_ERR_CODE;
        MiniWMError errCode = MiniWMError::WM_OK;
        if (replyType_ == SceneReplyType::ERROR_CODE) {
            errCode = replyErrCode_;
        }
        if (code == TRANS_ID_GET_UNRELIABLE_WINDOW_INFO) {
            WriteUnreliableWindowReply(reply, writeErrCode, errCode);
        } else if (code == TRANS_ID_GET_WINDOW_INFO) {
            WriteWindowInfoReply(reply, writeErrCode, errCode, touchHotAreasCnt_);
        } else {
            return IPC_FAILED;
        }
        return IPC_OK;
    }

    SceneReplyType replyType_ = SceneReplyType::WINDOW_INFO;
    MiniWMError replyErrCode_ = MiniWMError::WM_OK;
    uint32_t touchHotAreasCnt_ = 1;
    int32_t sendRequestRet_ = IPC_OK;
    uint32_t lastCode_ = 0;
    std::u16string lastToken_;
    int32_t lastWindowId_ = -1;
};

class FakeLegacyWmsRemote final : public WmMiniRemoteObject {
public:
    FakeLegacyWmsRemote() : WmMiniRemoteObject(WINDOW_MANAGER_DESCRIPTOR) {}
    ~FakeLegacyWmsRemote() override = default;

    int SendRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) override
    {
        (void)option;
        lastCode_ = code;
        lastToken_ = data.ReadInterfaceToken();
        if (code == TRANS_ID_GET_LEGACY_UNRELIABLE_WINDOW_INFO) {
            hasWindowId_ = data.ReadInt32(lastWindowId_);
        }
        if (sendRequestRet_ != IPC_OK) {
            return sendRequestRet_;
        }
        if (code == TRANS_ID_GET_LEGACY_WINDOW_INFO) {
            WriteWindowInfoReply(reply);
            return IPC_OK;
        }
        if (code == TRANS_ID_GET_LEGACY_UNRELIABLE_WINDOW_INFO) {
            WriteUnreliableWindowReply(reply);
            return IPC_OK;
        }
        return IPC_FAILED;
    }

    int32_t sendRequestRet_ = IPC_OK;
    uint32_t lastCode_ = 0;
    std::u16string lastToken_;
    bool hasWindowId_ = false;
    int32_t lastWindowId_ = -1;
};

class FakeUnknownWmsRemote final : public WmMiniRemoteObject {
public:
    FakeUnknownWmsRemote() : WmMiniRemoteObject(u"OHOS.IUnknownWindowManager") {}
    ~FakeUnknownWmsRemote() override = default;

    int SendRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) override
    {
        (void)code;
        (void)data;
        (void)reply;
        (void)option;
        requestCalled_ = true;
        return IPC_FAILED;
    }

    bool requestCalled_ = false;
};

class FakeSessionManagerServiceRemote final : public WmMiniRemoteObject {
public:
    explicit FakeSessionManagerServiceRemote(const sptr<FakeSceneSessionManagerRemote>& sceneRemote)
        : WmMiniRemoteObject(SESSION_MANAGER_SERVICE_DESCRIPTOR), sceneRemote_(sceneRemote)
    {}
    ~FakeSessionManagerServiceRemote() override = default;

    int SendRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) override
    {
        (void)option;
        lastCode_ = code;
        lastToken_ = data.ReadInterfaceToken();
        if (sendRequestRet_ != IPC_OK) {
            return sendRequestRet_;
        }
        (void)reply.WriteRemoteObject(sceneRemote_);
        return IPC_OK;
    }

    sptr<FakeSceneSessionManagerRemote> sceneRemote_;
    int32_t sendRequestRet_ = IPC_OK;
    uint32_t lastCode_ = 0;
    std::u16string lastToken_;
};

class FakeSceneBoardWmsRemote final : public WmMiniRemoteObject {
public:
    explicit FakeSceneBoardWmsRemote(const sptr<FakeSessionManagerServiceRemote>& sessionManagerRemote)
        : WmMiniRemoteObject(MOCK_SESSION_MANAGER_DESCRIPTOR), sessionManagerRemote_(sessionManagerRemote)
    {}
    ~FakeSceneBoardWmsRemote() override = default;

    int SendRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) override
    {
        (void)option;
        ++requestCount_;
        lastCode_ = code;
        lastToken_ = data.ReadInterfaceToken();
        hasUserId_ = data.ReadInt32(lastUserId_);
        if (sendRequestRet_ != IPC_OK) {
            return sendRequestRet_;
        }
        if (!writeErrCode_) {
            return IPC_OK;
        }
        (void)reply.WriteInt32(mockSessionManagerErrCode_);
        if (writeRemote_) {
            (void)reply.WriteRemoteObject(sessionManagerRemote_);
        }
        return IPC_OK;
    }

    sptr<FakeSessionManagerServiceRemote> sessionManagerRemote_;
    int32_t sendRequestRet_ = IPC_OK;
    int32_t mockSessionManagerErrCode_ = IPC_OK;
    bool writeErrCode_ = true;
    bool writeRemote_ = true;
    uint32_t requestCount_ = 0;
    uint32_t lastCode_ = 0;
    std::u16string lastToken_;
    bool hasUserId_ = false;
    int32_t lastUserId_ = -1;
};

class WMClientMiniIpcTest : public testing::Test {
public:
    void SetUp() override
    {
        ResetTestControls();
        sceneRemote_ = sptr<FakeSceneSessionManagerRemote>::MakeSptr();
        sessionManagerRemote_ = sptr<FakeSessionManagerServiceRemote>::MakeSptr(sceneRemote_);
        wmsRemote_ = sptr<FakeSceneBoardWmsRemote>::MakeSptr(sessionManagerRemote_);
        legacyWmsRemote_ = sptr<FakeLegacyWmsRemote>::MakeSptr();
        WmMiniClientTest::SetWmsRemote(wmsRemote_);
    }

    void TearDown() override
    {
        WmMiniClientTest::SetWmsRemote(nullptr);
        ResetTestControls();
    }

    void ResetTestControls() const
    {
        WmMiniClientTest::SetMultiInstanceEnabled(false);
        WmMiniClientTest::SetWriteFailurePoint(WRITE_FAIL_NONE);
        WmMiniClientTest::SetMiniInfoAllocFail(false);
        WmMiniClientTest::SetParcelableVectorSizeOverLimit(false);
        WmMiniClientTest::SetParcelableVectorResizeFail(false);
    }

    sptr<FakeSceneSessionManagerRemote> sceneRemote_;
    sptr<FakeSessionManagerServiceRemote> sessionManagerRemote_;
    sptr<FakeSceneBoardWmsRemote> wmsRemote_;
    sptr<FakeLegacyWmsRemote> legacyWmsRemote_;
};
}

/**
 * @tc.name: GetWindowInfoIpc001
 * @tc.desc: Test window info IPC path with user id
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WMClientMiniIpcTest, GetWindowInfoIpc001, TestSize.Level0)
{
    WmMiniClientTest::SetMultiInstanceEnabled(true);
    sceneRemote_->replyType_ = SceneReplyType::WINDOW_INFO;

    std::vector<sptr<MiniAccessibilityWindowInfo>> infos;
    MiniWMError ret = WMClientMini::GetAccessibilityWindowInfo(TEST_USER_ID, infos);

    EXPECT_EQ(MiniWMError::WM_OK, ret);
    EXPECT_EQ(TRANS_ID_GET_SESSION_MANAGER_SERVICE_BY_USER_ID, wmsRemote_->lastCode_);
    EXPECT_EQ(std::u16string(MOCK_SESSION_MANAGER_DESCRIPTOR), wmsRemote_->lastToken_);
    EXPECT_TRUE(wmsRemote_->hasUserId_);
    EXPECT_EQ(TEST_USER_ID, wmsRemote_->lastUserId_);
    EXPECT_EQ(1U, wmsRemote_->requestCount_);
    EXPECT_EQ(TRANS_ID_GET_SCENE_SESSION_MANAGER, sessionManagerRemote_->lastCode_);
    EXPECT_EQ(std::u16string(SESSION_MANAGER_SERVICE_DESCRIPTOR), sessionManagerRemote_->lastToken_);
    EXPECT_EQ(TRANS_ID_GET_WINDOW_INFO, sceneRemote_->lastCode_);
    EXPECT_EQ(std::u16string(SCENE_SESSION_MANAGER_DESCRIPTOR), sceneRemote_->lastToken_);
    ASSERT_EQ(1U, infos.size());
    ASSERT_NE(nullptr, infos[0]);
    EXPECT_EQ(TEST_WINDOW_INFO_ID, infos[0]->wid_);
    EXPECT_EQ(TEST_INNER_WINDOW_ID, infos[0]->innerWid_);
    EXPECT_EQ(1U, infos[0]->touchHotAreas_.size());
    EXPECT_FLOAT_EQ(0.5F, infos[0]->scaleX_);
    EXPECT_FLOAT_EQ(0.75F, infos[0]->scaleY_);
}

/**
 * @tc.name: GetWindowInfoSingleInstanceRootIpc001
 * @tc.desc: Test single-instance mode uses the default service for root user
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WMClientMiniIpcTest, GetWindowInfoSingleInstanceRootIpc001, TestSize.Level0)
{
    WmMiniClientTest::SetMultiInstanceEnabled(false);

    std::vector<sptr<MiniAccessibilityWindowInfo>> infos;
    MiniWMError ret = WMClientMini::GetAccessibilityWindowInfo(TEST_ROOT_USER_ID, infos);

    EXPECT_EQ(MiniWMError::WM_OK, ret);
    EXPECT_EQ(TRANS_ID_GET_SESSION_MANAGER_SERVICE, wmsRemote_->lastCode_);
    EXPECT_FALSE(wmsRemote_->hasUserId_);
    EXPECT_EQ(1U, wmsRemote_->requestCount_);
}

/**
 * @tc.name: GetWindowInfoSingleInstanceUserIpc001
 * @tc.desc: Test single-instance mode uses the default service for a regular user
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WMClientMiniIpcTest, GetWindowInfoSingleInstanceUserIpc001, TestSize.Level0)
{
    WmMiniClientTest::SetMultiInstanceEnabled(false);

    std::vector<sptr<MiniAccessibilityWindowInfo>> infos;
    MiniWMError ret = WMClientMini::GetAccessibilityWindowInfo(TEST_USER_ID, infos);

    EXPECT_EQ(MiniWMError::WM_OK, ret);
    EXPECT_EQ(TRANS_ID_GET_SESSION_MANAGER_SERVICE, wmsRemote_->lastCode_);
    EXPECT_FALSE(wmsRemote_->hasUserId_);
    EXPECT_EQ(1U, wmsRemote_->requestCount_);
}

/**
 * @tc.name: GetWindowInfoMultiInstanceRootIpc001
 * @tc.desc: Test multi-instance mode treats root user as an exact target
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WMClientMiniIpcTest, GetWindowInfoMultiInstanceRootIpc001, TestSize.Level0)
{
    WmMiniClientTest::SetMultiInstanceEnabled(true);

    std::vector<sptr<MiniAccessibilityWindowInfo>> infos;
    MiniWMError ret = WMClientMini::GetAccessibilityWindowInfo(TEST_ROOT_USER_ID, infos);

    EXPECT_EQ(MiniWMError::WM_OK, ret);
    EXPECT_EQ(TRANS_ID_GET_SESSION_MANAGER_SERVICE_BY_USER_ID, wmsRemote_->lastCode_);
    EXPECT_TRUE(wmsRemote_->hasUserId_);
    EXPECT_EQ(TEST_ROOT_USER_ID, wmsRemote_->lastUserId_);
    EXPECT_EQ(1U, wmsRemote_->requestCount_);
}

/**
 * @tc.name: GetWindowInfoMultiInstanceMissingUserIpc001
 * @tc.desc: Test multi-instance mode does not fall back when the target user is missing
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WMClientMiniIpcTest, GetWindowInfoMultiInstanceMissingUserIpc001, TestSize.Level0)
{
    WmMiniClientTest::SetMultiInstanceEnabled(true);
    wmsRemote_->mockSessionManagerErrCode_ = IPC_FAILED;

    std::vector<sptr<MiniAccessibilityWindowInfo>> infos;
    MiniWMError ret = WMClientMini::GetAccessibilityWindowInfo(TEST_MISSING_USER_ID, infos);

    EXPECT_EQ(MiniWMError::WM_ERROR_SAMGR, ret);
    EXPECT_EQ(TRANS_ID_GET_SESSION_MANAGER_SERVICE_BY_USER_ID, wmsRemote_->lastCode_);
    EXPECT_TRUE(wmsRemote_->hasUserId_);
    EXPECT_EQ(TEST_MISSING_USER_ID, wmsRemote_->lastUserId_);
    EXPECT_EQ(1U, wmsRemote_->requestCount_);
    EXPECT_TRUE(infos.empty());
}

/**
 * @tc.name: GetUnreliableWindowInfoIpc001
 * @tc.desc: Test unreliable window info IPC path with default user id
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WMClientMiniIpcTest, GetUnreliableWindowInfoIpc001, TestSize.Level0)
{
    sceneRemote_->replyType_ = SceneReplyType::UNRELIABLE_WINDOW;

    std::vector<sptr<MiniUnreliableWindowInfo>> infos;
    MiniWMError ret = WMClientMini::GetUnreliableWindowInfo(TEST_WINDOW_ID, -1, infos);

    EXPECT_EQ(MiniWMError::WM_OK, ret);
    EXPECT_EQ(TRANS_ID_GET_SESSION_MANAGER_SERVICE, wmsRemote_->lastCode_);
    EXPECT_EQ(std::u16string(MOCK_SESSION_MANAGER_DESCRIPTOR), wmsRemote_->lastToken_);
    EXPECT_FALSE(wmsRemote_->hasUserId_);
    EXPECT_EQ(TRANS_ID_GET_SCENE_SESSION_MANAGER, sessionManagerRemote_->lastCode_);
    EXPECT_EQ(std::u16string(SESSION_MANAGER_SERVICE_DESCRIPTOR), sessionManagerRemote_->lastToken_);
    EXPECT_EQ(TRANS_ID_GET_UNRELIABLE_WINDOW_INFO, sceneRemote_->lastCode_);
    EXPECT_EQ(std::u16string(SCENE_SESSION_MANAGER_DESCRIPTOR), sceneRemote_->lastToken_);
    EXPECT_EQ(TEST_WINDOW_ID, sceneRemote_->lastWindowId_);
    ASSERT_EQ(1U, infos.size());
    ASSERT_NE(nullptr, infos[0]);
    EXPECT_EQ(TEST_WINDOW_ID, infos[0]->windowId_);
    EXPECT_EQ(TEST_UNRELIABLE_WINDOW_Z_ORDER, infos[0]->zOrder_);
    EXPECT_FLOAT_EQ(0.5F, infos[0]->scaleX_);
    EXPECT_FLOAT_EQ(0.75F, infos[0]->scaleY_);
}

/**
 * @tc.name: GetWindowInfoLegacyIpc001
 * @tc.desc: Test window info IPC path with legacy WMS
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WMClientMiniIpcTest, GetWindowInfoLegacyIpc001, TestSize.Level0)
{
    WmMiniClientTest::SetWmsRemote(legacyWmsRemote_);

    std::vector<sptr<MiniAccessibilityWindowInfo>> infos;
    MiniWMError ret = WMClientMini::GetAccessibilityWindowInfo(TEST_USER_ID, infos);

    EXPECT_EQ(MiniWMError::WM_OK, ret);
    EXPECT_EQ(TRANS_ID_GET_LEGACY_WINDOW_INFO, legacyWmsRemote_->lastCode_);
    EXPECT_EQ(std::u16string(WINDOW_MANAGER_DESCRIPTOR), legacyWmsRemote_->lastToken_);
    EXPECT_TRUE(sessionManagerRemote_->lastToken_.empty());
    ASSERT_EQ(1U, infos.size());
    ASSERT_NE(nullptr, infos[0]);
    EXPECT_EQ(TEST_WINDOW_INFO_ID, infos[0]->wid_);
    EXPECT_EQ(TEST_INNER_WINDOW_ID, infos[0]->innerWid_);
    EXPECT_EQ(1U, infos[0]->touchHotAreas_.size());
}

/**
 * @tc.name: GetUnreliableWindowInfoLegacyIpc001
 * @tc.desc: Test unreliable window info IPC path with legacy WMS
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WMClientMiniIpcTest, GetUnreliableWindowInfoLegacyIpc001, TestSize.Level0)
{
    WmMiniClientTest::SetWmsRemote(legacyWmsRemote_);

    std::vector<sptr<MiniUnreliableWindowInfo>> infos;
    MiniWMError ret = WMClientMini::GetUnreliableWindowInfo(TEST_WINDOW_ID, TEST_USER_ID, infos);

    EXPECT_EQ(MiniWMError::WM_OK, ret);
    EXPECT_EQ(TRANS_ID_GET_LEGACY_UNRELIABLE_WINDOW_INFO, legacyWmsRemote_->lastCode_);
    EXPECT_EQ(std::u16string(WINDOW_MANAGER_DESCRIPTOR), legacyWmsRemote_->lastToken_);
    EXPECT_TRUE(legacyWmsRemote_->hasWindowId_);
    EXPECT_EQ(TEST_WINDOW_ID, legacyWmsRemote_->lastWindowId_);
    EXPECT_TRUE(sessionManagerRemote_->lastToken_.empty());
    ASSERT_EQ(1U, infos.size());
    ASSERT_NE(nullptr, infos[0]);
    EXPECT_EQ(TEST_WINDOW_ID, infos[0]->windowId_);
}

/**
 * @tc.name: UnsupportedWindowManagerBackendIpc001
 * @tc.desc: Test unsupported window manager backend is rejected before sending request
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WMClientMiniIpcTest, UnsupportedWindowManagerBackendIpc001, TestSize.Level0)
{
    sptr<FakeUnknownWmsRemote> unknownRemote = sptr<FakeUnknownWmsRemote>::MakeSptr();
    WmMiniClientTest::SetWmsRemote(unknownRemote);

    std::vector<sptr<MiniAccessibilityWindowInfo>> infos;
    MiniWMError ret = WMClientMini::GetAccessibilityWindowInfo(TEST_USER_ID, infos);

    EXPECT_EQ(MiniWMError::WM_ERROR_IPC_FAILED, ret);
    EXPECT_FALSE(unknownRemote->requestCalled_);
    EXPECT_TRUE(infos.empty());
}

/**
 * @tc.name: LegacyWindowManagerRequestFailureIpc001
 * @tc.desc: Test legacy WMS request failure is returned as IPC failure
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WMClientMiniIpcTest, LegacyWindowManagerRequestFailureIpc001, TestSize.Level0)
{
    legacyWmsRemote_->sendRequestRet_ = IPC_FAILED;
    WmMiniClientTest::SetWmsRemote(legacyWmsRemote_);

    std::vector<sptr<MiniAccessibilityWindowInfo>> infos;
    MiniWMError ret = WMClientMini::GetAccessibilityWindowInfo(TEST_USER_ID, infos);

    EXPECT_EQ(MiniWMError::WM_ERROR_IPC_FAILED, ret);
    EXPECT_EQ(TRANS_ID_GET_LEGACY_WINDOW_INFO, legacyWmsRemote_->lastCode_);
    EXPECT_TRUE(infos.empty());
}

/**
 * @tc.name: GetWindowInfoIpc002
 * @tc.desc: Test IPC path returns SAMGR error when WMS remote is null
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WMClientMiniIpcTest, GetWindowInfoIpc002, TestSize.Level0)
{
    WmMiniClientTest::SetWmsRemote(nullptr);

    std::vector<sptr<MiniAccessibilityWindowInfo>> infos;
    MiniWMError ret = WMClientMini::GetAccessibilityWindowInfo(TEST_USER_ID, infos);

    EXPECT_EQ(MiniWMError::WM_ERROR_SAMGR, ret);
    EXPECT_TRUE(infos.empty());
}

/**
 * @tc.name: GetUnreliableWindowInfoIpc002
 * @tc.desc: Test IPC path returns IPC failed when scene reply is invalid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WMClientMiniIpcTest, GetUnreliableWindowInfoIpc002, TestSize.Level0)
{
    sceneRemote_->replyType_ = SceneReplyType::BROKEN_VECTOR;

    std::vector<sptr<MiniUnreliableWindowInfo>> infos;
    MiniWMError ret = WMClientMini::GetUnreliableWindowInfo(TEST_WINDOW_ID, TEST_USER_ID, infos);

    EXPECT_EQ(MiniWMError::WM_ERROR_IPC_FAILED, ret);
}

/**
 * @tc.name: GetWindowInfoIpc003
 * @tc.desc: Test IPC path returns SAMGR error when WMS request fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WMClientMiniIpcTest, GetWindowInfoIpc003, TestSize.Level0)
{
    wmsRemote_->sendRequestRet_ = IPC_FAILED;

    std::vector<sptr<MiniAccessibilityWindowInfo>> infos;
    MiniWMError ret = WMClientMini::GetAccessibilityWindowInfo(TEST_USER_ID, infos);

    EXPECT_EQ(MiniWMError::WM_ERROR_SAMGR, ret);
}

/**
 * @tc.name: MiniInfoMarshalling001
 * @tc.desc: Test mini window info marshalling is intentionally unsupported
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WMClientMiniIpcTest, MiniInfoMarshalling001, TestSize.Level0)
{
    MessageParcel parcel;
    MiniAccessibilityWindowInfo windowInfo;
    MiniUnreliableWindowInfo unreliableInfo;

    EXPECT_FALSE(windowInfo.Marshalling(parcel));
    EXPECT_FALSE(unreliableInfo.Marshalling(parcel));
}

/**
 * @tc.name: MiniWindowInfoUnmarshalling001
 * @tc.desc: Test window info unmarshalling success and failure branches
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WMClientMiniIpcTest, MiniWindowInfoUnmarshalling001, TestSize.Level0)
{
    MessageParcel successParcel;
    WriteWindowInfoPayload(successParcel, 0);
    sptr<MiniAccessibilityWindowInfo> info = MiniAccessibilityWindowInfo::Unmarshalling(successParcel);
    ASSERT_NE(nullptr, info);
    EXPECT_TRUE(info->touchHotAreas_.empty());
    EXPECT_EQ(TEST_WINDOW_INFO_ID, info->wid_);

    MessageParcel emptyParcel;
    EXPECT_EQ(nullptr, MiniAccessibilityWindowInfo::Unmarshalling(emptyParcel));

    MessageParcel missingTailParcel;
    WriteWindowInfoFixedFields(missingTailParcel);
    EXPECT_EQ(nullptr, MiniAccessibilityWindowInfo::Unmarshalling(missingTailParcel));

    MessageParcel missingCountParcel;
    WriteWindowInfoFixedFields(missingCountParcel);
    WriteWindowInfoTailFields(missingCountParcel);
    EXPECT_EQ(nullptr, MiniAccessibilityWindowInfo::Unmarshalling(missingCountParcel));

    MessageParcel tooManyAreasParcel;
    WriteWindowInfoFixedFields(tooManyAreasParcel);
    WriteWindowInfoTailFields(tooManyAreasParcel);
    ASSERT_TRUE(tooManyAreasParcel.WriteUint32(MAX_TOUCH_HOT_AREAS_LOCAL + 1));
    EXPECT_EQ(nullptr, MiniAccessibilityWindowInfo::Unmarshalling(tooManyAreasParcel));

    MessageParcel partialAreaParcel;
    WriteWindowInfoFixedFields(partialAreaParcel);
    WriteWindowInfoTailFields(partialAreaParcel);
    ASSERT_TRUE(partialAreaParcel.WriteUint32(1));
    ASSERT_TRUE(partialAreaParcel.WriteInt32(TEST_TOUCH_HOT_AREA_POS_X));
    EXPECT_EQ(nullptr, MiniAccessibilityWindowInfo::Unmarshalling(partialAreaParcel));
}

/**
 * @tc.name: MiniUnreliableWindowInfoUnmarshalling001
 * @tc.desc: Test unreliable window info unmarshalling success, failure, and allocation failure branches
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WMClientMiniIpcTest, MiniUnreliableWindowInfoUnmarshalling001, TestSize.Level0)
{
    MessageParcel successParcel;
    WriteUnreliableWindowPayload(successParcel);
    sptr<MiniUnreliableWindowInfo> info = MiniUnreliableWindowInfo::Unmarshalling(successParcel);
    ASSERT_NE(nullptr, info);
    EXPECT_EQ(TEST_WINDOW_ID, info->windowId_);

    MessageParcel emptyParcel;
    EXPECT_EQ(nullptr, MiniUnreliableWindowInfo::Unmarshalling(emptyParcel));

    MessageParcel partialParcel;
    ASSERT_TRUE(partialParcel.WriteInt32(TEST_WINDOW_ID));
    EXPECT_EQ(nullptr, MiniUnreliableWindowInfo::Unmarshalling(partialParcel));

    WmMiniClientTest::SetMiniInfoAllocFail(true);
    MessageParcel windowInfoParcel;
    MessageParcel unreliableParcel;
    EXPECT_EQ(nullptr, MiniAccessibilityWindowInfo::Unmarshalling(windowInfoParcel));
    EXPECT_EQ(nullptr, MiniUnreliableWindowInfo::Unmarshalling(unreliableParcel));
    WmMiniClientTest::SetMiniInfoAllocFail(false);
}

/**
 * @tc.name: GetSessionManagerRemoteIpc001
 * @tc.desc: Test session manager remote reply failure branches
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WMClientMiniIpcTest, GetSessionManagerRemoteIpc001, TestSize.Level0)
{
    std::vector<sptr<MiniAccessibilityWindowInfo>> infos;

    wmsRemote_->writeErrCode_ = false;
    EXPECT_EQ(MiniWMError::WM_ERROR_SAMGR, WMClientMini::GetAccessibilityWindowInfo(TEST_USER_ID, infos));

    wmsRemote_->writeErrCode_ = true;
    wmsRemote_->mockSessionManagerErrCode_ = IPC_FAILED;
    EXPECT_EQ(MiniWMError::WM_ERROR_SAMGR, WMClientMini::GetAccessibilityWindowInfo(TEST_USER_ID, infos));

    wmsRemote_->mockSessionManagerErrCode_ = IPC_OK;
    wmsRemote_->writeRemote_ = false;
    EXPECT_EQ(MiniWMError::WM_ERROR_SAMGR, WMClientMini::GetAccessibilityWindowInfo(TEST_USER_ID, infos));
}

/**
 * @tc.name: GetSceneSessionManagerRemoteIpc001
 * @tc.desc: Test scene session manager remote failure branches
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WMClientMiniIpcTest, GetSceneSessionManagerRemoteIpc001, TestSize.Level0)
{
    std::vector<sptr<MiniAccessibilityWindowInfo>> infos;

    sessionManagerRemote_->sendRequestRet_ = IPC_FAILED;
    EXPECT_EQ(MiniWMError::WM_ERROR_SAMGR, WMClientMini::GetAccessibilityWindowInfo(TEST_USER_ID, infos));

    sessionManagerRemote_->sendRequestRet_ = IPC_OK;
    sessionManagerRemote_->sceneRemote_ = nullptr;
    EXPECT_EQ(MiniWMError::WM_ERROR_SAMGR, WMClientMini::GetAccessibilityWindowInfo(TEST_USER_ID, infos));
}

/**
 * @tc.name: WriteFailurePointIpc001
 * @tc.desc: Test IPC write failure branches
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WMClientMiniIpcTest, WriteFailurePointIpc001, TestSize.Level0)
{
    std::vector<sptr<MiniAccessibilityWindowInfo>> windowInfos;
    std::vector<sptr<MiniUnreliableWindowInfo>> unreliableInfos;

    WmMiniClientTest::SetWriteFailurePoint(WRITE_FAIL_MOCK_INTERFACE_TOKEN);
    EXPECT_EQ(MiniWMError::WM_ERROR_SAMGR, WMClientMini::GetAccessibilityWindowInfo(TEST_USER_ID,
        windowInfos));

    WmMiniClientTest::SetMultiInstanceEnabled(true);
    WmMiniClientTest::SetWriteFailurePoint(WRITE_FAIL_MOCK_USER_ID);
    EXPECT_EQ(MiniWMError::WM_ERROR_SAMGR, WMClientMini::GetAccessibilityWindowInfo(TEST_USER_ID,
        windowInfos));

    WmMiniClientTest::SetMultiInstanceEnabled(false);
    WmMiniClientTest::SetWriteFailurePoint(WRITE_FAIL_SESSION_INTERFACE_TOKEN);
    EXPECT_EQ(MiniWMError::WM_ERROR_SAMGR, WMClientMini::GetAccessibilityWindowInfo(TEST_USER_ID,
        windowInfos));

    WmMiniClientTest::SetWriteFailurePoint(WRITE_FAIL_WINDOW_INFO_INTERFACE_TOKEN);
    EXPECT_EQ(MiniWMError::WM_ERROR_IPC_FAILED, WMClientMini::GetAccessibilityWindowInfo(TEST_USER_ID,
        windowInfos));

    WmMiniClientTest::SetWriteFailurePoint(WRITE_FAIL_UNRELIABLE_INTERFACE_TOKEN);
    EXPECT_EQ(MiniWMError::WM_ERROR_IPC_FAILED, WMClientMini::GetUnreliableWindowInfo(TEST_WINDOW_ID,
        TEST_USER_ID, unreliableInfos));

    WmMiniClientTest::SetWriteFailurePoint(WRITE_FAIL_UNRELIABLE_WINDOW_ID);
    EXPECT_EQ(MiniWMError::WM_ERROR_IPC_FAILED, WMClientMini::GetUnreliableWindowInfo(TEST_WINDOW_ID,
        TEST_USER_ID, unreliableInfos));

    WmMiniClientTest::SetWriteFailurePoint(WRITE_FAIL_NONE);
}

/**
 * @tc.name: WindowInfoReplyIpc001
 * @tc.desc: Test window info IPC reply failure branches
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WMClientMiniIpcTest, WindowInfoReplyIpc001, TestSize.Level0)
{
    std::vector<sptr<MiniAccessibilityWindowInfo>> infos;

    sceneRemote_->sendRequestRet_ = IPC_FAILED;
    EXPECT_EQ(MiniWMError::WM_ERROR_IPC_FAILED, WMClientMini::GetAccessibilityWindowInfo(TEST_USER_ID, infos));

    sceneRemote_->sendRequestRet_ = IPC_OK;
    sceneRemote_->replyType_ = SceneReplyType::BROKEN_VECTOR;
    EXPECT_EQ(MiniWMError::WM_ERROR_IPC_FAILED, WMClientMini::GetAccessibilityWindowInfo(TEST_USER_ID, infos));

    sceneRemote_->replyType_ = SceneReplyType::VECTOR_NO_READABLE_BYTES;
    EXPECT_EQ(MiniWMError::WM_ERROR_IPC_FAILED, WMClientMini::GetAccessibilityWindowInfo(TEST_USER_ID, infos));

    sceneRemote_->replyType_ = SceneReplyType::VECTOR_ONE_BYTE_ENTRY;
    EXPECT_EQ(MiniWMError::WM_ERROR_IPC_FAILED, WMClientMini::GetAccessibilityWindowInfo(TEST_USER_ID, infos));

    sceneRemote_->replyType_ = SceneReplyType::PARTIAL_PARCELABLE;
    EXPECT_EQ(MiniWMError::WM_ERROR_IPC_FAILED, WMClientMini::GetAccessibilityWindowInfo(TEST_USER_ID, infos));

    sceneRemote_->replyType_ = SceneReplyType::NO_REPLY_ERR_CODE;
    EXPECT_EQ(MiniWMError::WM_ERROR_IPC_FAILED, WMClientMini::GetAccessibilityWindowInfo(TEST_USER_ID, infos));

    sceneRemote_->replyType_ = SceneReplyType::ERROR_CODE;
    sceneRemote_->replyErrCode_ = MiniWMError::WM_ERROR_INVALID_WINDOW;
    EXPECT_EQ(MiniWMError::WM_ERROR_INVALID_WINDOW, WMClientMini::GetAccessibilityWindowInfo(TEST_USER_ID, infos));
}

/**
 * @tc.name: UnreliableWindowInfoReplyIpc001
 * @tc.desc: Test unreliable window info IPC reply failure branches
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WMClientMiniIpcTest, UnreliableWindowInfoReplyIpc001, TestSize.Level0)
{
    std::vector<sptr<MiniUnreliableWindowInfo>> infos;

    WmMiniClientTest::SetWmsRemote(nullptr);
    EXPECT_EQ(MiniWMError::WM_ERROR_SAMGR, WMClientMini::GetUnreliableWindowInfo(TEST_WINDOW_ID,
        TEST_USER_ID, infos));

    WmMiniClientTest::SetWmsRemote(wmsRemote_);
    sceneRemote_->sendRequestRet_ = IPC_FAILED;
    EXPECT_EQ(MiniWMError::WM_ERROR_IPC_FAILED, WMClientMini::GetUnreliableWindowInfo(TEST_WINDOW_ID,
        TEST_USER_ID, infos));

    sceneRemote_->sendRequestRet_ = IPC_OK;
    sceneRemote_->replyType_ = SceneReplyType::NO_REPLY_ERR_CODE;
    EXPECT_EQ(MiniWMError::WM_ERROR_IPC_FAILED, WMClientMini::GetUnreliableWindowInfo(TEST_WINDOW_ID,
        TEST_USER_ID, infos));

    sceneRemote_->replyType_ = SceneReplyType::ERROR_CODE;
    sceneRemote_->replyErrCode_ = MiniWMError::WM_ERROR_INVALID_WINDOW;
    EXPECT_EQ(MiniWMError::WM_ERROR_INVALID_WINDOW, WMClientMini::GetUnreliableWindowInfo(TEST_WINDOW_ID,
        TEST_USER_ID, infos));
}

/**
 * @tc.name: ParcelableVectorIpc001
 * @tc.desc: Test forced parcelable vector guard branches
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WMClientMiniIpcTest, ParcelableVectorIpc001, TestSize.Level0)
{
    std::vector<sptr<MiniAccessibilityWindowInfo>> windowInfos;
    std::vector<sptr<MiniUnreliableWindowInfo>> unreliableInfos;

    WmMiniClientTest::SetParcelableVectorSizeOverLimit(true);
    EXPECT_EQ(MiniWMError::WM_ERROR_IPC_FAILED, WMClientMini::GetAccessibilityWindowInfo(TEST_USER_ID,
        windowInfos));

    WmMiniClientTest::SetParcelableVectorSizeOverLimit(false);
    WmMiniClientTest::SetParcelableVectorResizeFail(true);
    EXPECT_EQ(MiniWMError::WM_ERROR_IPC_FAILED, WMClientMini::GetUnreliableWindowInfo(TEST_WINDOW_ID,
        TEST_USER_ID, unreliableInfos));

    WmMiniClientTest::SetParcelableVectorResizeFail(false);
}
