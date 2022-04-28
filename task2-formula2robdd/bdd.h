#pragma once

#include <memory>
#include <map>

#include "formula.h"

using namespace model::logic;

namespace model::bdd {

struct Node final {
    int var;

    const Node *low;
    const Node *high;

    Node(): var(), low(nullptr), high(nullptr) {}

    Node(int var, const Node *low, const Node *high):
        var(var), low(low), high(high) {}
};

class Bdd final {
public:
    static const Node zero;
    static const Node one;

    const Node& create(const Formula &formula);

private:
    // Pool of nodes organized so as to efficiently
    // search for a given (var, low, high).
    std::map<std::tuple<int, const Node *, const Node *>, const Node> _nodes;

};

std::ostream& operator <<(std::ostream &out, const Node &node);

} // namespace model::bdd
