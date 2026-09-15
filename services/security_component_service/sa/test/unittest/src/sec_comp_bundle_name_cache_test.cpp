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
#include "sec_comp_bundle_name_cache_test.h"

#include "accesstoken_kit.h"
#include "sec_comp_err.h"

using namespace testing::ext;

namespace OHOS {
namespace Security {
namespace SecurityComponent {
using namespace OHOS::Security::AccessToken;

namespace {
constexpr AccessTokenID TEST_TOKEN_ID = 123;
constexpr AccessTokenID TEST_TOKEN_ID_SECOND = 456;
constexpr AccessTokenID TEST_INVALID_TOKEN_ID = 0;
const std::string TEST_BUNDLE_NAME = "com.example.bundle";
const std::string TEST_BUNDLE_NAME_SECOND = "com.example.second";
const std::string TEST_BUNDLE_NAME_THIRD = "com.example.third";
const std::string TEST_EMPTY_BUNDLE_NAME = "";
}

void SecCompBundleNameCacheTest::SetUpTestCase()
{
}

void SecCompBundleNameCacheTest::TearDownTestCase()
{
}

void SecCompBundleNameCacheTest::SetUp()
{
    // clean cache between cases: each case must start with an empty cache
    SecCompBundleNameCache::GetInstance().ClearCache();
    AccessTokenKit::getHapTokenInfoRes = 0;
    AccessTokenKit::hapTokenInfoRes_.bundleName = TEST_BUNDLE_NAME;
}

void SecCompBundleNameCacheTest::TearDown()
{
    AccessTokenKit::getHapTokenInfoRes = 0;
}

/*
 * @tc.name: GetBundleName001
 * @tc.desc: cache miss queries token info, returns bundle name and second query hits cache
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompBundleNameCacheTest, GetBundleName001, TestSize.Level1)
{
    // first query: cache miss branch, token info queried and value cached
    EXPECT_EQ(TEST_BUNDLE_NAME, SecCompBundleNameCache::GetInstance().GetBundleName(TEST_TOKEN_ID));
    // second query for the same token: cache hit branch, no new token info query
    EXPECT_EQ(TEST_BUNDLE_NAME, SecCompBundleNameCache::GetInstance().GetBundleName(TEST_TOKEN_ID));
    // third query still hits the same cached entry
    EXPECT_EQ(TEST_BUNDLE_NAME, SecCompBundleNameCache::GetInstance().GetBundleName(TEST_TOKEN_ID));
}

/*
 * @tc.name: GetBundleName002
 * @tc.desc: second query hits cache: token info result flips but cached value stays
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompBundleNameCacheTest, GetBundleName002, TestSize.Level1)
{
    // prime the cache with the first query
    EXPECT_EQ(TEST_BUNDLE_NAME, SecCompBundleNameCache::GetInstance().GetBundleName(TEST_TOKEN_ID));
    // flip mock result after the first query: cache must serve the old value
    AccessTokenKit::hapTokenInfoRes_.bundleName = TEST_BUNDLE_NAME_SECOND;
    EXPECT_EQ(TEST_BUNDLE_NAME, SecCompBundleNameCache::GetInstance().GetBundleName(TEST_TOKEN_ID));
    // flip again: cached value must still be the first one
    AccessTokenKit::hapTokenInfoRes_.bundleName = TEST_BUNDLE_NAME_THIRD;
    EXPECT_EQ(TEST_BUNDLE_NAME, SecCompBundleNameCache::GetInstance().GetBundleName(TEST_TOKEN_ID));
}

/*
 * @tc.name: GetBundleName003
 * @tc.desc: failed token query returns empty and is NOT cached: retry succeeds
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompBundleNameCacheTest, GetBundleName003, TestSize.Level1)
{
    // first query fails: empty result, failure branch taken
    AccessTokenKit::getHapTokenInfoRes = -1;
    EXPECT_EQ(TEST_EMPTY_BUNDLE_NAME, SecCompBundleNameCache::GetInstance().GetBundleName(TEST_TOKEN_ID));
    // failure is not cached: a later successful query must fill the cache
    AccessTokenKit::getHapTokenInfoRes = 0;
    EXPECT_EQ(TEST_BUNDLE_NAME, SecCompBundleNameCache::GetInstance().GetBundleName(TEST_TOKEN_ID));
    // now the cache holds the successful value: flipping the mock no longer matters
    AccessTokenKit::hapTokenInfoRes_.bundleName = TEST_BUNDLE_NAME_SECOND;
    EXPECT_EQ(TEST_BUNDLE_NAME, SecCompBundleNameCache::GetInstance().GetBundleName(TEST_TOKEN_ID));
}

/*
 * @tc.name: GetBundleName004
 * @tc.desc: invalid token id 0 returns empty without any token info query
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompBundleNameCacheTest, GetBundleName004, TestSize.Level1)
{
    // make any real query fail: proves the guard branch returns before querying
    AccessTokenKit::getHapTokenInfoRes = -1;
    EXPECT_EQ(TEST_EMPTY_BUNDLE_NAME, SecCompBundleNameCache::GetInstance().GetBundleName(TEST_INVALID_TOKEN_ID));
    // repeated invalid queries keep returning empty and never touch token info
    EXPECT_EQ(TEST_EMPTY_BUNDLE_NAME, SecCompBundleNameCache::GetInstance().GetBundleName(TEST_INVALID_TOKEN_ID));
    // a valid token still works after invalid ones: guard branch does not pollute state
    AccessTokenKit::getHapTokenInfoRes = 0;
    EXPECT_EQ(TEST_BUNDLE_NAME, SecCompBundleNameCache::GetInstance().GetBundleName(TEST_TOKEN_ID));
}

/*
 * @tc.name: GetBundleName005
 * @tc.desc: different token ids are cached separately
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompBundleNameCacheTest, GetBundleName005, TestSize.Level1)
{
    // first token cached with the first mock value
    EXPECT_EQ(TEST_BUNDLE_NAME, SecCompBundleNameCache::GetInstance().GetBundleName(TEST_TOKEN_ID));
    // second token cached with the second mock value
    AccessTokenKit::hapTokenInfoRes_.bundleName = TEST_BUNDLE_NAME_SECOND;
    EXPECT_EQ(TEST_BUNDLE_NAME_SECOND, SecCompBundleNameCache::GetInstance().GetBundleName(TEST_TOKEN_ID_SECOND));
    // first token keeps its own cached value after the mock flips again
    AccessTokenKit::hapTokenInfoRes_.bundleName = TEST_BUNDLE_NAME_THIRD;
    EXPECT_EQ(TEST_BUNDLE_NAME, SecCompBundleNameCache::GetInstance().GetBundleName(TEST_TOKEN_ID));
    // second token also keeps its own cached value
    EXPECT_EQ(TEST_BUNDLE_NAME_SECOND, SecCompBundleNameCache::GetInstance().GetBundleName(TEST_TOKEN_ID_SECOND));
}

/*
 * @tc.name: ClearCache001
 * @tc.desc: ClearCache drops cached values so the next query goes to token info again
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompBundleNameCacheTest, ClearCache001, TestSize.Level1)
{
    // prime the cache for two tokens
    EXPECT_EQ(TEST_BUNDLE_NAME, SecCompBundleNameCache::GetInstance().GetBundleName(TEST_TOKEN_ID));
    AccessTokenKit::hapTokenInfoRes_.bundleName = TEST_BUNDLE_NAME_SECOND;
    EXPECT_EQ(TEST_BUNDLE_NAME_SECOND, SecCompBundleNameCache::GetInstance().GetBundleName(TEST_TOKEN_ID_SECOND));
    // after clear: the new mock value must be served (not the stale cache)
    SecCompBundleNameCache::GetInstance().ClearCache();
    AccessTokenKit::hapTokenInfoRes_.bundleName = TEST_BUNDLE_NAME_THIRD;
    EXPECT_EQ(TEST_BUNDLE_NAME_THIRD, SecCompBundleNameCache::GetInstance().GetBundleName(TEST_TOKEN_ID));
    // the other token is also cleared and re-queried
    EXPECT_EQ(TEST_BUNDLE_NAME_THIRD, SecCompBundleNameCache::GetInstance().GetBundleName(TEST_TOKEN_ID_SECOND));
}

/*
 * @tc.name: GetInstance001
 * @tc.desc: GetInstance returns the same singleton and the instance stays usable
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SecCompBundleNameCacheTest, GetInstance001, TestSize.Level1)
{
    SecCompBundleNameCache& first = SecCompBundleNameCache::GetInstance();
    SecCompBundleNameCache& second = SecCompBundleNameCache::GetInstance();
    // both references must point to the same singleton object
    EXPECT_EQ(&first, &second);
    // the singleton is a functional cache: query through both references
    EXPECT_EQ(TEST_BUNDLE_NAME, first.GetBundleName(TEST_TOKEN_ID));
    EXPECT_EQ(TEST_BUNDLE_NAME, second.GetBundleName(TEST_TOKEN_ID));
    // clearing through one reference is observed through the other: same object
    SecCompBundleNameCache::GetInstance().ClearCache();
    AccessTokenKit::hapTokenInfoRes_.bundleName = TEST_BUNDLE_NAME_SECOND;
    EXPECT_EQ(TEST_BUNDLE_NAME_SECOND, second.GetBundleName(TEST_TOKEN_ID));
}
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
