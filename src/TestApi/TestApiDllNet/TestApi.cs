using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace TestApiDllNet.detail
{
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    internal struct testapi_String_in
    {
        [MarshalAs(UnmanagedType.LPStr)]
        public string data;
        public UInt32 length;
    };

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    internal struct testapi_String_out
    {
        public IntPtr setter;
        public IntPtr object_;
    };

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    internal struct testapi_String
    {
        public testapi_String_in in_;
        public testapi_String_out out_;
    };

    internal class StringSetter
    {
        public static void set(ref testapi_String_in stringIn, string s)
        {
            stringIn.data = s;
            stringIn.length = (UInt32)s.Length;
        }
    }

    internal class StringGetter : IDisposable
    {
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void StringSetterDelegate(IntPtr obj, IntPtr data, UInt32 length);

        private static void stringSetterFunction(IntPtr obj, IntPtr data, UInt32 length)
        {
            var stringHandle = GCHandle.FromIntPtr(obj);
            stringHandle.Target = Marshal.PtrToStringAnsi(data, (int)length);
        }

        private GCHandle m_stringHandle;

        public StringGetter(ref testapi_String_out stringOut)
        {
            m_stringHandle = GCHandle.Alloc(null, GCHandleType.Pinned);
            stringOut.object_ = GCHandle.ToIntPtr(m_stringHandle);
            stringOut.setter = Marshal.GetFunctionPointerForDelegate(new StringSetterDelegate(stringSetterFunction));
        }

        public string get()
        {
            var theString = (string)m_stringHandle.Target;
            return theString;
        }

        public void Dispose()
        {
            if (m_stringHandle.IsAllocated)
                m_stringHandle.Free();
        }
    };

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    internal struct testapi_Item
    {
       public Int32 i;
       public testapi_String s;
    }

    internal class ItemEnumerator : IEnumerator<Item>
    {
        [DllImport("TestApiDll.dll", CallingConvention = CallingConvention.Cdecl)]
        private extern static void testapi_ItemEnumerator_destroy(IntPtr enumerator);

        [DllImport("TestApiDll.dll", CallingConvention = CallingConvention.Cdecl)]
        private extern static void testapi_ItemEnumerator_getCurrent(IntPtr enumerator, ref testapi_Item c_item);

        [DllImport("TestApiDll.dll", CallingConvention = CallingConvention.Cdecl)]
        private extern static bool testapi_ItemEnumerator_moveNext(IntPtr enumerator);

        [DllImport("TestApiDll.dll", CallingConvention = CallingConvention.Cdecl)]
        private extern static void testapi_ItemEnumerator_reset(IntPtr enumerator);

        [DllImport("TestApiDll.dll", CallingConvention = CallingConvention.Cdecl)]
        private extern static IntPtr testapi_ItemEnumerator_clone(IntPtr enumerator);

        [DllImport("TestApiDll.dll", CallingConvention = CallingConvention.Cdecl)]
        private extern static bool testapi_ItemEnumerator_equal(IntPtr enumerator1, IntPtr enumerator2);

        IntPtr m_imp;

        public ItemEnumerator(IntPtr imp)
        {
            m_imp = imp;
        }

        public void Dispose()
        {
            if (m_imp != IntPtr.Zero)
            {
                testapi_ItemEnumerator_destroy(m_imp);
                m_imp = IntPtr.Zero;
            }
        }

        private Item getCurrent()
        {
            testapi_Item c_item = new detail.testapi_Item();
            using(var stringGetter = new detail.StringGetter(ref c_item.s.out_))
            {
                testapi_ItemEnumerator_getCurrent(m_imp, ref c_item);
                Item item = new Item { I = c_item.i, S = stringGetter.get() };
                return item;
            }
        }

        public Item Current
        {
            get { return getCurrent(); }
        }

        object System.Collections.IEnumerator.Current
        {
            get { return getCurrent(); }
        }

        public bool MoveNext()
        {
            return testapi_ItemEnumerator_moveNext(m_imp);
        }

        public void Reset()
        {
            testapi_ItemEnumerator_reset(m_imp);
        }
    }
}

namespace TestApiDllNet
{
    public class Item
    {
        public int I { get; set; }
        public string S { get; set; }
    }

    public class ItemCollection : IDisposable, IEnumerable<Item>
    {
        [DllImport("TestApiDll.dll", CallingConvention = CallingConvention.Cdecl)]
        private extern static IntPtr testapi_ItemCollection_create();

        [DllImport("TestApiDll.dll", CallingConvention = CallingConvention.Cdecl)]
        private extern static void testapi_ItemCollection_destroy(IntPtr collection);

        [DllImport("TestApiDll.dll", CallingConvention = CallingConvention.Cdecl)]
        private extern static void testapi_ItemCollection_addItem(IntPtr collection, ref detail.testapi_Item c_item);

        [DllImport("TestApiDll.dll", CallingConvention = CallingConvention.Cdecl)]
        private extern static IntPtr testapi_ItemCollection_getItemEnumerator(IntPtr collection);

        IntPtr m_imp = testapi_ItemCollection_create();

        public void Dispose()
        {
            if (m_imp != IntPtr.Zero)
            {
                testapi_ItemCollection_destroy(m_imp);
                m_imp = IntPtr.Zero;
            }
        }

        public void addItem(Item item)
        {
            detail.testapi_Item c_item = new detail.testapi_Item();
            c_item.i = item.I;
            detail.StringSetter.set(ref c_item.s.in_, item.S);
            testapi_ItemCollection_addItem(m_imp, ref c_item);
        }

        public IEnumerator<Item> GetEnumerator()
        {
            return new detail.ItemEnumerator(testapi_ItemCollection_getItemEnumerator(m_imp));
        }

        System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator()
        {
            return new detail.ItemEnumerator(testapi_ItemCollection_getItemEnumerator(m_imp));
        }
    }
}
