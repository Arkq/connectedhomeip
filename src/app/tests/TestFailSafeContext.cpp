/*
 *
 *    Copyright (c) 2020-2022 Project CHIP Authors
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
 *      This file implements a unit test suite for the Configuration Manager
 *      code functionality.
 *
 */

#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gtest/gtest.h>

#include <app/FailSafeContext.h>
#include <lib/support/CHIPMem.h>
#include <lib/support/CodeUtils.h>
#include <platform/CHIPDeviceLayer.h>

using namespace chip;
using namespace chip::Logging;
using namespace chip::DeviceLayer;

namespace {

constexpr FabricIndex kTestAccessingFabricIndex1 = 1;
constexpr FabricIndex kTestAccessingFabricIndex2 = 2;

class TestFailSafeContext : public ::testing::Test
{
public:
    static void SetUpTestSuite()
    {
        ASSERT_EQ(chip::Platform::MemoryInit(), CHIP_NO_ERROR);
        ASSERT_EQ(PlatformMgr().InitChipStack(), CHIP_NO_ERROR);
    }
    static void TearDownTestSuite()
    {
        PlatformMgr().Shutdown();
        chip::Platform::MemoryShutdown();
    }
};

TEST_F(TestFailSafeContext, ArmFailSafe)
{
    CHIP_ERROR err = CHIP_NO_ERROR;

    chip::app::FailSafeContext failSafeContext;

    err = failSafeContext.ArmFailSafe(kTestAccessingFabricIndex1, System::Clock::Seconds16(1));
    EXPECT_EQ(err, CHIP_NO_ERROR);
    EXPECT_EQ(failSafeContext.IsFailSafeArmed(), true);
    EXPECT_EQ(failSafeContext.GetFabricIndex(), kTestAccessingFabricIndex1);
    EXPECT_EQ(failSafeContext.IsFailSafeArmed(kTestAccessingFabricIndex1), true);
    EXPECT_EQ(failSafeContext.IsFailSafeArmed(kTestAccessingFabricIndex2), false);

    failSafeContext.DisarmFailSafe();
    EXPECT_EQ(failSafeContext.IsFailSafeArmed(), false);
}

TEST_F(TestFailSafeContext, NocCommandInvoked)
{
    CHIP_ERROR err = CHIP_NO_ERROR;

    chip::app::FailSafeContext failSafeContext;

    err = failSafeContext.ArmFailSafe(kTestAccessingFabricIndex1, System::Clock::Seconds16(1));
    EXPECT_EQ(err, CHIP_NO_ERROR);
    EXPECT_EQ(failSafeContext.GetFabricIndex(), kTestAccessingFabricIndex1);

    failSafeContext.SetAddNocCommandInvoked(kTestAccessingFabricIndex2);
    EXPECT_EQ(failSafeContext.NocCommandHasBeenInvoked(), true);
    EXPECT_EQ(failSafeContext.AddNocCommandHasBeenInvoked(), true);
    EXPECT_EQ(failSafeContext.GetFabricIndex(), kTestAccessingFabricIndex2);

    failSafeContext.SetUpdateNocCommandInvoked();
    EXPECT_EQ(failSafeContext.NocCommandHasBeenInvoked(), true);
    EXPECT_EQ(failSafeContext.UpdateNocCommandHasBeenInvoked(), true);

    failSafeContext.DisarmFailSafe();
}

} // namespace
