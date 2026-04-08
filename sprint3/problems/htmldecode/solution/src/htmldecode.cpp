#include "htmldecode.h"
#include <string>
#include <unordered_map>
#include <cctype>

std::string HtmlDecode(std::string_view str) {
    static const std::unordered_map<std::string, char> entities = {
        {"&lt;", '<'},
        {"&gt;", '>'},
        {"&amp;", '&'},
        {"&apos;", '\''},
        {"&quot;", '"'},
        {"&LT;", '<'},
        {"&GT;", '>'},
        {"&AMP;", '&'},
        {"&APOS;", '\''},
        {"&QUOT;", '"'},
        {"&lt", '<'},
        {"&gt", '>'},
        {"&amp", '&'},
        {"&apos", '\''},
        {"&quot", '"'},
        {"&LT", '<'},
        {"&GT", '>'},
        {"&AMP", '&'},
        {"&APOS", '\''},
        {"&QUOT", '"'}
    };
    
    std::string result;
    result.reserve(str.size());
    
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '&') {
            // Ищем конец мнемоники
            size_t end = i + 1;
            while (end < str.size() && 
                   (std::isalnum(str[end]) || str[end] == '#' || str[end] == ';')) {
                ++end;
            }
            
            // Проверяем все возможные длины мнемоники
            bool found = false;
            for (size_t len = std::min<size_t>(end - i, 10); len >= 2 && !found; --len) {
                std::string candidate(str.substr(i, len));
                auto it = entities.find(candidate);
                if (it != entities.end()) {
                    result.push_back(it->second);
                    i += len - 1;
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                result.push_back('&');
            }
        } else {
            result.push_back(str[i]);
        }
    }
    
    return result;
}
