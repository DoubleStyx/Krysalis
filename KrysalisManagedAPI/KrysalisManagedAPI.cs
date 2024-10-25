using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using KrysalisManagedWrapper;

namespace KrysalisManagedAPI;

public class KrysalisManagedAPI
{
    public static bool StartRenderer()
    {
        return KrysalisManagedWrapper.KrysalisManagedWrapper.MainWrapper();
    }
}
