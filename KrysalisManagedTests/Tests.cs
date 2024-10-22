// Tests.cs
// Unit tests for KrysalisManaged
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using KrysalisManaged;

namespace KrysalisManagedTests
{
    [TestClass]
    public class KrysalisManagedTests
    {
        [TestMethod]
        public void MyTest()
        {
            bool result = 5 + 3 > 7;

            Assert.IsTrue(result);
        }
    }
}
