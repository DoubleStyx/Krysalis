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
#if WINDOWS
            "KrysalisNative.dll";
#elif LINUX
            "libKrysalisNative.so";
#elif OSX
            "libKrysalisNative.dylib";
#else
            "KrysalisNative";
#endif

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    public static extern bool main_ffi();
}
