#include "ltl.h"

namespace model::ltl {

std::vector<std::unique_ptr<const Formula>> Formula::_formulae;


bool Formula::operator ()(bool value) const {
    // THIS OPERATOR WORKS CORRECTLY ONLY FOR ATOM, NOT, AND, OR, X
    switch (this->kind()) {
        case Formula::ATOM:
            if (this->prop() == "true") return true;
            if (this->prop() == "false") return false;
            return value;
        case Formula::X:
            return this->lhs()(value);
        case Formula::NOT:
            return !this->lhs()(value);
        case Formula::AND:
            return this->lhs()(value) && this->rhs()(value);
        case Formula::OR:
            return this->lhs()(value) || this->rhs()(value);
        // ignore these formulas
        case Formula::IMPL:
        case Formula::G:
        case Formula::F:
        case Formula::U:
        case Formula::R:
            return false;
    }
}


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


const Formula& add_x(const Formula& f, int x_number) {
    if (x_number == 0) return f;
    return add_x(X(f), x_number - 1);
}


const Formula& move_x_inside(const Formula& f, int x_number) {
    // FORMULA SHOULD BE MADE STANDARD BEFOREHAND
    // X(~p) = ~X(p)
    // X (p && q) = Xp && Xq
    // X (p || q) = Xp || Xq
    // X (p U q) = Xp U Xq
    switch (f.kind()) {
        case Formula::ATOM: {
            return add_x(f, x_number);
        }
        case Formula::NOT:
            return !move_x_inside(f.lhs(), x_number);
        case Formula::X:
            return move_x_inside(f.lhs(), x_number + 1);
        case Formula::AND:
            return move_x_inside(f.lhs(), x_number) && move_x_inside(f.rhs(), x_number);
        case Formula::OR:
            return move_x_inside(f.lhs(), x_number) || move_x_inside(f.rhs(), x_number);
        case Formula::U:
            return U(move_x_inside(f.lhs(), x_number), move_x_inside(f.rhs(), x_number));
        case Formula::IMPL:
        case Formula::G:
        case Formula::F:
        case Formula::R:
            return f;
    }
}


} // namespace model::ltl
