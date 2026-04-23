#include <catch2/catch_test_macros.hpp>
#include "../src/htmldecode.h"

TEST_CASE("Empty string") {
    CHECK(HtmlDecode("") == "");
}

TEST_CASE("String without HTML entities") {
    CHECK(HtmlDecode("Hello World!") == "Hello World!");
}

TEST_CASE("Entity &lt; with semicolon") {
    CHECK(HtmlDecode("2 &lt; 3") == "2 < 3");
}

TEST_CASE("Entity &gt; with semicolon") {
    CHECK(HtmlDecode("3 &gt; 2") == "3 > 2");
}

TEST_CASE("Entity &amp; with semicolon") {
    CHECK(HtmlDecode("Johnson&amp;Johnson") == "Johnson&Johnson");
}

TEST_CASE("Entity &apos; with semicolon") {
    CHECK(HtmlDecode("It&apos;s a test") == "It's a test");
}

TEST_CASE("Entity &quot; with semicolon") {
    CHECK(HtmlDecode("He said &quot;Hello&quot;") == "He said \"Hello\"");
}

TEST_CASE("Entities without semicolon") {
    CHECK(HtmlDecode("2 &lt 3") == "2 < 3");
}

TEST_CASE("Uppercase entities") {
    CHECK(HtmlDecode("2 &LT; 3 &GT; 2 &AMP; &APOS; &QUOT;") == "2 < 3 > 2 & ' \"");
}

TEST_CASE("Mixed case entities - should not decode") {
    CHECK(HtmlDecode("&Lt; &Gt; &AmP; &aPos; &Quot;") == "&Lt; &Gt; &AmP; &aPos; &Quot;");
}

TEST_CASE("Entities at the beginning") {
    CHECK(HtmlDecode("&lt;Hello") == "<Hello");
}

TEST_CASE("Entities at the end") {
    CHECK(HtmlDecode("Hello&gt;") == "Hello>");
}

TEST_CASE("Entities in the middle") {
    CHECK(HtmlDecode("Hello&lt;World&gt;!") == "Hello<World>!");
}

TEST_CASE("Multiple entities in a row") {
    CHECK(HtmlDecode("&lt;&gt;&amp;&apos;&quot;") == "<>&'\"");
}

TEST_CASE("Incomplete entity") {
    CHECK(HtmlDecode("Hello &am") == "Hello &am");
}

TEST_CASE("Unknown entity") {
    CHECK(HtmlDecode("Hello &unknown; World") == "Hello &unknown; World");
}

TEST_CASE("No recursive decoding") {
    CHECK(HtmlDecode("&amp;lt;") == "&lt;");
}

TEST_CASE("Complex example M&amp;M&APOSs") {
    CHECK(HtmlDecode("M&amp;M&APOSs") == "M&M's");
}

TEST_CASE("Johnson&Johnson examples") {
    CHECK(HtmlDecode("Johnson&amp;Johnson") == "Johnson&Johnson");
    CHECK(HtmlDecode("Johnson&ampJohnson") == "Johnson&Johnson");
    CHECK(HtmlDecode("Johnson&AMP;Johnson") == "Johnson&Johnson");
    CHECK(HtmlDecode("Johnson&AMPJohnson") == "Johnson&Johnson");
}

TEST_CASE("String with ampersand not forming entity") {
    CHECK(HtmlDecode("AT&T is a company") == "AT&T is a company");
}

TEST_CASE("Long string with many entities") {
    CHECK(HtmlDecode("&lt;div&gt;Hello&amp;World&lt;/div&gt;") == "<div>Hello&World</div>");
}

TEST_CASE("Entity &apos without semicolon") {
    CHECK(HtmlDecode("It&aposs a test") == "It's a test");
}

TEST_CASE("Entity &quot without semicolon") {
    CHECK(HtmlDecode("He said&quotHello&quot") == "He said\"Hello\"");
}
