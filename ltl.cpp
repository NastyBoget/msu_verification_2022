#include "ltl.h"

namespace model::ltl {

std::vector<std::unique_ptr<const Formula>> Formula::_formulae;


bool Formula::operator ()(std::map<std::string, bool> &values) const {
    // THIS OPERATOR WORKS CORRECTLY ONLY FOR ATOM, NOT, AND, OR, X
    // calculates value of the formula if it consists only of classical operators
    switch (this->kind()) {
        case Formula::ATOM:
            if (this->prop() == "true") return true;
            if (this->prop() == "false") return false;
            return values[this->prop()];
        case Formula::X:
            return values[this->prop()];
        case Formula::NOT:
            return !this->lhs()(values);
        case Formula::AND:
            return this->lhs()(values) && this->rhs()(values);
        case Formula::OR:
            return this->lhs()(values) || this->rhs()(values);
        // ignore these formulas
        case Formula::U:
        case Formula::IMPL:
        case Formula::G:
        case Formula::F:
        case Formula::R:
            return false;
    }
}


Formula::BoolTernary Formula::operator ()(std::map<std::string, const Formula> &values) const {
    // THIS OPERATOR WORKS CORRECTLY ONLY FOR ATOM, NOT, AND, OR, X, U
    // calculates value of the formula according to the presence of subformulas in values
    switch (this->kind()) {
        case Formula::ATOM:
        case Formula::X: {
            if (values.find(this->prop()) != values.end())
                return Formula::TRUE;
            return Formula::FALSE;
        }
        case Formula::NOT: {
            if (this->lhs()(values) == Formula::TRUE)
                return Formula::FALSE;
            return Formula::TRUE;
        }
        case Formula::AND: {
            bool bool_value =  (this->lhs()(values) == Formula::TRUE) && (this->rhs()(values) == Formula::TRUE);
            if (bool_value)
                return Formula::TRUE;
            return Formula::FALSE;
        }
        case Formula::OR: {
            bool bool_value =  (this->lhs()(values) == Formula::TRUE) || (this->rhs()(values) == Formula::TRUE);
            if (bool_value)
                return Formula::TRUE;
            return Formula::FALSE;
        }
        case Formula::U: {
            if (values.find(this->rhs().prop()) != values.end())
                return Formula::TRUE;
            if (values.find(this->lhs().prop()) != values.end())
                return Formula::UNKNOWN;
            return Formula::FALSE;
        }
        // ignore these formulas
        case Formula::IMPL:
        case Formula::G:
        case Formula::F:
        case Formula::R:
            return Formula::FALSE;
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
