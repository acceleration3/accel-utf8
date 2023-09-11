#include <iostream>

#include <accel/utf8>

using namespace accel;

size_t __wcslen(const wchar_t* ws)
{
    size_t position = 0;
    size_t count = 0;
    
    while (true)
    {
        wchar_t test = ws[position];
        if (test == L'\0')
            count++;
        if (count == 2)
            break;
        position++;
    }

    return position - 2;
}

int main(int argc, char* argv[])
{
    utf8::string str;

    {
        utf8::string test = u8"こんにちは　世界！😂";
        utf8::string test2 = u8"こんにちは　世界！😂";
        utf8::string test3 = u8"こんにちは　世界！😀";

        ACC_ASSERT(test == test2);
        ACC_ASSERT(test == u8"こんにちは　世界！😂");
        ACC_ASSERT(test != test3);
        ACC_ASSERT(test != u8"こんにちは　世界！😀");

        ACC_ASSERT(test.length() == 10);
        ACC_ASSERT(test.data_size() == 31);
        ACC_ASSERT(!test.is_ascii());
        
        ACC_ASSERT(test[0] == u8"こ");
        ACC_ASSERT(test[1] == u8"ん");
        ACC_ASSERT(test[2] == u8"に");
        ACC_ASSERT(test[3] == u8"ち");
        ACC_ASSERT(test[4] == u8"は");
        ACC_ASSERT(test[5] == u8"　");
        ACC_ASSERT(test[6] == u8"世");
        ACC_ASSERT(test[7] == u8"界");
        ACC_ASSERT(test[8] == u8"！");
        ACC_ASSERT(test[9] == u8"😂");

        ACC_ASSERT(test.to_wstring() == L"こんにちは　世界！😂");
        ACC_ASSERT(test[0].to_wstring() == L"こ");

        test = test + test.substr(6, 2);

        ACC_ASSERT(test == u8"こんにちは　世界！😂世界");
    }

    {
        utf8::string test = "hello world!";
        std::string test2 = "hello world!";
        ACC_ASSERT(test == test2);

        ACC_ASSERT(test.is_ascii());

        test += test2;

        ACC_ASSERT(test == "hello world!hello world!");
    }

    {
        std::wstring wide = L"This is a wide string! 日本語　👺";
        utf8::string test(wide);

        ACC_ASSERT(test.length() == 28);
        ACC_ASSERT(test.to_wstring() == wide);
        ACC_ASSERT(!test.is_ascii());

        test += wide;

        ACC_ASSERT(test == u8"This is a wide string! 日本語　👺This is a wide string! 日本語　👺");
    }

    {
        utf8::string test = "test";
        
        ACC_ASSERT(test.is_ascii());
        ACC_ASSERT(test.to_string() == "test");
        ACC_ASSERT(test.try_convert_to_string() == "test");
        
        test += u8"あ";

        ACC_ASSERT(!test.is_ascii());

        try
        {
            test.try_convert_to_string();
            ACC_ASSERT(false);
        }
        catch(const std::exception& e) {}
    }

    std::cout << "All UTF-8 tests passed.\n";

    return 0;
}
