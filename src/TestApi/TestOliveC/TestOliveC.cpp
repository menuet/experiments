
#include "TestOliveC.h"
#include <cwchar>


extern "C" {

    int32_t __cdecl TestOliveC_onChangedFieldV1(
        int32_t fieldId,
        const wchar_t* oldValue,
        const wchar_t* newValue,
        wchar_t* messageData,
        uint32_t messageMaxSize
        )
    {
        const bool oldEqualsNew = std::wcscmp(oldValue, newValue) == 0;
        std::swprintf(messageData, messageMaxSize, L"Les valeurs du champ %d sont %ws", fieldId, (oldEqualsNew ? L"egales" : L"differentes"));
        return oldEqualsNew ? 1 : 2;
    }

    int32_t __cdecl TestOliveC_onChangedFieldV2(
        int32_t fieldId,
        const wchar_t* oldValue,
        const wchar_t* newValue,
        wchar_t** messageData,
        uint32_t* messageSize
        )
    {
        const bool oldEqualsNew = std::wcscmp(oldValue, newValue) == 0;
        std::wstring message = L"Les valeurs du champ ";
        message += std::to_wstring(fieldId);
        message += L" sont ";
        message += (oldEqualsNew ? L"egales" : L"differentes");
        *messageData = new wchar_t[message.size() + 1];
        std::memcpy(*messageData, message.data(), (message.size() + 1) * sizeof(wchar_t));
        return oldEqualsNew ? 1 : 2;
    }

    void __cdecl TestOliveC_destroyMessageData(
        wchar_t* messageData
        )
    {
        delete [] messageData;
    }

}


namespace TestOliveCpp {

    int __cdecl onChangedField(
        int fieldId,
        const wchar_t* oldValue,
        const wchar_t* newValue,
        std::wstring& message
        )
    {
        const bool oldEqualsNew = std::wcscmp(oldValue, newValue) == 0;
        message = L"Les valeurs du champ ";
        message += std::to_wstring(fieldId);
        message += L" sont ";
        message += (oldEqualsNew ? L"egales" : L"differentes");
        return oldEqualsNew ? 1 : 2;
    }

}
