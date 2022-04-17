#include "fsm.h"

namespace model::fsm {

bool VERBOSE = false;


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

std::vector<const Formula> delete_duplicates(std::vector<const Formula> &formulas) {
    // removes equal formulas from the result of make_closure_set function
    std::vector<const Formula> result;
    for (auto &f : formulas) {
        if (std::find(result.begin(), result.end(), f) == result.end()) {
            result.push_back(f);
        }
    }
    return result;
}


std::map<std::string, const Formula> get_true_cl_formulas(std::vector<const Formula> &closure,
        std::map<std::string, bool> &atoms_values) {
    std::map<std::string, const Formula> true_formulas;
    for (auto &closure_elem : closure) {
        if (closure_elem(atoms_values)) {
            true_formulas.insert({closure_elem.prop(), closure_elem});
        }
    }

    if (VERBOSE) {
        for (auto &elem: true_formulas) {
            std:: cout << elem.first << "\t\t";
        }
        std::cout << std::endl;
    }
    return true_formulas;
}


std::map<std::string, std::map<std::string, const Formula>>
get_states_for_atoms_values(const std::vector<const Formula> &closure, size_t& states_number,
        const std::map<std::string, const Formula> &true_formulas) {

    std::map<std::string, std::map<std::string, const Formula>> local_states;
    local_states.insert({"s" + std::to_string(states_number), true_formulas});
    states_number++;
    for (auto &closure_elem : closure) {
        std::__1::map<std::string, std::__1::map<std::string, const Formula>> additional_states;
        for (auto &local_state : local_states) {
            if (local_state.second.find(closure_elem.prop()) != local_state.second.end())
                continue;
            auto closure_elem_value = closure_elem(local_state.second);
            if (closure_elem_value == Formula::TRUE) {
                local_state.second.insert({closure_elem.prop(), closure_elem});
                continue;
            }
            if (closure_elem_value == Formula::UNKNOWN) {
                auto new_state_name = "s" + std::to_string(states_number);
                additional_states.insert({new_state_name, local_state.second});
                additional_states[new_state_name].insert({closure_elem.prop(), closure_elem});
                states_number++;
            }
        }
        for (const auto &additional_state : additional_states) {
            local_states.insert({additional_state.first, additional_state.second});
        }
    }
    return local_states;
}


std::map<std::string, std::map<std::string, const Formula>> make_atoms_set(std::vector<const Formula> &closure) {
    // returns set of pairs {state_name, vector of true formulas}
    std::map<std::string, std::map<std::string, const Formula>> states;
    std::map<std::string, bool> atoms_values;

    // init atoms values
    for (auto &closure_elem : closure) {
        if (closure_elem.kind() == Formula::ATOM or closure_elem.kind() == Formula::X) {
            if (closure_elem.kind() == Formula::ATOM and
                (closure_elem.prop() == "true" or closure_elem.prop() == "false"))
                continue;
            atoms_values.insert({closure_elem.prop(), false});
        }
    }
    if (VERBOSE)
        std::cout << std::endl;

    // find states for different atoms values combinations
    size_t states_number = 1;
    size_t rows_number = 1 << atoms_values.size();
    for (size_t i = 0; i < rows_number; i++) {
        size_t j = 0;
        // fill atoms values with all possible combinations
        for (auto &atoms_value : atoms_values) {
            atoms_value.second = (bool) ((i >> j) & 1);
            j++;

            if (VERBOSE)
                std::cout << atoms_value.first << "=" << atoms_value.second << ",";
        }
        if (VERBOSE)
            std::cout << std::endl;

        std::map<std::string, const Formula> true_formulas = get_true_cl_formulas(closure, atoms_values);
        auto local_states = get_states_for_atoms_values(closure, states_number, true_formulas);

        for (const auto &local_state : local_states) {
            states.insert({local_state.first, local_state.second});
        }

    }

    if (VERBOSE) {
        std::cout << std::endl;
        std::cout << "States:" << std::endl;
        for (const auto &state : states) {
            std::cout << state.first << ": ";
            for (const auto &formulas : state.second) {
                std::cout << formulas.first << ", ";
            }
        }
        std::cout << std::endl;
    }

    return states;
}


// TODO
// make initial states set
// make final states set
// make transitions


}