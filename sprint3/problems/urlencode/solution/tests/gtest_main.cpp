#include <gtest/gtest.h>
#include "../src/urlencode.h"

// Тест 1: Пустая строка
TEST(UrlEncodeTestSuite, EmptyString) {
    std::string input = "";
    std::string expected = "";
    EXPECT_EQ(UrlEncode(input), expected);
}

// Тест 2: Строка без служебных символов
TEST(UrlEncodeTestSuite, NoSpecialChars) {
    std::string input = "HelloWorld123";
    std::string expected = "HelloWorld123";
    EXPECT_EQ(UrlEncode(input), expected);
}

// Тест 3: Строка с буквами в разных регистрах
TEST(UrlEncodeTestSuite, MixedCaseLetters) {
    std::string input = "HelloWorldABCabc";
    std::string expected = "HelloWorldABCabc";
    EXPECT_EQ(UrlEncode(input), expected);
}

// Тест 4: Строка с цифрами
TEST(UrlEncodeTestSuite, Digits) {
    std::string input = "0123456789";
    std::string expected = "0123456789";
    EXPECT_EQ(UrlEncode(input), expected);
}

// Тест 5: Строка с пробелами (должны стать +)
TEST(UrlEncodeTestSuite, Spaces) {
    std::string input = "Hello World";
    std::string expected = "Hello+World";
    EXPECT_EQ(UrlEncode(input), expected);
}

// Тест 6: Строка с несколькими пробелами подряд
TEST(UrlEncodeTestSuite, MultipleSpaces) {
    std::string input = "Hello   World";
    std::string expected = "Hello+++World";
    EXPECT_EQ(UrlEncode(input), expected);
}

// Тест 7: Строка с зарезервированными символами
TEST(UrlEncodeTestSuite, ReservedChars) {
    std::string input = "! # $ & ' ( ) * + , / : ; = ? @ [ ]";
    // Пробелы станут +, остальные символы кодируются
    std::string expected = "%21+%23+%24+%26+%27+%28+%29+%2A+%2B+%2C+%2F+%3A+%3B+%3D+%3F+%40+%5B+%5D";
    EXPECT_EQ(UrlEncode(input), expected);
}

// Тест 8: Символ '*' отдельно
TEST(UrlEncodeTestSuite, Asterisk) {
    std::string input = "abc*def";
    std::string expected1 = "abc%2Adef";
    std::string expected2 = "abc%2adef";
    std::string result = UrlEncode(input);
    // Допускается как верхний, так и нижний регистр
    EXPECT_TRUE(result == expected1 || result == expected2);
}

// Тест 9: Символ '~' (не должен кодироваться)
TEST(UrlEncodeTestSuite, Tilde) {
    std::string input = "Hello~World";
    std::string expected = "Hello~World";
    EXPECT_EQ(UrlEncode(input), expected);
}

// Тест 10: Символ '.' (не должен кодироваться)
TEST(UrlEncodeTestSuite, Dot) {
    std::string input = "file.name.txt";
    std::string expected = "file.name.txt";
    EXPECT_EQ(UrlEncode(input), expected);
}

// Тест 11: Символ '_' (не должен кодироваться)
TEST(UrlEncodeTestSuite, Underscore) {
    std::string input = "my_file_name";
    std::string expected = "my_file_name";
    EXPECT_EQ(UrlEncode(input), expected);
}

// Тест 12: Символ '-' (не должен кодироваться)
TEST(UrlEncodeTestSuite, Hyphen) {
    std::string input = "my-file-name";
    std::string expected = "my-file-name";
    EXPECT_EQ(UrlEncode(input), expected);
}

// Тест 13: Символы с кодами меньше 32
TEST(UrlEncodeTestSuite, ControlChars) {
    std::string input;
    input.push_back(0x00);  // NULL
    input.push_back(0x01);  // SOH
    input.push_back(0x02);  // STX
    input.push_back(0x1F);  // US
    
    std::string expected = "%00%01%02%1F";
    EXPECT_EQ(UrlEncode(input), expected);
}

// Тест 14: Символы с кодами >= 128 (UTF-8)
TEST(UrlEncodeTestSuite, Utf8Chars) {
    std::string input = "Привет";
    // Русские буквы в UTF-8 занимают 2 байта
    std::string expected = "%D0%9F%D1%80%D0%B8%D0%B2%D0%B5%D1%82";
    EXPECT_EQ(UrlEncode(input), expected);
}

// Тест 15: Смешанная строка
TEST(UrlEncodeTestSuite, MixedString) {
    std::string input = "Hello World! Привет!";
    // Пробелы -> +, ! -> %21, русские буквы кодируются
    std::string expected = "Hello+World%21+%D0%9F%D1%80%D0%B8%D0%B2%D0%B5%D1%82%21";
    EXPECT_EQ(UrlEncode(input), expected);
}

// Тест 16: Строка с символом табуляции (код 9)
TEST(UrlEncodeTestSuite, TabChar) {
    std::string input = "Hello\tWorld";
    std::string expected = "Hello%09World";
    EXPECT_EQ(UrlEncode(input), expected);
}

// Тест 17: Строка с символом новой строки (код 10)
TEST(UrlEncodeTestSuite, NewlineChar) {
    std::string input = "Hello\nWorld";
    std::string expected = "Hello%0AWorld";
    EXPECT_EQ(UrlEncode(input), expected);
}

// Тест 18: Строка с символом возврата каретки (код 13)
TEST(UrlEncodeTestSuite, CarriageReturnChar) {
    std::string input = "Hello\rWorld";
    std::string expected = "Hello%0DWorld";
    EXPECT_EQ(UrlEncode(input), expected);
}

// Тест 19: Строка с символом '%' (должен кодироваться)
TEST(UrlEncodeTestSuite, PercentChar) {
    std::string input = "100%";
    std::string expected = "100%25";
    EXPECT_EQ(UrlEncode(input), expected);
}

// Тест 20: Строка с символом '&'
TEST(UrlEncodeTestSuite, AmpersandChar) {
    std::string input = "a&b";
    std::string expected = "a%26b";
    EXPECT_EQ(UrlEncode(input), expected);
}

// Тест 21: Строка с символом '='
TEST(UrlEncodeTestSuite, EqualsChar) {
    std::string input = "a=b";
    std::string expected = "a%3Db";
    EXPECT_EQ(UrlEncode(input), expected);
}

// Тест 22: Строка с символом '?'
TEST(UrlEncodeTestSuite, QuestionMarkChar) {
    std::string input = "a?b";
    std::string expected = "a%3Fb";
    EXPECT_EQ(UrlEncode(input), expected);
}

// Тест 23: Строка с символом '@'
TEST(UrlEncodeTestSuite, AtChar) {
    std::string input = "a@b";
    std::string expected = "a%40b";
    EXPECT_EQ(UrlEncode(input), expected);
}

// Тест 24: Строка с символом '#'
TEST(UrlEncodeTestSuite, HashChar) {
    std::string input = "a#b";
    std::string expected = "a%23b";
    EXPECT_EQ(UrlEncode(input), expected);
}

// Тест 25: Строка с символом '+'
TEST(UrlEncodeTestSuite, PlusChar) {
    std::string input = "a+b";
    std::string expected = "a%2Bb";
    EXPECT_EQ(UrlEncode(input), expected);
}
