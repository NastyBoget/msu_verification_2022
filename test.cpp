#include "ltl.h"
#include "fsm.h"

using namespace model::ltl;
using namespace model::fsm;


void test_make_standard(std::vector<const Formula> &formulas) {
    std::cout << "Test make standard:" << std::endl;
    for (auto &f : formulas) {
        std::cout << "Before: " << f << "\tAfter: " << make_standard(f) << std::endl;
    }
    std::cout << "====================================================================================" << std::endl;
    std::cout << std::endl;
}


void test_closure(std::vector<const Formula> &formulas) {
    std::cout << "Test closure:" << std::endl;
    for (auto &f : formulas) {
        const auto& st_f = move_x_inside(make_standard(f));
        std::cout << st_f << std::endl;
        auto closure = make_closure_set(st_f);
        closure = delete_duplicates(closure);
        for (auto &closure_f : closure) {
            std::cout << "\t\t" << closure_f.prop();
        }
        make_atoms_set(closure);
        std::cout << std::endl;
    }
    std::cout << "====================================================================================" << std::endl;
    std::cout << std::endl;
}

int main() {
    std::vector<const Formula> formulas;
    const Formula &atom_p = P("p");
    const Formula &atom_q = P("q");
    const Formula &formula = G(P("p") >> F(P("q")));
    const Formula &formula1 = U(P("p") >> X(P("q")), !P("p") && P("q"));
    const Formula &formula2 = X(U(P("p") >> X(P("q")), !P("p") && P("q")));

    formulas.push_back(atom_p);
    formulas.push_back(atom_q);
    formulas.push_back(!atom_p);
    formulas.push_back(G(atom_p));
    formulas.push_back(F(atom_p));
    formulas.push_back(X(atom_p));
    formulas.push_back(atom_p && atom_q);
    formulas.push_back(atom_p || atom_q);
    formulas.push_back(atom_p >> atom_q);
    formulas.push_back(U(atom_p, atom_q));
    formulas.push_back(R(atom_p, atom_q));
    formulas.push_back(formula);
    formulas.push_back(formula1);
    formulas.push_back(formula2);

    test_make_standard(formulas);
    test_closure(formulas);

    std::cout << "Formula: " << std::endl;
    std::cout << formula << std::endl << std::endl;

    Automaton automaton;
    automaton.add_state("s0");
    automaton.add_state("s1");
    automaton.add_state("s2");
    automaton.set_initial("s0");
    automaton.set_final("s1", 0);
    automaton.set_final("s2", 1);
    automaton.add_trans("s0", {"p"}, "s1");
    automaton.add_trans("s0", {"q"}, "s2");
    automaton.add_trans("s1", {}, "s0");
    automaton.add_trans("s1", {"p", "q"}, "s2");
    automaton.add_trans("s2", {}, "s2");

    std::cout << "Automaton: " << std::endl;
    std::cout << automaton << std::endl;

    return 0;
}
