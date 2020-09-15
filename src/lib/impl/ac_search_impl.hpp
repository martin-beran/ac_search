#pragma once

namespace ac {

/*** basic_automaton *********************************************************/

template <class CharT> template <class InputIt>
basic_automaton<CharT>::basic_automaton(InputIt first, InputIt last)
{
    for (; first != last; ++first)
        ;
}

/*** basic_automaton::builder ************************************************/

/*** basic *******************************************************************/

template <class CharT, class Callback> 
matcher<CharT, Callback>::matcher(const automaton_type& dfa, Callback callback):
    dfa(dfa), callback(callback)
{
}

template <class CharT, class Callback>
bool matcher<CharT, Callback>::step(value_type c)
{
    return c == 0;
}

}
