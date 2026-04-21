#include "urldecode.h"
#include <stdexcept>
#include <cctype>

std::string UrlDecode(std::string_view str) {
    std::string result;
    result.reserve(str.size());
    
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '%') {
            // Проверяем, что есть два следующих символа
            if (i + 2 >= str.size()) {
                throw std::invalid_argument("Incomplete percent sequence");
            }
            
            // Проверяем, что символы являются шестнадцатеричными цифрами
            auto c1 = str[i + 1];
            auto c2 = str[i + 2];
            
            auto hex_to_val = [](char c) -> int {
                if (c >= '0' && c <= '9') return c - '0';
                if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
                if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
                throw std::invalid_argument("Invalid hex character in percent sequence");
            };
            
            int value = hex_to_val(c1) * 16 + hex_to_val(c2);
            result.push_back(static_cast<char>(value));
            i += 2;
        } else if (str[i] == '+') {
            result.push_back(' ');
        } else {
            result.push_back(str[i]);
        }
    }
    
    return result;
}
