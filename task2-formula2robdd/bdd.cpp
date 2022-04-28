#include "bdd.h"

namespace model::bdd {

const Node Bdd::zero(-1, nullptr, nullptr);
const Node Bdd::one (-1, nullptr, nullptr);


int find_least_variable(const Formula &formula) {
    switch (formula.kind()) {
        case Formula::FALSE:
        case Formula::TRUE:
            return INT_MAX;
        case Formula::VAR:
            return formula.var();
        case Formula::NOT:
            return find_least_variable(formula.arg());
        case Formula::AND:
        case Formula::OR:
        case Formula::XOR:
        case Formula::IMPL:
        case Formula::EQ:
            return std::min(find_least_variable(formula.lhs()), find_least_variable(formula.rhs()));
    }
}


const Node& Bdd::create(const Formula &formula) {
    // TODO Reduce
    if (formula.kind() == Formula::FALSE)
        return zero;
    if (formula.kind() == Formula::TRUE)
        return one;

    if (formula.kind() == Formula::VAR) {
        auto node_tuple = std::make_tuple(formula.var(), &zero, &one);
        auto node_it = _nodes.find(node_tuple);
        if (node_it != _nodes.end()){
            return node_it->second;
        }
        _nodes.emplace(node_tuple, Node(formula.var(), &zero, &one));
        return _nodes[node_tuple];
    }
    // find variable x with the least number
    int v_num = find_least_variable(formula);
    if (v_num == INT_MAX) {
        bool value = formula();
        return value ? one : zero;
    }

    // Apply (F * G) = Reduce ( Compose (x, Apply (F|x=1 * G|x=1), Apply (F|x=0 * G|x=0) ) )
    auto &high = create(formula(v_num, true));
    auto &low = create(formula(v_num, false));
    if (&low == &high) {
        // second rule of Reduce: delete redundant vertex
        return high;
    }

    auto node_tuple = std::make_tuple(v_num, &low, &high);
    auto node_it = _nodes.find(node_tuple);
    if (node_it != _nodes.end()){
        return node_it->second;
    }
    _nodes.emplace(node_tuple, Node(v_num, &low, &high));
    return _nodes[node_tuple];

}


void print_bdd(std::ostream &out, const std::string &prefix, const Node &node, bool is_left) {
    out << prefix;
    out << (is_left ? "├──" : "└──" );

    // print the value of the node
    if (&node == &Bdd::zero) {
        out << "zero" << std::endl;
        return;
    }
    if (&node == &Bdd::one) {
        out << "one" << std::endl;
        return;
    }
    out << "x" << node.var << std::endl;

    // enter the next tree level - left and right branch
    if (node.low != nullptr)
        print_bdd(out, prefix + (is_left ? "│   " : "    "), *node.low, true);
    if (node.high != nullptr)
        print_bdd(out, prefix + (is_left ? "│   " : "    "), *node.high, false);
}


std::ostream& operator <<(std::ostream &out, const Node &node) {
    print_bdd(out, "", node, false);
    return out;
}


} // namespace model::bdd
