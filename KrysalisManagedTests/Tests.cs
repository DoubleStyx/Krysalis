using Xunit;
using KrysalisManaged;
using Xunit.Sdk;

[assembly: CollectionBehavior(DisableTestParallelization = true)]

namespace KrysalisManagedTests
{
    public class KrysalisManagedTests
    {
        [STAThread] // You can still use the STAThread attribute for the COM issue
        [StaFact] // xUnit attribute for a test method
        public void Tests()
        {
            bool result = KrysalisManaged.KrysalisManaged.main_ffi();
            Assert.True(result); // xUnit uses Assert.True instead of Assert.IsTrue
        }
    }
}
