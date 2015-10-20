
import ctypes

lib = ctypes.cdll.LoadLibrary('TestApiDll')

class testapi_String_in(ctypes.Structure):
    _fields_ = [("data", ctypes.c_char_p),
                ("length", ctypes.c_uint32)]

class testapi_String_out(ctypes.Structure):
    _fields_ = [("setter", ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.c_char_p, ctypes.c_uint32)),
                ("object", ctypes.c_void_p)]

class testapi_String(ctypes.Structure):
    _fields_ = [("in_", testapi_String_in),
                ("out_", testapi_String_out)]

class testapi_Item(ctypes.Structure):
    _fields_ = [("i", ctypes.c_int32),
                ("s", testapi_String)]

class ItemCollection(object):
    def __init__(self):
        self.m_imp = lib.testapi_ItemCollection_create()

    def addItem(self, i, s):
        item = testapi_Item(i = i)
        bs = bytes(s, 'mbcs');
        item.s.in_.data = bs;
        item.s.in_.length = len(bs);
        lib.testapi_ItemCollection_addItem(self.m_imp, ctypes.byref(item))

    def items(self):
        enumerator = lib.testapi_ItemCollection_getItemEnumerator(self.m_imp)
        while lib.testapi_ItemEnumerator_moveNext(enumerator):
            item = testapi_Item()
            StringSetter = ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.c_char_p, ctypes.c_uint32)
            localBuffer = ''
            def stringSetter(obj, data, length):
                coll = enumerator
                nonlocal localBuffer
                localBuffer = str(data, 'mbcs')
            v = ctypes.addressof(item.s)
            item.s.out_.setter = StringSetter(stringSetter)
            lib.testapi_ItemEnumerator_getCurrent(enumerator, ctypes.byref(item))
            yield (item.i, localBuffer)
        lib.testapi_ItemEnumerator_destroy(enumerator)
