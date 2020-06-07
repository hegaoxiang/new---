using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace GEditor.import
{
    class IGCore
    {
        [DllImport(@"Build\GEngine.dll")]
        public static extern void InitD3D(IntPtr hwnd, double width, double height);

        [DllImport(@"Build\GEngine.dll")]
        public static extern int RenderFrame();
// 
//         [DllImport(@"Core.dll")]
//         public static extern void SetWorkDirectory([MarshalAs(UnmanagedType.LPWStr)] string dir);
// 
//         [DllImport(@"Core1.dll")]
//         public static extern void MsgProc(IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam);
    }
}
