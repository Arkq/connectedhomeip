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
 *      This file implements unit tests for EventPathParams
 *
 */

#include <gtest/gtest.h>

#include <app/EventPathParams.h>

namespace chip {
namespace app {
namespace TestEventPathParams {

TEST(TestEventPathParams, TestSamePath)
{
    EventPathParams eventPathParams1(2, 3, 4);
    EventPathParams eventPathParams2(2, 3, 4);
    EXPECT_TRUE(eventPathParams1.IsSamePath(eventPathParams2));
}

TEST(TestEventPathParams, TestDifferentEndpointId)
{
    EventPathParams eventPathParams1(2, 3, 4);
    EventPathParams eventPathParams2(6, 3, 4);
    EXPECT_TRUE(!eventPathParams1.IsSamePath(eventPathParams2));
}

TEST(TestEventPathParams, TestDifferentClusterId)
{
    EventPathParams eventPathParams1(2, 3, 4);
    EventPathParams eventPathParams2(2, 6, 4);
    EXPECT_TRUE(!eventPathParams1.IsSamePath(eventPathParams2));
}

TEST(TestEventPathParams, TestDifferentEventId)
{
    EventPathParams eventPathParams1(2, 3, 4);
    EventPathParams eventPathParams2(2, 3, 6);
    EXPECT_TRUE(!eventPathParams1.IsSamePath(eventPathParams2));
}

} // namespace TestEventPathParams
} // namespace app
} // namespace chip
