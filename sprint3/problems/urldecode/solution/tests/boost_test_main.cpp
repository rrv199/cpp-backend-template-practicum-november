#define BOOST_TEST_MODULE UrlDecode tests
#include <boost/test/unit_test.hpp>
#include "../src/urldecode.h"

BOOST_AUTO_TEST_SUITE(UrlDecodeTestSuite)

BOOST_AUTO_TEST_CASE(EmptyStringTest) {
    BOOST_TEST(UrlDecode("") == "");
}

BOOST_AUTO_TEST_CASE(NoPercentEncodingTest) {
    BOOST_TEST(UrlDecode("Hello World!") == "Hello World!");
}

BOOST_AUTO_TEST_CASE(PlusAsSpaceTest) {
    BOOST_TEST(UrlDecode("Hello+World+!") == "Hello World !");
}

BOOST_AUTO_TEST_CASE(ValidPercentEncodingUpperTest) {
    BOOST_TEST(UrlDecode("Hello%20World%21") == "Hello World!");
}

BOOST_AUTO_TEST_CASE(ValidPercentEncodingLowerTest) {
    BOOST_TEST(UrlDecode("Hello%20World%21") == "Hello World!");
}

BOOST_AUTO_TEST_CASE(SpecialCharsTest) {
    BOOST_TEST(UrlDecode("%40%23%24%25%5E%26%2A%28%29") == "@#$%^&*()");
}

BOOST_AUTO_TEST_CASE(IncompletePercentSequenceTest) {
    BOOST_CHECK_THROW(UrlDecode("Hello%2"), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(InvalidHexCharTest) {
    BOOST_CHECK_THROW(UrlDecode("Hello%2GWorld"), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(MixedPlusAndPercentTest) {
    BOOST_TEST(UrlDecode("Hello+%20World%21+%3F") == "Hello  World! ?");
}

BOOST_AUTO_TEST_CASE(RussianLettersTest) {
    BOOST_TEST(UrlDecode("%D0%9F%D1%80%D0%B8%D0%B2%D0%B5%D1%82") == "Привет");
}

BOOST_AUTO_TEST_CASE(MultiplePercentTest) {
    BOOST_TEST(UrlDecode("%41%42%43") == "ABC");
}

BOOST_AUTO_TEST_SUITE_END()
