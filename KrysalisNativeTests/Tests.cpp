// Test.cpp
// Runs the unit tests for KrysalisNative
#include "KrysalisNative.h"
#include "Utils.h"
#include "SceneBuilder.h"
#include <gtest/gtest.h>

// Unit test for the Add function
TEST(AddTest, Add) {
    openLogFile();

    bool result = runWindow(true);

    closeLogFile();

    EXPECT_EQ(result, true);
}
