#pragma once

namespace ac {

/*** basic_automaton::builder ************************************************/

template <class CharT> class basic_automaton<CharT>::builder {
public:
    template <class InputIt> void add(size_t idx, InputIt first, InputIt last);
    void finish(basic_automaton<CharT>& dfa);
};

template <class CharT> template <class InputIt>
void basic_automaton<CharT>::builder::add(size_t idx,
                                          InputIt first, InputIt last)
{
    // TODO
    first == last && idx == 0;
}

template <class CharT>
void basic_automaton<CharT>::builder::finish(basic_automaton& dfa)
{
    // TODO
    dfa.fgn.size();
}

/*** basic_automaton *********************************************************/

template <class CharT> template <class InputIt>
basic_automaton<CharT>::basic_automaton(InputIt first, InputIt last)
{
    builder bld;
    size_t i = 0;
    for (; first != last; ++first, ++i)
        bld.add(i, first->begin(), first->end());
    bld.finish(*this);
}

/*** basic *******************************************************************/

template <class CharT, class Callback> 
matcher<CharT, Callback>::matcher(const automaton_type& dfa, Callback callback):
    dfa(dfa), callback(callback)
{
}

template <class CharT, class Callback>
bool matcher<CharT, Callback>::step(value_type c)
{
    // TODO
    return c == 0;
}

}
