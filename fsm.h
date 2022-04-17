#pragma once

#include <iostream>
#include <map>
#include <set>
#include <utility>
#include <vector>
#include "ltl.h"

using namespace model::ltl;

namespace model::fsm {


class State final {
    // state of the Automaton like s1, s2, ...
public:
    explicit State(std::string label): _label(std::move(label)) {}

    bool operator ==(const State &rhs) const {
      return _label == rhs._label;
    }

    [[nodiscard]] std::string label() const { return _label; }

private:
    const std::string _label;
};


class Transition final {
    // function S x Sigma -> S, where S - set of States, Sigma - set of symbols
public:
    Transition(const State &source, std::set<std::string> symbol, const State &target):
        _source(source), _target(target), _symbol(std::move(symbol)) {}

    bool operator ==(const Transition &rhs) const {
        return _source == rhs._source
            && _symbol == rhs._symbol
            && _target == rhs._target;
  }

    [[nodiscard]] const State& source() const { return _source; }
    [[nodiscard]] const State& target() const { return _target; }
    [[nodiscard]] const std::set<std::string>& symbol() const { return _symbol; }

private:
    const State &_source;
    const State &_target;
    std::set<std::string> _symbol;
};


class Automaton final {
    // Automata <S, S0, Delta, F>
    // S and S0 - sets of states and initial states
    // Delta - set of transitiona S x Sigma -> S
    // F - set of sets of final states
    friend std::ostream& operator <<(std::ostream &out, const Automaton &automaton);

public:
    Automaton() = default;

    void add_state(const std::string &state_label);
    void set_initial(const std::string &state_label);
    void set_final(const std::string &state_label, unsigned final_set_index);

    void add_trans(
        const std::string &source,
        const std::set<std::string> &symbol,
        const std::string &target
    );

private:
    std::map<std::string, State> _states;
    std::set<std::string> _initial_states;
    std::map<unsigned, std::set<std::string>> _final_states;
    std::map<std::string, std::vector<Transition>> _transitions;
};


inline void Automaton::add_state(const std::string &state_label) {
    State state(state_label);
    _states.insert({state_label, state});
}

inline void Automaton::set_initial(const std::string &state_label) {
    _initial_states.insert(state_label);
}

inline void Automaton::set_final(const std::string &state_label, unsigned final_set_index) {
    _final_states[final_set_index].insert(state_label);
}

inline void Automaton::add_trans(
    const std::string &source,
    const std::set<std::string> &symbol,
    const std::string &target) {

    auto s = _states.find(source);
    auto t = _states.find(target);

    Transition trans(s->second, symbol, t->second);
    _transitions[source].push_back(trans);
}


std::ostream& operator <<(std::ostream &out, const State &state);
std::ostream& operator <<(std::ostream &out, const Transition &transition);
std::ostream& operator <<(std::ostream &out, const Automaton &automaton);
std::vector<const Formula> make_closure_set(const Formula &f);
std::vector<const Formula> delete_duplicates(std::vector<const Formula> &formulas);
std::map<std::string, std::map<std::string, const Formula>> make_atoms_set(std::vector<const Formula> &closure);

} // namespace model::fsm
