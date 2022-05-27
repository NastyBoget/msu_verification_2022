#include "ltl.h"

namespace model::ltl {

std::vector<std::unique_ptr<const Formula>> Formula::_formulae;


Formula::BoolTernary Formula::operator ()(std::map<std::string, bool> &values) const {
    // THIS OPERATOR WORKS CORRECTLY ONLY FOR ATOM, NOT, AND, OR, X
    // calculates value of the formula if it consists only of classical operators
    switch (this->kind()) {
        case ATOM:
            if (this->prop() == "true") return TRUE;
            if (this->prop() == "false") return FALSE;
            return values[this->prop()] ? TRUE : FALSE;
        case X:
            return values[this->prop()] ? TRUE : FALSE;
        case NOT: {
            auto arg_value = this->lhs()(values);
            if (arg_value == UNKNOWN)
                return UNKNOWN;
            return arg_value == TRUE ? FALSE : TRUE;
        }
        case AND: {
            auto left_value = this->lhs()(values);
            auto right_value = this->rhs()(values);
            if (left_value == UNKNOWN or right_value == UNKNOWN)
                return UNKNOWN;
            return (left_value == TRUE and right_value == TRUE) ? TRUE : FALSE;
        }
        case OR:{
            auto left_value = this->lhs()(values);
            auto right_value = this->rhs()(values);
            if (left_value == UNKNOWN or right_value == UNKNOWN)
                return UNKNOWN;
            return (left_value == TRUE or right_value == TRUE) ? TRUE : FALSE;
        }
        // ignore these formulas
        case U:
        case IMPL:
        case G:
        case F:
        case R:
            return UNKNOWN;
    }
    return UNKNOWN;
}


Formula::BoolTernary Formula::operator ()(std::map<std::string, Formula> &values) const {
    // THIS OPERATOR WORKS CORRECTLY ONLY FOR ATOM, NOT, AND, OR, X, U
    // calculates value of the formula according to the presence of subformulas in values
    switch (this->kind()) {
        case ATOM:
        case X: {
            if (values.find(this->prop()) != values.end())
                return TRUE;
            return FALSE;
        }
        case NOT: {
            auto value = this->lhs()(values);
            if (value == FALSE)
                return TRUE;
            return value == UNKNOWN ? value : FALSE;
        }
        case AND: {
            auto left_value = this->lhs()(values);
            auto right_value = this->rhs()(values);
            if (left_value == UNKNOWN or right_value == UNKNOWN)
                return UNKNOWN;
            if (left_value == TRUE and right_value == TRUE)
                return TRUE;
            return FALSE;
        }
        case OR: {
            bool bool_value =  (this->lhs()(values) == TRUE) or (this->rhs()(values) == TRUE);
            if (bool_value)
                return TRUE;
            return FALSE;
        }
        case U: {
            if (values.find(this->prop()) != values.end())
                return TRUE;
            return UNKNOWN;
        }
        // ignore these formulas
        case IMPL:
        case G:
        case F:
        case R:
            return UNKNOWN;
    }
    return UNKNOWN;
}


bool Formula::operator ==(const Formula &other) const {
    if (this->kind() != other.kind() || this->prop() != other.prop()) {
        return false;
    }
    switch (this->kind()) {
        case ATOM:
            return true;
        case NOT:
        case X:
        case G:
        case F:
            return this->lhs() == other.lhs();
        case AND:
        case OR:
        case IMPL:
        case U:
        case R:
            return (this->lhs() == other.lhs()) && (this->rhs() == other.rhs());
    }
    return false;
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
    return f;
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
    return f;
}


} // namespace model::ltl
