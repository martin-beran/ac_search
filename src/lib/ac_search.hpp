#pragma once

namespace ac {

template <class CharT> class basic_matcher {
public:
    template <class Patterns>
        explicit basic_matcher(const Patterns& /*patterns*/) {}
    template <class Callback> bool step(CharT /*c*/, const Callback& /*callback*/) {
        return false;
    }
};

using matcher = basic_matcher<char>;

}

#include "impl/ac_search_impl.hpp"
