using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace TestOliveCSharp
{
    class TestOlive
    {
        [DllImport("TestOliveC.dll", CallingConvention = CallingConvention.Cdecl)]
        private extern static Int32 TestOliveC_onChangedFieldV1(
            Int32 fieldId,
            [MarshalAs(UnmanagedType.LPWStr)] string oldValue,
            [MarshalAs(UnmanagedType.LPWStr)] string newValue,
            IntPtr messageData,
            UInt32 messageMaxSize
            );

        [DllImport("TestOliveC.dll", CallingConvention = CallingConvention.Cdecl)]
        private extern static Int32 TestOliveC_onChangedFieldV2(
            Int32 fieldId,
            [MarshalAs(UnmanagedType.LPWStr)] string oldValue,
            [MarshalAs(UnmanagedType.LPWStr)] string newValue,
            out IntPtr messageData,
            out UInt32 messageSize
            );

        [DllImport("TestOliveC.dll", CallingConvention = CallingConvention.Cdecl)]
        private extern static Int32 TestOliveC_destroyMessageData(
            IntPtr messageData
            );

        public static int onChangedFieldV1(int fieldId, string oldValue, string newValue, out string message)
        {
            const UInt32 messageMaxSize = 500;
#if METHODE_AVEC_PIN
            char[] messageArray = new char[messageMaxSize];
            GCHandle pinnedMessageArray = GCHandle.Alloc(messageArray, GCHandleType.Pinned);
            IntPtr messageData = pinnedMessageArray.AddrOfPinnedObject();
            var result = TestOliveC_onChangedFieldV1(fieldId, oldValue, newValue, messageData, messageMaxSize);
            message = Marshal.PtrToStringUni(messageData);
            pinnedMessageArray.Free();
#else
            IntPtr messageData = Marshal.AllocHGlobal((int)(messageMaxSize * sizeof(char)));
            var result = TestOliveC_onChangedFieldV1(fieldId, oldValue, newValue, messageData, messageMaxSize);
            message = Marshal.PtrToStringUni(messageData);
            Marshal.FreeHGlobal(messageData);
#endif
            return result;
        }

        public static int onChangedFieldV2(int fieldId, string oldValue, string newValue, out string message)
        {
            IntPtr messageData;
            UInt32 messageSize;
            var result = TestOliveC_onChangedFieldV2(fieldId, oldValue, newValue, out messageData, out messageSize);
            message = Marshal.PtrToStringUni(messageData, (int)messageSize);
            TestOliveC_destroyMessageData(messageData);
            return 0;
        }
    }
}
