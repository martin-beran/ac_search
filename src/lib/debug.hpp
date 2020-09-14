#pragma once

#include <iostream>

class DEBUG {
public:
    DEBUG() = default;
    ~DEBUG() {
        std::cerr << std::endl;
    }
    template <class T> std::ostream& operator<<(const T& v) {
        std::cerr << v;
        return std::cerr;
    }
};
