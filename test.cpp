#include "ltl.h"
#include "fsm.h"

using namespace model::ltl;
using namespace model::fsm;

void test_make_standard() {
    // atom ! && || >> G F X U R
    const Formula &atom_p = P("p");
    const Formula &atom_q = P("q");
    const Formula &formula_not = !atom_p;
    const Formula &formula_g = G(atom_p);
    const Formula &formula_f = F(atom_p);
    const Formula &formula_x = X(atom_p);
    const Formula &formula_and = atom_p && atom_q;
    const Formula &formula_or = atom_p || atom_q;
    const Formula &formula_impl = atom_p >> atom_q;
    const Formula &formula_u = U(atom_p, atom_q);
    const Formula &formula_r = R(atom_p, atom_q);

    std::cout << "Before: " << atom_p << "\tAfter: " << make_standard(atom_p) << std::endl;
    std::cout << "Before: " << atom_q << "\tAfter: " << make_standard(atom_q) << std::endl;
    std::cout << "Before: " << formula_not << "\tAfter: " << make_standard(formula_not) << std::endl;
    std::cout << "Before: " << formula_g << "\tAfter: " << make_standard(formula_g) << std::endl;
    std::cout << "Before: " << formula_f << "\tAfter: " << make_standard(formula_f) << std::endl;
    std::cout << "Before: " << formula_x << "\tAfter: " << make_standard(formula_x) << std::endl;
    std::cout << "Before: " << formula_and << "\tAfter: " << make_standard(formula_and) << std::endl;
    std::cout << "Before: " << formula_or << "\tAfter: " << make_standard(formula_or) << std::endl;
    std::cout << "Before: " << formula_impl << "\tAfter: " << make_standard(formula_impl) << std::endl;
    std::cout << "Before: " << formula_u << "\tAfter: " << make_standard(formula_u) << std::endl;
    std::cout << "Before: " << formula_r << "\tAfter: " << make_standard(formula_r) << std::endl;

    std::cout << std::endl;
}


int main() {
    test_make_standard();

    const Formula &formula = G(P("p") >> F(P("q")));

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
