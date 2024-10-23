using Xunit;
using KrysalisManaged;
using Xunit.Sdk;
using System.Runtime;
using System;

[assembly: CollectionBehavior(DisableTestParallelization = true)]

namespace KrysalisManagedTests
{
    public class KrysalisManagedTests
    {
        [STAThread]
        [StaFact]
        public void Tests()
        {
            bool result = KrysalisManaged.KrysalisManaged.main_ffi();
            Assert.True(result);
        }
    }
}
