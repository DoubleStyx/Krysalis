// Test.cpp
// Runs the unit tests
#include "KrysalisNative.h"
#include "Utils.h"
#include "SceneBuilder.h"
#include "CppUnitTest.h"  // Microsoft Unit Testing framework

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace KrysalisNativeTests
{
    TEST_CLASS(RendererTests)
    {
    public:
        // Test method for renderer
        TEST_METHOD(TestRenderer)
        {
            openLogFile();

            // Call the function you're testing
            bool result = runWindow(true);

            closeLogFile();

            // Use the Microsoft Unit Testing Framework's Assert
            Assert::IsTrue(result, L"Rendering test failed!");
        }
    };
}
