#include <gtest/gtest.h>
#include "../src/urlencode.h"

TEST(UrlEncodeTestSuite, EmptyString) {
    EXPECT_EQ(UrlEncode(""), "");
}

TEST(UrlEncodeTestSuite, NoSpecialChars) {
    EXPECT_EQ(UrlEncode("HelloWorld123"), "HelloWorld123");
}

TEST(UrlEncodeTestSuite, MixedCaseLetters) {
    EXPECT_EQ(UrlEncode("HelloWorldABCabc"), "HelloWorldABCabc");
}

TEST(UrlEncodeTestSuite, Digits) {
    EXPECT_EQ(UrlEncode("0123456789"), "0123456789");
}

TEST(UrlEncodeTestSuite, Spaces) {
    EXPECT_EQ(UrlEncode("Hello World"), "Hello+World");
}

TEST(UrlEncodeTestSuite, MultipleSpaces) {
    EXPECT_EQ(UrlEncode("Hello   World"), "Hello+++World");
}

TEST(UrlEncodeTestSuite, ReservedChars) {
    std::string input = "! # $ & ' ( ) * + , / : ; = ? @ [ ]";
    std::string expected = "%21+%23+%24+%26+%27+%28+%29+%2A+%2B+%2C+%2F+%3A+%3B+%3D+%3F+%40+%5B+%5D";
    EXPECT_EQ(UrlEncode(input), expected);
}

TEST(UrlEncodeTestSuite, Asterisk) {
    std::string result = UrlEncode("abc*def");
    EXPECT_TRUE(result == "abc%2Adef" || result == "abc%2adef");
}

TEST(UrlEncodeTestSuite, Tilde) {
    EXPECT_EQ(UrlEncode("Hello~World"), "Hello~World");
}

TEST(UrlEncodeTestSuite, Dot) {
    EXPECT_EQ(UrlEncode("file.name.txt"), "file.name.txt");
}

TEST(UrlEncodeTestSuite, Underscore) {
    EXPECT_EQ(UrlEncode("my_file_name"), "my_file_name");
}

TEST(UrlEncodeTestSuite, Hyphen) {
    EXPECT_EQ(UrlEncode("my-file-name"), "my-file-name");
}

TEST(UrlEncodeTestSuite, ControlChars) {
    std::string input;
    input.push_back(0x00);
    input.push_back(0x01);
    input.push_back(0x02);
    input.push_back(0x1F);
    
    EXPECT_EQ(UrlEncode(input), "%00%01%02%1F");
}

TEST(UrlEncodeTestSuite, Utf8Chars) {
    EXPECT_EQ(UrlEncode("Привет"), "%D0%9F%D1%80%D0%B8%D0%B2%D0%B5%D1%82");
}

TEST(UrlEncodeTestSuite, MixedString) {
    EXPECT_EQ(UrlEncode("Hello World! Привет!"), "Hello+World%21+%D0%9F%D1%80%D0%B8%D0%B2%D0%B5%D1%82%21");
}

TEST(UrlEncodeTestSuite, TabChar) {
    EXPECT_EQ(UrlEncode("Hello\tWorld"), "Hello%09World");
}

TEST(UrlEncodeTestSuite, NewlineChar) {
    EXPECT_EQ(UrlEncode("Hello\nWorld"), "Hello%0AWorld");
}

TEST(UrlEncodeTestSuite, CarriageReturnChar) {
    EXPECT_EQ(UrlEncode("Hello\rWorld"), "Hello%0DWorld");
}

TEST(UrlEncodeTestSuite, PercentChar) {
    EXPECT_EQ(UrlEncode("100%"), "100%25");
}

TEST(UrlEncodeTestSuite, AmpersandChar) {
    EXPECT_EQ(UrlEncode("a&b"), "a%26b");
}

TEST(UrlEncodeTestSuite, EqualsChar) {
    EXPECT_EQ(UrlEncode("a=b"), "a%3Db");
}

TEST(UrlEncodeTestSuite, QuestionMarkChar) {
    EXPECT_EQ(UrlEncode("a?b"), "a%3Fb");
}

TEST(UrlEncodeTestSuite, AtChar) {
    EXPECT_EQ(UrlEncode("a@b"), "a%40b");
}

TEST(UrlEncodeTestSuite, HashChar) {
    EXPECT_EQ(UrlEncode("a#b"), "a%23b");
}

TEST(UrlEncodeTestSuite, PlusChar) {
    EXPECT_EQ(UrlEncode("a+b"), "a%2Bb");
}
