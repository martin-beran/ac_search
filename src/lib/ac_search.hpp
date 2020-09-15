#pragma once

namespace ac {

template <class CharT> class basic_automaton {
public:
    using value_type = CharT;
    template <class InputIt>
        explicit basic_automaton(InputIt first, InputIt last);
private:
    class builder {
    public:
    };
};

template <class CharT, class Callback> class matcher {
public:
    using value_type = CharT;
    using automaton_type = basic_automaton<value_type>;
    matcher(const automaton_type& dfa, Callback callback);
    bool step(value_type c);
private:
    const automaton_type& dfa;
    Callback callback;
};

using automaton = basic_automaton<char>;

}

#include "impl/ac_search_impl.hpp"
