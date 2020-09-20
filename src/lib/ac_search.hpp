#pragma once

#include <limits>
#include <vector>

namespace ac {

template <class DFA, class Callback> class matcher;

template <class CharT = char, class State = unsigned, class Index = size_t>
class automaton {
public:
    using value_type = CharT;
    using state_type = State;
    using index_type = Index;
    using automaton_type = automaton<value_type, state_type, index_type>;
    template <class Callback>
        using matcher_type = matcher<automaton_type, Callback>;
    template <class InputIt>
        explicit automaton(InputIt first, InputIt last);
private:
    static constexpr state_type none = std::numeric_limits<state_type>::max();
    static constexpr state_type state_max = none - 1;
    static constexpr index_type index_max =
        std::numeric_limits<index_type>::max();
    struct node {
        index_type edges;
        index_type out;
    };
    struct edge {
        value_type value;
        state_type next;
        bool operator<(value_type c) const;
    };
    class builder;
    state_type f(state_type state) const;
    state_type g(state_type state, value_type c) const;
    std::vector<node> fgn;
    std::vector<edge> fge;
    std::vector<index_type> o;
    template <class DFA, class Callback> friend class matcher;
};

template <class DFA, class Callback> class matcher {
public:
    using automaton_type = DFA;
    using value_type = typename automaton_type::value_type;
    using state_type = typename automaton_type::state_type;
    using index_type = typename automaton_type::index_type;
    matcher(const automaton_type& dfa, Callback callback);
    bool step(value_type c);
private:
    const automaton_type& dfa;
    Callback callback;
    state_type state{};
};

}

#include "impl/ac_search_impl.hpp"
