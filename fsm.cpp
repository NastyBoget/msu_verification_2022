#include "fsm.h"

namespace model::fsm {


std::ostream& operator <<(std::ostream &out, const State &state) {
    return out << state.label();
}


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


std::ostream& operator <<(std::ostream &out, const Automaton &automaton) {
    bool separator;

    out << "S0 = {";
    separator = false;
    for (const auto &state: automaton._initial_states) {
        out << (separator ? ", " : "") << state;
        separator = true;
    }
    out << "}" << std::endl;

    for (const auto &entry: automaton._final_states) {
        out << "F" << entry.first << " = {";
        separator = false;
        for (const auto &state: entry.second) {
            out << (separator ? ", " : "") << state;
            separator = true;
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

std::vector<const Formula> make_closure_set(const Formula &f) {
    // FORMULA SHOULD BE MADE STANDARD BEFOREHAND
    // make closure set (without negative formulas)
    // example: closure( (p -> Xq) U (~p && q) ) = { (p -> Xq) U (~p && q), ~p || Xq, ~p && q, Xq, p, q }
    std::vector<const Formula> closure, second_closure;
    switch (f.kind()) {
        case Formula::ATOM:
            closure.push_back(f);
            return closure;
        case Formula::NOT:
            return make_closure_set(f.lhs());
        case Formula::X:
            closure = make_closure_set(f.lhs());
            closure.push_back(f);
            return closure;
        case Formula::AND:
        case Formula::OR:
        case Formula::U:
            closure = make_closure_set(f.lhs());
            second_closure = make_closure_set(f.rhs());
            for (auto &second_closure_f : second_closure) {
                closure.push_back(second_closure_f);
            }
            closure.push_back(f);
            return closure;

        // ignore these cases
        case Formula::IMPL:
        case Formula::G:
        case Formula::F:
        case Formula::R:
            return closure;
    }
}

// TODO
// formula calculation
// make atoms set
// make initial states set
// make final states set
// make transitions


}