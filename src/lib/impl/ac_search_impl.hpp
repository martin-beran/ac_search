#pragma once

#include <limits>
#include <map>
#include <queue>
#include <set>
#include <stdexcept>

namespace ac {

/*** automaton::builder ******************************************************/

template <class CharT, class State, class Index>
class automaton<CharT, State, Index>::builder {
public:
    struct b_node {
        bool null = false;
        state_type state{};
        std::map<value_type, b_node> g{};
        const b_node* f = nullptr;
        std::set<index_type> o{};
    };
    template <class InputIt>
        void add(index_type idx, InputIt first, InputIt last);
    void finish(automaton_type& dfa);
    template <class Accu, class Func> Accu bfs(Func preorder);
    b_node state0;
    index_type n_states = 1;
};

template <class CharT, class State, class Index> template <class InputIt>
void automaton<CharT, State, Index>::builder::add(index_type idx,
                                                  InputIt first, InputIt last)
{
    b_node* state = &state0;
    for (; first != last; ++first) {
        auto added = state->g.try_emplace(*first);
        if (added.second) {
            added.first->second.state = state_type(n_states);
            if (++n_states == state_max)
                throw std::range_error("Too many states");
        }
        state = &added.first->second;
    }
    state->o.insert(idx);
}

template <class CharT, class State, class Index>
template <class Accu, class Func>
Accu automaton<CharT, State, Index>::builder::bfs(Func preorder)
{
    std::queue<b_node*> queue;
    Accu accu{};
    queue.emplace(&state0);
    while (!queue.empty()) {
        preorder(accu, *queue.front());
        for (auto& next: queue.front()->g)
            if (!next.second.null)
                queue.emplace(&next.second);
        queue.pop();
    }
    return accu;
}

template <class CharT, class State, class Index>
void automaton<CharT, State, Index>::builder::finish(automaton_type& dfa)
{
    struct sizes {
        size_t n_g;
        size_t n_o;
    };
    std::vector<b_node*> nodes(n_states);
    sizes sz = bfs<sizes>(
        [this, &nodes, &dfa](auto& sz, auto& node) {
            if (&node == &state0 || node.g.size() >= size_t(dfa.threshold)) {
                for (value_type c = std::numeric_limits<value_type>::min();;) {
                    node.g.insert({c, b_node{true}});
                    if (c < std::numeric_limits<value_type>::max())
                        ++c;
                    else
                        break;
                }
            }
            nodes[node.state] = &node;
            for (auto& edge: node.g)
                if (!edge.second.null) {
                    auto back = node.f;
                    typename decltype(b_node::g)::const_iterator f;
                    while (back &&
                           ((f = back->g.find(edge.first)) == back->g.end() ||
                            f->second.null))
                    {
                        back = back->f;
                    }
                    edge.second.f = back ? &f->second : &state0;
                    edge.second.o.insert(edge.second.f->o.begin(),
                                         edge.second.f->o.end());
                }
            sz.n_g += node.g.size();
            sz.n_o += node.o.size();
        });
    if (sz.n_g >= index_max)
        throw std::range_error("Too many edges (transitions)");
    if (sz.n_o >= index_max)
        throw std::range_error("Too many outputs");
    dfa.fgn.reserve(n_states + 1);
    dfa.fge.reserve(sz.n_g + n_states); // space also for function f
    dfa.o.reserve(sz.n_o);
    size_t i_fge = 0;
    size_t i_o = 0;
    for (auto pn: nodes) {
        dfa.fgn.push_back(automaton_type::node{i_fge, i_o});
        dfa.fge.push_back(automaton_type::edge{value_type{},
                                        pn->f ? pn->f->state : state_type{}});
        for (auto& e: pn->g)
            dfa.fge.push_back(automaton_type::edge{e.first, e.second.state});
        i_fge += 1 + pn->g.size();
        for (auto& o: pn->o)
            dfa.o.push_back(o);
        i_o += pn->o.size();
    }
    dfa.fgn.push_back(automaton_type::node{i_fge, i_o});
}

/*** automaton::edge *********************************************************/

template <class CharT, class State, class Index>
bool automaton<CharT, State, Index>::edge::operator<(value_type c) const
{
    return value < c;
}

/*** automaton ***************************************************************/

template <class CharT, class State, class Index> template <class InputIt>
automaton<CharT, State, Index>::automaton(InputIt first, InputIt last,
                                          ptrdiff_t threshold):
    threshold(threshold)
{
    builder bld;
    index_type i{};
    for (; first != last; ++first, ++i) {
        if (i == index_max)
            throw std::range_error("Pattern index too big");
        bld.add(i, first->begin(), first->end());
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
    auto begin = fge.begin() + ptrdiff_t(fgn[state].edges + 1);
    auto end = fge.begin() + ptrdiff_t(fgn[state + 1].edges);
    auto edge = end - begin == value_range ?
        begin + ptrdiff_t(c) -
            ptrdiff_t(std::numeric_limits<value_type>::min()) :
        std::lower_bound(begin, end, c);
    if (edge == end || edge->value != c || edge->next == state_type{})
        return state == state_type{} ? state_type{} : none;
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
