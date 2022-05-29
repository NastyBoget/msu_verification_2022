#include "ltl.h"
#include "fsm.h"

using namespace model::ltl;
using namespace model::fsm;


void test(std::vector<Formula> &formulas) {
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

    formulas.push_back(F(P("p") >> X(!P("q")))); // 10
    formulas.push_back(U(P("p") >> X(P("q")), !P("p") && P("q"))); // 12
    formulas.push_back(U(P("x"), U(P("y"), P("z")))); // 12
    formulas.push_back(U(F(P("p")), G(P("q")))); // 13
    formulas.push_back(U(F(P("p")), !P("p") && X(G(P("q"))))); //28
    formulas.push_back(U(P("x"), G(U(P("y"), P("z"))))); // 21
    test(formulas);

    return 0;
}
