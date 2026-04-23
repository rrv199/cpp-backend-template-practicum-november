#include "urlencode.h"
#include <sstream>
#include <iomanip>
#include <cctype>

bool IsUnsafe(char c) {
    const std::string reserved = "!#$&'()*+,/:;=?@[]%";
    
    if (static_cast<unsigned char>(c) < 32 || static_cast<unsigned char>(c) >= 128) {
        return true;
    }
    
    if (reserved.find(c) != std::string::npos) {
        return true;
    }
    
    return false;
}

std::string UrlEncode(std::string_view str) {
    std::ostringstream result;
    result << std::hex << std::uppercase;
    
    for (char c : str) {
        if (c == ' ') {
            result << '+';
        } else if (IsUnsafe(c)) {
            result << '%' << std::setw(2) << std::setfill('0') 
                   << static_cast<int>(static_cast<unsigned char>(c));
        } else {
            result << c;
        }
    }
    
    return result.str();
}
