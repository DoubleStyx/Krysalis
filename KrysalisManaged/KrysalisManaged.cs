using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace KrysalisManaged;

public class KrysalisManaged
{
        private const string LibraryName =
#if Windows
            "KrysalisNative.dll";
#elif Macos
            "libKrysalisNative.dylib";
#elif Linux
            "libKrysalisNative.so";
#endif

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern bool main_ffi();
}
