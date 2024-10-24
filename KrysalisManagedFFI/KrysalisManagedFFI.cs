using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace KrysalisManagedFFI;

public class KrysalisManagedFFI
{
    [DllImport("KrysalisNativeFFI.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern bool main_ffi();
}
