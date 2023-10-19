#include "accel/macros"
#include <iostream>
#include <vector>

#include <accel/utf8>

using namespace accel;

int main(int argc, char* argv[])
{
    utf8::string str;

    {
        utf8::string test = u8"こんにちは　世界！😂";
        utf8::string test2 = u8"こんにちは　世界！😂";
        utf8::string test3 = u8"こんにちは　世界！😀";

        ACC_ASSERT(test.find(u8"こん") == 0);
        ACC_ASSERT(test.find(u8"ちは") == 3);
        ACC_ASSERT(test.find(u8"世界！😂") == 6);
        ACC_ASSERT(test.find(u8"は", 2) == 4);
        ACC_ASSERT(test.find(u8"😂", 4) == 9);

        ACC_ASSERT_EXCEPTION(test.find(u8"a", 10), std::out_of_range);
        ACC_ASSERT_EXCEPTION(test.find(u8"ab", 9), std::out_of_range);

        ACC_ASSERT(test.find(u8"test", 2) == utf8::string::npos);
        ACC_ASSERT(test.find(u8"こん", 2) == utf8::string::npos);

        ACC_ASSERT(test.find_first_of(u8"aちd") == 3);
        ACC_ASSERT(test.find_first_of(u8"aちdこ") == 0);
        ACC_ASSERT(test.find_first_of(u8"aちd", 2) == 3);
        ACC_ASSERT(test.find_first_of(u8"asgs世ad", 4) == 6);

        ACC_ASSERT(test.find_first_of(u8"abcd") == utf8::string::npos);
        ACC_ASSERT(test.find_first_of(u8"aちd", 4) == utf8::string::npos);

        ACC_ASSERT_EXCEPTION(test.find_first_of(u8"asdaa", 10), std::out_of_range);
        ACC_ASSERT_EXCEPTION(test.find_first_of(u8"asdaa", 10), std::out_of_range);
        
        ACC_ASSERT(test.find_first_not_of(u8"こんにちは") == 5);
        ACC_ASSERT(test.find_first_not_of(test) == utf8::string::npos);
        ACC_ASSERT_EXCEPTION(test.find_first_not_of(u8"a", 10), std::out_of_range);
        
        utf8::string test4 = u8"かきくけこかきくけこ";

        {
            auto temp = test4;
            temp.replace(0, 2, u8"くけ");
            ACC_ASSERT(temp == u8"くけくけこかきくけこ");
            temp.replace(5, 3, u8"く");
            ACC_ASSERT(temp == u8"くけくけこくけこ");
            temp.replace(3, 1, u8"くくくくくくく");
            ACC_ASSERT(temp == u8"くけくくくくくくくくこくけこ");
        }

        ACC_ASSERT(test4.find_last_of(u8"きくけ") == 8);
        ACC_ASSERT(test4.find_last_of(u8"こきくけ") == 9);
        ACC_ASSERT(test4.find_last_of(u8"こきくけ", 4) == 4);
        
        ACC_ASSERT(test4.find_last_of(u8"abcd") == utf8::string::npos);
        
        ACC_ASSERT_EXCEPTION(test4.find_last_of(u8"きくけ", 10), std::out_of_range);

        ACC_ASSERT(test4.find_last_not_of(u8"かきくけ") == 9);
        ACC_ASSERT(test4.find_last_not_of(u8"かきくけ", 8) == 4);
        ACC_ASSERT(test4.find_last_not_of(test4) == utf8::string::npos);
        ACC_ASSERT_EXCEPTION(test4.find_last_not_of(u8"a", 10), std::out_of_range);

        ACC_ASSERT(test.substr(2) == u8"にちは　世界！😂");
        ACC_ASSERT(test.substr(4) == u8"は　世界！😂");
        ACC_ASSERT(test.substr(8) == u8"！😂");

        ACC_ASSERT(test.substr(2, 2) == u8"にち");
        ACC_ASSERT(test.substr(4, 3) == u8"は　世");
        ACC_ASSERT(test.substr(8, 1) == u8"！");

        ACC_ASSERT(test == test2);
        ACC_ASSERT(test == u8"こんにちは　世界！😂");
        ACC_ASSERT(test != test3);
        ACC_ASSERT(test != u8"こんにちは　世界！😀");

        ACC_ASSERT(test.get_length() == 10);
        ACC_ASSERT(test.get_data_size() == 31);
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
        ACC_ASSERT(test.find_last_of("h", 2) == 0);
    }

    {
        std::wstring wide = L"This is a wide string! 日本語　👺";
        utf8::string test(wide);

        ACC_ASSERT(test.get_length() == 28);
        ACC_ASSERT(test.to_wstring() == wide);
        ACC_ASSERT(!test.is_ascii());

        test += wide;

        ACC_ASSERT(test == u8"This is a wide string! 日本語　👺This is a wide string! 日本語　👺");
    }

    {
        utf8::string test = "test";

        ACC_ASSERT(test.is_ascii());
        ACC_ASSERT(test.to_string() == "test");

        test += u8"あ";

        ACC_ASSERT(!test.is_ascii());

        auto it = test.begin();

        test = test + "a" + u8"a" + L"a";
        ACC_ASSERT(test == u8"testあaaa");
    }

    std::cout << "All UTF-8 tests passed.\n";

    return 0;
}
