﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace KrysalisManaged;
internal class Utils {
	[DllImport("KrysalisNative.dll", CallingConvention = CallingConvention.Cdecl)]
	public static extern int Add(int a, int b);
}