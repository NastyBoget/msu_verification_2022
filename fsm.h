#pragma once

#include <iostream>
#include <map>
#include <set>
#include <utility>
#include <vector>

namespace model::fsm {

class State final {
public:
  State(std::string label): _label(std::move(label)) {}

  bool operator ==(const State &rhs) const {
    return _label == rhs._label;
  }

  [[nodiscard]] std::string label() const { return _label; }

private:
  const std::string _label;
};

std::ostream& operator <<(std::ostream &out, const State &state) {
  return out << state.label();
}

class Transition final {
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

std::ostream& operator <<(std::ostream &out, const Transition &transition) {
  out << transition.source();
  out << " --[";

  bool separator = false;
  for (const auto & i : transition.symbol()) {
    out << (separator ? ", " : "") << i;
    separator = true;
  }

  out << "]--> ";
  out << transition.target();

  return out;
}

class Automaton final {
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

std::ostream& operator <<(std::ostream &out, const Automaton &automaton) {
  bool separator;
 
  out << "S0 = {";
  separator = false;
  for (const auto &state: automaton._initial_states) {
    out << (separator ? ", " : "") << state;
  }
  out << "}" << std::endl;

  for (const auto &entry: automaton._final_states) {
    out << "F" << entry.first << " = {";
    separator = false;
    for (const auto &state: entry.second) {
      out << (separator ? ", " : "") << state;
    }
    out << "}" << std::endl;
  }

  out << "T = {" << std::endl;
  separator = false;
  for (const auto &entry: automaton._transitions) {
    for (const auto &transition: entry.second) {
      out << (separator ? "\n" : "") << "  " << transition;
      separator = true;
    }
  }
  out << std::endl << "}";

  return out;
}

} // namespace model::fsm
