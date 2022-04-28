#include <cassert>
#include <iostream>

#include "bdd.h"
#include "formula.h"

using namespace model::bdd;
using namespace model::logic;

int main() {
    const Formula &formula1 =  x(0) >>  x(1);
    const Formula &formula2 = !x(1) >> !x(0);
    const Formula &formula3 = formula1 == formula2;
    const Formula &formula4 = T;
    const Formula &formula5 = x(0) != x(1);
    const Formula &formula6 = !x(0) && x(1) || x(0) && !x(1);
    const Formula &formula7 = (!x(0) || !x(1)) && (x(0) || x(1));
 
    Bdd bdd;

    const Node& root1 = bdd.create(formula1);
    std::cout << formula1 << std::endl << root1 << std::endl;
    const Node& root2 = bdd.create(formula2);
    std::cout << formula2 << std::endl << root2 << std::endl;
    assert(&root1 == &root2);

    const Node& root3 = bdd.create(formula3);
    std::cout << formula3 << std::endl << root3 << std::endl;
    const Node& root4 = bdd.create(formula4);
    std::cout << formula4 << std::endl << root4 << std::endl;
    assert(&root3 == &root4);

    const Node& root5 = bdd.create(formula5);
    std::cout << formula5 << std::endl << root5 << std::endl;
    const Node& root6 = bdd.create(formula6);
    std::cout << formula6 << std::endl << root6 << std::endl;
    const Node& root7 = bdd.create(formula7);
    std::cout << formula7 << std::endl << root7 << std::endl;
    assert(&root5 == &root6);
    assert(&root6 == &root7);

    return 0;
}
