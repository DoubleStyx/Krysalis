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
        public void Tests()
        {
            bool result = KrysalisManaged.main_ffi();

            Assert.IsTrue(result);
        }
    }
}
