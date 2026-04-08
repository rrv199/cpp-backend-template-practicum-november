#define BOOST_TEST_MODULE UrlDecode tests
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <stdexcept>

#include "../src/urldecode.h"

BOOST_AUTO_TEST_SUITE(UrlDecodeTestSuite)

// Тест 1: Пустая строка
BOOST_AUTO_TEST_CASE(EmptyStringTest) {
    std::string input = "";
    std::string expected = "";
    BOOST_TEST(UrlDecode(input) == expected);
}

// Тест 2: Строка без %-последовательностей
BOOST_AUTO_TEST_CASE(NoPercentEncodingTest) {
    std::string input = "Hello World!";
    std::string expected = "Hello World!";
    BOOST_TEST(UrlDecode(input) == expected);
}

// Тест 3: Строка с пробелом в виде +
BOOST_AUTO_TEST_CASE(PlusAsSpaceTest) {
    std::string input = "Hello+World+!";
    std::string expected = "Hello World !";
    BOOST_TEST(UrlDecode(input) == expected);
}

// Тест 4: Валидные %-последовательности (верхний регистр)
BOOST_AUTO_TEST_CASE(ValidPercentEncodingUpperTest) {
    std::string input = "Hello%20World%21";
    std::string expected = "Hello World!";
    BOOST_TEST(UrlDecode(input) == expected);
}

// Тест 5: Валидные %-последовательности (нижний регистр)
BOOST_AUTO_TEST_CASE(ValidPercentEncodingLowerTest) {
    std::string input = "Hello%20World%21";
    std::string expected = "Hello World!";
    BOOST_TEST(UrlDecode(input) == expected);
}

// Тест 6: Смешанный регистр
BOOST_AUTO_TEST_CASE(MixedCasePercentEncodingTest) {
    std::string input = "Hello%20World%21";
    std::string expected = "Hello World!";
    BOOST_TEST(UrlDecode(input) == expected);
}

// Тест 7: Кодирование специальных символов
BOOST_AUTO_TEST_CASE(SpecialCharsTest) {
    std::string input = "%40%23%24%25%5E%26%2A%28%29";
    std::string expected = "@#$%^&*()";
    BOOST_TEST(UrlDecode(input) == expected);
}

// Тест 8: Строка с зарезервированными символами
BOOST_AUTO_TEST_CASE(ReservedCharsTest) {
    std::string input = "/?&=";
    std::string expected = "/?&=";
    BOOST_TEST(UrlDecode(input) == expected);
}

// Тест 9: Неполная %-последовательность в конце
BOOST_AUTO_TEST_CASE(IncompletePercentSequenceTest) {
    std::string input = "Hello%2";
    BOOST_CHECK_THROW(UrlDecode(input), std::invalid_argument);
}

// Тест 10: Неполная %-последовательность в середине
BOOST_AUTO_TEST_CASE(IncompletePercentSequenceMiddleTest) {
    std::string input = "Hello%2World";
    BOOST_CHECK_THROW(UrlDecode(input), std::invalid_argument);
}

// Тест 11: Невалидный символ в %-последовательности
BOOST_AUTO_TEST_CASE(InvalidHexCharTest) {
    std::string input = "Hello%2GWorld";
    BOOST_CHECK_THROW(UrlDecode(input), std::invalid_argument);
}

// Тест 12: Пустая %-последовательность
BOOST_AUTO_TEST_CASE(EmptyPercentSequenceTest) {
    std::string input = "Hello%%World";
    BOOST_CHECK_THROW(UrlDecode(input), std::invalid_argument);
}

// Тест 13: Строка с несколькими + и %-последовательностями
BOOST_AUTO_TEST_CASE(MixedPlusAndPercentTest) {
    std::string input = "Hello+%20World%21+%3F";
    std::string expected = "Hello  World! ?";
    BOOST_TEST(UrlDecode(input) == expected);
}

// Тест 14: Кодировка символа '~' (не должен кодироваться)
BOOST_AUTO_TEST_CASE(TildeNoEncodingTest) {
    std::string input = "Hello~World";
    std::string expected = "Hello~World";
    BOOST_TEST(UrlDecode(input) == expected);
}

// Тест 15: Кодировка символа '.' (не должен кодироваться)
BOOST_AUTO_TEST_CASE(DotNoEncodingTest) {
    std::string input = "file.name.txt";
    std::string expected = "file.name.txt";
    BOOST_TEST(UrlDecode(input) == expected);
}

// Тест 16: Кодировка символа '_' (не должен кодироваться)
BOOST_AUTO_TEST_CASE(UnderscoreNoEncodingTest) {
    std::string input = "my_file_name";
    std::string expected = "my_file_name";
    BOOST_TEST(UrlDecode(input) == expected);
}

// Тест 17: Кодировка символа '-' (не должен кодироваться)
BOOST_AUTO_TEST_CASE(HyphenNoEncodingTest) {
    std::string input = "my-file-name";
    std::string expected = "my-file-name";
    BOOST_TEST(UrlDecode(input) == expected);
}

// Тест 18: Кодировка русских букв
BOOST_AUTO_TEST_CASE(RussianLettersTest) {
    std::string input = "%D0%9F%D1%80%D0%B8%D0%B2%D0%B5%D1%82";
    std::string expected = "Привет";
    BOOST_TEST(UrlDecode(input) == expected);
}

// Тест 19: Кодировка нескольких символов подряд
BOOST_AUTO_TEST_CASE(MultiplePercentTest) {
    std::string input = "%41%42%43";
    std::string expected = "ABC";
    BOOST_TEST(UrlDecode(input) == expected);
}

// Тест 20: Строка с символом + и %-последовательностью
BOOST_AUTO_TEST_CASE(PlusAndPercentTest) {
    std::string input = "Hello+%20World";
    std::string expected = "Hello  World";
    BOOST_TEST(UrlDecode(input) == expected);
}

BOOST_AUTO_TEST_SUITE_END()
