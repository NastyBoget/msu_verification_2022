#include "ltl.h"

namespace model::ltl {

std::vector<std::unique_ptr<const Formula>> Formula::_formulae;

bool Formula::operator ==(const Formula &other) const {
    if (this->kind() != other.kind() || this->prop() != other.prop()) {
        return false;
    }
    switch (this->kind()) {
        case Formula::ATOM:
            return true;
        case Formula::NOT:
        case Formula::X:
        case Formula::G:
        case Formula::F:
            return this->lhs() == other.lhs();
        case Formula::AND:
        case Formula::OR:
        case Formula::IMPL:
        case Formula::U:
        case Formula::R:
            return (this->lhs() == other.lhs()) && (this->rhs() == other.rhs());
    }
}

std::ostream& operator <<(std::ostream &out, const Formula &formula) {
    switch (formula.kind()) {
    case Formula::ATOM:
        return out << formula.prop();
    case Formula::NOT:
        return out << "!(" << formula.arg() << ")";
    case Formula::AND:
        return out << "(" << formula.lhs() << ") && (" << formula.rhs() << ")";
    case Formula::OR:
        return out << "(" << formula.lhs() << ") || (" << formula.rhs() << ")";
    case Formula::IMPL:
        return out << "(" << formula.lhs() << ") -> (" << formula.rhs() << ")";
    case Formula::X:
        return out << "X(" << formula.arg() << ")";
    case Formula::G:
        return out << "G(" << formula.arg() << ")";
    case Formula::F:
        return out << "F(" << formula.arg() << ")";
    case Formula::U:
        return out << "(" << formula.lhs() << ") U (" << formula.rhs() << ")";
    case Formula::R:
        return out << "(" << formula.lhs() << ") R (" << formula.rhs() << ")";
    }

    return out;
}


const Formula& make_standard(const Formula& f) {
    // apply equivalent transformations for IMPL, G, F, R
    // p -> q = ~p \/ q
    // G q = ~( true U ~q)
    // F q = true U q
    // p R q = ~(~p U ~q)

    switch (f.kind()) {
        case Formula::ATOM:
            // atoms aren't changed
            return P(f.prop());
        // check operands of the unary operations that don't need to be transformed
        case Formula::NOT:
            return !make_standard(f.lhs());
        case Formula::X:
            return X(make_standard(f.lhs()));
        // check operands of the binary operations that don't need to be transformed
        case Formula::AND:
            return make_standard(f.lhs()) && make_standard(f.rhs());
        case Formula::OR:
            return make_standard(f.lhs()) || make_standard(f.rhs());
        case Formula::U:
            return  U(make_standard(f.lhs()), make_standard(f.rhs()));
        // transform the operands
        case Formula::IMPL:
            // p -> q = ~p \/ q
            return !make_standard(f.lhs()) || make_standard(f.rhs());
        case Formula::G:
            // G q = ~( true U ~q)
            return !(U(P("true"), !make_standard(f.lhs())));
        case Formula::F:
            // F q = true U q
            return U(P("true"), make_standard(f.lhs()));
        case Formula::R:
            // p R q = ~(~p U ~q)
            return !(U(!make_standard(f.lhs()), !make_standard(f.rhs())));
    }


}

} // namespace model::ltl
