#pragma once

#include <stdexcept>

namespace ac {

/*** automaton::builder ******************************************************/

template <class CharT, class State, class Index>
class automaton<CharT, State, Index>::builder {
public:
    template <class InputIt>
        void add(index_type idx, InputIt first, InputIt last);
    void finish(automaton_type& dfa);
};

template <class CharT, class State, class Index> template <class InputIt>
void automaton<CharT, State, Index>::builder::add(index_type idx,
                                                  InputIt first, InputIt last)
{
    // TODO
    first == last && idx == 0;
}

template <class CharT, class State, class Index>
void automaton<CharT, State, Index>::builder::finish(automaton_type& dfa)
{
    // TODO
    dfa.fgn.size();
}

/*** automaton::edge *********************************************************/

template <class CharT, class State, class Index>
bool automaton<CharT, State, Index>::edge::operator<(value_type c) const
{
    return value < c;
}

/*** automaton ***************************************************************/

template <class CharT, class State, class Index> template <class InputIt>
automaton<CharT, State, Index>::automaton(InputIt first, InputIt last)
{
    builder bld;
    index_type i = 0;
    for (; first != last; ++first, ++i) {
        bld.add(i, first->begin(), first->end());
        if (i == index_max)
            throw std::range_error("Pattern index too big");
    }
    bld.finish(*this);
}

template <class CharT, class State, class Index>
auto automaton<CharT, State, Index>::f(state_type state) const -> state_type
{
    return fge[fgn[state].edges].next;
}

template <class CharT, class State, class Index>
auto automaton<CharT, State, Index>::g(state_type state, value_type c) const ->
    state_type
{
    auto end = fge.begin() + fgn[state + 1].edges;
    auto edge = std::lower_bound(fge.begin() + fgn[state].edges + 1, end, c);
    if (edge == end || edge->value != c)
        return state == 0 ? 0 : none;
    else
        return edge->next;
}

/*** matcher *****************************************************************/

template <class DFA, class Callback> 
matcher<DFA, Callback>::matcher(const automaton_type& dfa, Callback callback):
    dfa(dfa), callback(callback)
{
}

template <class DFA, class Callback>
bool matcher<DFA, Callback>::step(value_type c)
{
    state_type new_state;
    while ((new_state = dfa.g(state, c)) == automaton_type::none)
        state = dfa.f(state);
    state = new_state;
    bool found = false;
    for (index_type oi = dfa.fgn[state].out; oi < dfa.fgn[state + 1].out; ++oi)
    {
        found = true;
        callback(dfa.o[oi]);
    }
    return found;

}

}
