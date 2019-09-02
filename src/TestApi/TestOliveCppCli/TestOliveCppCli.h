
#pragma once


#include "../TestOliveC/TestOliveC.h"
#include <msclr\marshal.h>
#include <msclr\marshal_cppstd.h>
#include <string>


namespace TestOliveCppCli {

	public ref class TestOlive
	{
    public:

        static int onChangedField(int fieldId, System::String^ oldValue, System::String^ newValue, System::String^% message)
        {
            std::wstring wMessage;
            const auto result = TestOliveCpp::onChangedField(
                fieldId,
                msclr::interop::marshal_as<std::wstring>(oldValue).c_str(),
                msclr::interop::marshal_as<std::wstring>(newValue).c_str(),
                wMessage
                );
            message = gcnew System::String(wMessage.c_str(), 0, static_cast<int>(wMessage.length()));
            return result;
        }
	};

}
