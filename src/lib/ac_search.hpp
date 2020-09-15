#pragma once

#include <vector>

namespace ac {

template <class CharT> class basic_automaton {
public:
    using value_type = CharT;
    template <class InputIt>
        explicit basic_automaton(InputIt first, InputIt last);
private:
    struct node {
        size_t edges;
        size_t edges_sz;
        size_t out;
        size_t out_sz;
    };
    struct edge {
        value_type value;
        size_t next;
        size_t out;
    };
    std::vector<node> fgn;
    std::vector<edge> fge;
    std::vector<size_t> o;
    class builder;
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
    size_t state = 0;
};

using automaton = basic_automaton<char>;

}

#include "impl/ac_search_impl.hpp"
