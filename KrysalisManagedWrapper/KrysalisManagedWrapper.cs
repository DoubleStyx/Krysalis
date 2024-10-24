using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using KrysalisManagedFFI;

namespace KrysalisManagedWrapper;

public class KrysalisManagedWrapper
{
    public static bool MainFfi()
    {
        KrysalisManagedFFI.KrysalisManagedFFI.main_ffi();
    }
}
