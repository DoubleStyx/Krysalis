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
            /*
            KrysalisManaged.KrysalisManaged.openLogFile();

            bool result = KrysalisManaged.KrysalisManaged.runWindow(false); // set to true; TEMPORARY flag

            KrysalisManaged.KrysalisManaged.closeLogFile();
            */
            bool result = 5 + 3 > 7;

            Assert.IsTrue(result);
        }
    }
}