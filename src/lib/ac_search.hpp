#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

namespace ac {

template <class DFA, class Callback> class matcher;

template <class CharT = char, class State = unsigned, class Index = uint32_t>
class automaton {
public:
    using value_type = CharT;
    using state_type = State;
    using index_type = Index;
    using automaton_type = automaton<value_type, state_type, index_type>;
    static constexpr ptrdiff_t value_range = 1 +
        ptrdiff_t(std::numeric_limits<value_type>::max()) -
        ptrdiff_t(std::numeric_limits<value_type>::min());
    static constexpr ptrdiff_t threshold_default = value_range / 4;
    template <class Callback>
        using matcher_type = matcher<automaton_type, Callback>;
    template <class InputIt>
        explicit automaton(InputIt first, InputIt last,
                           ptrdiff_t threshold = threshold_default);
    size_t size() const {
        return size_bytes(fgn) + size_bytes(fge) + size_bytes(o);
    }
    size_t nodes() const {
        return fgn.size();
    }
    size_t full_size() const {
        return n_full_size;
    }
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
    template <class T> static size_t size_bytes(const T& v) {
        return v.size() * sizeof(typename T::value_type);
    }
    std::vector<node> fgn;
    std::vector<edge> fge;
    std::vector<index_type> o;
    size_t n_full_size = 0;
    ptrdiff_t threshold;
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
