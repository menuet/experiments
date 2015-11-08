
#pragma once


#include <cstdint>
#include <string>


extern "C" { // Indique au compilateur que les fonctions suivantes sont des fonctions C, dont le nom ne doit pas etre décoré ("mangled") avec le type des parametres

    __declspec(dllexport) // Indique au compilateur de rendre cette fonction visible et appelable depuis l'extérieur de la dll
    int32_t // valeur de retour
    __cdecl // convention d'appel (== façon dont les paramètres sont posés puis enlevés de la pile) (principales conventions: __cdecl, __stdcall == PASCAL)
    TestOliveC_onChangedFieldV1(
        int32_t fieldId,
        const wchar_t* oldValue,
        const wchar_t* newValue,
        wchar_t* messageData, // Pointeur vers une zone mémoire allouée par l'appelant
        uint32_t messageMaxSize // Taille de la zone mémoire allouée
        );

    __declspec(dllexport) int __cdecl TestOliveC_onChangedFieldV2(
        int fieldId,
        const wchar_t* oldValue,
        const wchar_t* newValue,
        wchar_t** messageData, // Pointeur sur un pointeur que l'appelé remplira avec l'adresse d'une zone mémoire qu'il a lui-même allouée
        uint32_t* messageSize // Pointeur sur un entier que l'appelé remplira avec la taille de la zone mémoire allouée
        );

    __declspec(dllexport) void __cdecl TestOliveC_destroyMessageData(
        wchar_t* messageData // Pointeur sur la zone mémoire que l'appelé avait allouée pendant la fonction TestOliveC_onChangedFieldV2
        );

}


namespace TestOliveCpp {

    __declspec(dllexport) int __cdecl onChangedField(
        int fieldId,
        const wchar_t* oldValue,
        const wchar_t* newValue,
        std::wstring& message
        );

}
