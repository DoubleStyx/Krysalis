using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;

namespace KrysalisNativeTests
{
    [TestClass]
    public class KrysalisNativeTests
    {
        [TestMethod]
        public void MyTest()
        {
            int a = 5;
            int b = 3;

            int result = a + b;

            Assert.AreEqual(8, result, "Add method should return the sum of two integers.");
        }
    }
}
