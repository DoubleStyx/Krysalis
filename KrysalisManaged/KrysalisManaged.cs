using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace KrysalisManaged;

public class KrysalisManaged
{
    [DllImport("KrysalisNative.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern bool main_ffi();
}
