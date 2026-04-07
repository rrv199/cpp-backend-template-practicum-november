#include <catch2/catch_test_macros.hpp>
#include "../src/htmldecode.h"

// Тест 1: Пустая строка
TEST_CASE("Empty string") {
    std::string input = "";
    std::string expected = "";
    CHECK(HtmlDecode(input) == expected);
}

// Тест 2: Строка без HTML-мнемоник
TEST_CASE("String without HTML entities") {
    std::string input = "Hello World!";
    std::string expected = "Hello World!";
    CHECK(HtmlDecode(input) == expected);
}

// Тест 3: Строка с мнемоникой &lt; (нижний регистр с ;)
TEST_CASE("Entity &lt; with semicolon") {
    std::string input = "2 &lt; 3";
    std::string expected = "2 < 3";
    CHECK(HtmlDecode(input) == expected);
}

// Тест 4: Строка с мнемоникой &gt; (нижний регистр с ;)
TEST_CASE("Entity &gt; with semicolon") {
    std::string input = "3 &gt; 2";
    std::string expected = "3 > 2";
    CHECK(HtmlDecode(input) == expected);
}

// Тест 5: Строка с мнемоникой &amp; (нижний регистр с ;)
TEST_CASE("Entity &amp; with semicolon") {
    std::string input = "Johnson&amp;Johnson";
    std::string expected = "Johnson&Johnson";
    CHECK(HtmlDecode(input) == expected);
}

// Тест 6: Строка с мнемоникой &apos; (нижний регистр с ;)
TEST_CASE("Entity &apos; with semicolon") {
    std::string input = "It&apos;s a test";
    std::string expected = "It's a test";
    CHECK(HtmlDecode(input) == expected);
}

// Тест 7: Строка с мнемоникой &quot; (нижний регистр с ;)
TEST_CASE("Entity &quot; with semicolon") {
    std::string input = "He said &quot;Hello&quot;";
    std::string expected = "He said \"Hello\"";
    CHECK(HtmlDecode(input) == expected);
}

// Тест 8: Мнемоники без точки с запятой
TEST_CASE("Entities without semicolon") {
    std::string input = "2 &lt 3";
    std::string expected = "2 < 3";
    CHECK(HtmlDecode(input) == expected);
}

// Тест 9: Мнемоники в верхнем регистре
TEST_CASE("Uppercase entities") {
    std::string input = "2 &LT; 3 &GT; 2 &AMP; &APOS; &QUOT;";
    std::string expected = "2 < 3 > 2 & ' \"";
    CHECK(HtmlDecode(input) == expected);
}

// Тест 10: Смешанный регистр (не должны декодироваться)
TEST_CASE("Mixed case entities - should not decode") {
    std::string input = "&Lt; &Gt; &AmP; &aPos; &Quot;";
    std::string expected = "&Lt; &Gt; &AmP; &aPos; &Quot;";
    CHECK(HtmlDecode(input) == expected);
}

// Тест 11: Мнемоники в начале строки
TEST_CASE("Entities at the beginning") {
    std::string input = "&lt;Hello";
    std::string expected = "<Hello";
    CHECK(HtmlDecode(input) == expected);
}

// Тест 12: Мнемоники в конце строки
TEST_CASE("Entities at the end") {
    std::string input = "Hello&gt;";
    std::string expected = "Hello>";
    CHECK(HtmlDecode(input) == expected);
}

// Тест 13: Мнемоники в середине строки
TEST_CASE("Entities in the middle") {
    std::string input = "Hello&lt;World&gt;!";
    std::string expected = "Hello<World>!";
    CHECK(HtmlDecode(input) == expected);
}

// Тест 14: Несколько мнемоник подряд
TEST_CASE("Multiple entities in a row") {
    std::string input = "&lt;&gt;&amp;&apos;&quot;";
    std::string expected = "<>&'\"";
    CHECK(HtmlDecode(input) == expected);
}

// Тест 15: Неполная мнемоника
TEST_CASE("Incomplete entity") {
    std::string input = "Hello &am";
    std::string expected = "Hello &am";
    CHECK(HtmlDecode(input) == expected);
}

// Тест 16: Несуществующая мнемоника
TEST_CASE("Unknown entity") {
    std::string input = "Hello &unknown; World";
    std::string expected = "Hello &unknown; World";
    CHECK(HtmlDecode(input) == expected);
}

// Тест 17: Комбинация существующих и несуществующих мнемоник
TEST_CASE("Mix of valid and invalid entities") {
    std::string input = "&lt;hello&gt;&unknown;&amp;";
    std::string expected = "<hello>&unknown;&";
    CHECK(HtmlDecode(input) == expected);
}

// Тест 18: Декодированные символы не участвуют в повторном декодировании
TEST_CASE("No recursive decoding") {
    std::string input = "&amp;lt;";
    std::string expected = "&lt;";
    CHECK(HtmlDecode(input) == expected);
}

// Тест 19: Сложный пример из условия
TEST_CASE("Complex example M&amp;M&APOSs") {
    std::string input = "M&amp;M&APOSs";
    std::string expected = "M&M's";
    CHECK(HtmlDecode(input) == expected);
}

// Тест 20: Пример Johnson&Johnson
TEST_CASE("Johnson&Johnson examples") {
    CHECK(HtmlDecode("Johnson&amp;Johnson") == "Johnson&Johnson");
    CHECK(HtmlDecode("Johnson&ampJohnson") == "Johnson&Johnson");
    CHECK(HtmlDecode("Johnson&AMP;Johnson") == "Johnson&Johnson");
    CHECK(HtmlDecode("Johnson&AMPJohnson") == "Johnson&Johnson");
}

// Тест 21: Строка с символами, похожими на мнемоники
TEST_CASE("String with ampersand not forming entity") {
    std::string input = "AT&T is a company";
    std::string expected = "AT&T is a company";
    CHECK(HtmlDecode(input) == expected);
}

// Тест 22: Мнемоника с цифрами
TEST_CASE("Entity with numbers") {
    std::string input = "&#123; not supported";
    std::string expected = "&#123; not supported";
    CHECK(HtmlDecode(input) == expected);
}

// Тест 23: Длинная строка с множеством мнемоник
TEST_CASE("Long string with many entities") {
    std::string input = "&lt;div&gt;Hello&amp;World&lt;/div&gt;";
    std::string expected = "<div>Hello&World</div>";
    CHECK(HtmlDecode(input) == expected);
}

// Тест 24: Мнемоника &apos без точки с запятой
TEST_CASE("Entity &apos without semicolon") {
    std::string input = "It&aposs a test";
    std::string expected = "It's a test";
    CHECK(HtmlDecode(input) == expected);
}

// Тест 25: Мнемоника &quot без точки с запятой
TEST_CASE("Entity &quot without semicolon") {
    std::string input = "He said&quotHello&quot";
    std::string expected = "He said\"Hello\"";
    CHECK(HtmlDecode(input) == expected);
}
