/*
 *
 *    Copyright (c) 2021 Project CHIP Authors
 *    All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

/**
 *    @file
 *      This file implements unit tests for CommandPathParams
 *
 */

#include <gtest/gtest.h>

#include <app/AttributePathParams.h>
#include <app/DataVersionFilter.h>
#include <app/EventPathParams.h>
#include <app/util/mock/Constants.h>

using namespace chip::Test;

namespace chip {
namespace app {
namespace TestPath {

TEST(TestClusterInfo, TestAttributePathIntersect)
{
    EndpointId endpointIdArray[2]   = { 1, kInvalidEndpointId };
    ClusterId clusterIdArray[2]     = { 2, kInvalidClusterId };
    AttributeId attributeIdArray[2] = { 3, kInvalidAttributeId };

    for (auto endpointId1 : endpointIdArray)
    {
        for (auto clusterId1 : clusterIdArray)
        {
            for (auto attributeId1 : attributeIdArray)
            {
                for (auto endpointId2 : endpointIdArray)
                {
                    for (auto clusterId2 : clusterIdArray)
                    {
                        for (auto attributeId2 : attributeIdArray)
                        {
                            AttributePathParams path1;
                            path1.mEndpointId  = endpointId1;
                            path1.mClusterId   = clusterId1;
                            path1.mAttributeId = attributeId1;
                            AttributePathParams path2;
                            path2.mEndpointId  = endpointId2;
                            path2.mClusterId   = clusterId2;
                            path2.mAttributeId = attributeId2;
                            EXPECT_TRUE(path1.Intersects(path2));
                        }
                    }
                }
            }
        }
    }

    {
        AttributePathParams path1;
        path1.mEndpointId = 1;
        AttributePathParams path2;
        path2.mEndpointId = 2;
        EXPECT_TRUE(!path1.Intersects(path2));
    }

    {
        AttributePathParams path1;
        path1.mClusterId = 1;
        AttributePathParams path2;
        path2.mClusterId = 2;
        EXPECT_TRUE(!path1.Intersects(path2));
    }

    {
        AttributePathParams path1;
        path1.mAttributeId = 1;
        AttributePathParams path2;
        path2.mAttributeId = 2;
        EXPECT_TRUE(!path1.Intersects(path2));
    }
}

TEST(TestClusterInfo, TestAttributePathIncludedSameFieldId)
{
    AttributePathParams clusterInfo1;
    AttributePathParams clusterInfo2;
    AttributePathParams clusterInfo3;
    clusterInfo1.mAttributeId = 1;
    clusterInfo2.mAttributeId = 1;
    clusterInfo3.mAttributeId = 1;
    EXPECT_TRUE(clusterInfo1.IsAttributePathSupersetOf(clusterInfo2));
    clusterInfo2.mListIndex = 1;
    EXPECT_TRUE(clusterInfo1.IsAttributePathSupersetOf(clusterInfo2));
    clusterInfo1.mListIndex = 0;
    EXPECT_TRUE(!clusterInfo1.IsAttributePathSupersetOf(clusterInfo3));
    clusterInfo3.mListIndex = 0;
    EXPECT_TRUE(clusterInfo1.IsAttributePathSupersetOf(clusterInfo3));
    clusterInfo3.mListIndex = 1;
    EXPECT_TRUE(!clusterInfo1.IsAttributePathSupersetOf(clusterInfo3));
}

TEST(TestClusterInfo, TestAttributePathIncludedDifferentFieldId)
{
    {
        AttributePathParams clusterInfo1;
        AttributePathParams clusterInfo2;
        clusterInfo1.mAttributeId = 1;
        clusterInfo2.mAttributeId = 2;
        EXPECT_TRUE(!clusterInfo1.IsAttributePathSupersetOf(clusterInfo2));
    }
    {
        AttributePathParams clusterInfo1;
        AttributePathParams clusterInfo2;
        clusterInfo2.mAttributeId = 2;
        EXPECT_TRUE(clusterInfo1.IsAttributePathSupersetOf(clusterInfo2));
    }
    {
        AttributePathParams clusterInfo1;
        AttributePathParams clusterInfo2;
        EXPECT_TRUE(clusterInfo1.IsAttributePathSupersetOf(clusterInfo2));
    }
    {
        AttributePathParams clusterInfo1;
        AttributePathParams clusterInfo2;

        clusterInfo1.mAttributeId = 1;
        EXPECT_TRUE(!clusterInfo1.IsAttributePathSupersetOf(clusterInfo2));
    }
}

TEST(TestClusterInfo, TestAttributePathIncludedDifferentEndpointId)
{
    AttributePathParams clusterInfo1;
    AttributePathParams clusterInfo2;
    clusterInfo1.mEndpointId = 1;
    clusterInfo2.mEndpointId = 2;
    EXPECT_TRUE(!clusterInfo1.IsAttributePathSupersetOf(clusterInfo2));
}

TEST(TestClusterInfo, TestAttributePathIncludedDifferentClusterId)
{
    AttributePathParams clusterInfo1;
    AttributePathParams clusterInfo2;
    clusterInfo1.mClusterId = 1;
    clusterInfo2.mClusterId = 2;
    EXPECT_TRUE(!clusterInfo1.IsAttributePathSupersetOf(clusterInfo2));
}

/*
{kInvalidEndpointId, kInvalidClusterId, kInvalidEventId},
{kInvalidEndpointId, MockClusterId(1), kInvalidEventId},
{kInvalidEndpointId, MockClusterId(1), MockEventId(1)},
{kMockEndpoint1, kInvalidClusterId, kInvalidEventId},
{kMockEndpoint1, MockClusterId(1), kInvalidEventId},
{kMockEndpoint1, MockClusterId(1), MockEventId(1)},
*/
chip::app::EventPathParams validEventpaths[6];
void InitEventPaths()
{
    validEventpaths[1].mClusterId  = MockClusterId(1);
    validEventpaths[2].mClusterId  = MockClusterId(1);
    validEventpaths[2].mEventId    = MockEventId(1);
    validEventpaths[3].mEndpointId = kMockEndpoint1;
    validEventpaths[4].mEndpointId = kMockEndpoint1;
    validEventpaths[4].mClusterId  = MockClusterId(1);
    validEventpaths[5].mEndpointId = kMockEndpoint1;
    validEventpaths[5].mClusterId  = MockClusterId(1);
    validEventpaths[5].mEventId    = MockEventId(1);
}

TEST(TestClusterInfo, TestEventPathSameEventId)
{
    ConcreteEventPath testPath(kMockEndpoint1, MockClusterId(1), MockEventId(1));
    for (auto & path : validEventpaths)
    {
        EXPECT_TRUE(path.IsValidEventPath());
        EXPECT_TRUE(path.IsEventPathSupersetOf(testPath));
    }
}

TEST(TestClusterInfo, TestEventPathDifferentEventId)
{
    ConcreteEventPath testPath(kMockEndpoint1, MockClusterId(1), MockEventId(2));
    EXPECT_TRUE(validEventpaths[0].IsEventPathSupersetOf(testPath));
    EXPECT_TRUE(validEventpaths[1].IsEventPathSupersetOf(testPath));
    EXPECT_TRUE(!validEventpaths[2].IsEventPathSupersetOf(testPath));
    EXPECT_TRUE(validEventpaths[3].IsEventPathSupersetOf(testPath));
    EXPECT_TRUE(validEventpaths[4].IsEventPathSupersetOf(testPath));
    EXPECT_TRUE(!validEventpaths[5].IsEventPathSupersetOf(testPath));
}

TEST(TestClusterInfo, TestEventPathDifferentClusterId)
{
    ConcreteEventPath testPath(kMockEndpoint1, MockClusterId(2), MockEventId(1));
    EXPECT_TRUE(validEventpaths[0].IsEventPathSupersetOf(testPath));
    EXPECT_TRUE(!validEventpaths[1].IsEventPathSupersetOf(testPath));
    EXPECT_TRUE(!validEventpaths[2].IsEventPathSupersetOf(testPath));
    EXPECT_TRUE(validEventpaths[3].IsEventPathSupersetOf(testPath));
    EXPECT_TRUE(!validEventpaths[4].IsEventPathSupersetOf(testPath));
    EXPECT_TRUE(!validEventpaths[5].IsEventPathSupersetOf(testPath));
}

TEST(TestClusterInfo, TestEventPathDifferentEndpointId)
{
    ConcreteEventPath testPath(kMockEndpoint2, MockClusterId(1), MockEventId(1));
    EXPECT_TRUE(validEventpaths[0].IsEventPathSupersetOf(testPath));
    EXPECT_TRUE(validEventpaths[1].IsEventPathSupersetOf(testPath));
    EXPECT_TRUE(validEventpaths[2].IsEventPathSupersetOf(testPath));
    EXPECT_TRUE(!validEventpaths[3].IsEventPathSupersetOf(testPath));
    EXPECT_TRUE(!validEventpaths[4].IsEventPathSupersetOf(testPath));
    EXPECT_TRUE(!validEventpaths[5].IsEventPathSupersetOf(testPath));
}

} // namespace TestPath
} // namespace app
} // namespace chip
