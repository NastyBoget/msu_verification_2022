#include "ltl.h"
#include "fsm.h"

using namespace model::ltl;
using namespace model::fsm;


void test_closure(std::vector<const Formula> &formulas) {
    for (auto &f : formulas) {
        std::cout << "Formula: " << f << std::endl;
        Automaton automaton = ltl_to_buchi(f);
        std::cout << "Automaton: " << std::endl;
        std::cout << automaton << std::endl;
        std::cout << std::endl;
    }
}

int main() {
    std::vector<const Formula> formulas;
//    const Formula &atom_p = P("p");
//    const Formula &atom_q = P("q");
//    const Formula &formula = G(P("p") >> F(P("q")));
//    const Formula &formula1 = U(P("p") >> X(P("q")), !P("p") && P("q"));
//    const Formula &formula2 = X(U(P("p") >> X(P("q")), !P("p") && P("q")));
//
//    formulas.push_back(atom_p);
//    formulas.push_back(atom_q);
//    formulas.push_back(!atom_p);
//    formulas.push_back(G(atom_p));
//    formulas.push_back(F(atom_p));
//    formulas.push_back(X(atom_p));
//    formulas.push_back(atom_p && atom_q);
//    formulas.push_back(atom_p || atom_q);
//    formulas.push_back(atom_p >> atom_q);
//    formulas.push_back(U(atom_p, atom_q));
//    formulas.push_back(R(atom_p, atom_q));
//    formulas.push_back(formula);
//    formulas.push_back(formula1);
//    formulas.push_back(formula2);

    formulas.push_back(G(P("p") >> X(P("q"))));
    formulas.push_back(U(P("p") >> X(P("q")), !P("p") && P("q")));
    formulas.push_back(U(P("x"), U(P("y"), P("z"))));
    test_closure(formulas);

    return 0;
}
