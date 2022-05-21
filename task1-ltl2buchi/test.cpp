#include "ltl.h"
#include "fsm.h"

using namespace model::ltl;
using namespace model::fsm;


void test_closure(std::vector<Formula> &formulas) {
    for (auto &f : formulas) {
        std::cout << "Formula: " << f << std::endl;
        Automaton automaton = ltl_to_buchi(f);
        std::cout << "Automaton: " << std::endl;
        std::cout << automaton << std::endl;
        std::cout << std::endl;
    }
}

int main() {
    std::vector<Formula> formulas;

    formulas.push_back(F(P("p") >> X(!P("q"))));
    formulas.push_back(U(P("p") >> X(P("q")), !P("p") && P("q")));
    formulas.push_back(U(P("x"), U(P("y"), P("z"))));
    test_closure(formulas);

    return 0;
}
