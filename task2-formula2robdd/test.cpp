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
    std::cout << "end" << std::endl;
    return 0;
}
