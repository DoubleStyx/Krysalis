// Test.cpp
// Runs the unit tests for KrysalisNative
#include "KrysalisNative.h"
#include "Utils.h"
#include "SceneBuilder.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace KrysalisNativeTests
{

    TEST_CLASS(RendererTests)
    {
    public:
        TEST_METHOD(TestRenderer)
        {
            openLogFile();

            bool result = runWindow(true);

            closeLogFile();

            Assert::IsTrue(result, L"Rendering test failed!");
        }
    };
}

int main() {
    return 0;
}