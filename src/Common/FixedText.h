#pragma once

#include <cstring>
#include <string>

inline void copyText(char* destination, std::size_t destinationSize, const std::string& source) {
    if (destinationSize == 0) {
        return;
    }

    std::strncpy(destination, source.c_str(), destinationSize - 1);
    destination[destinationSize - 1] = '\0';
}

inline std::string toString(const char* text) {
    return text == nullptr ? std::string() : std::string(text);
}
