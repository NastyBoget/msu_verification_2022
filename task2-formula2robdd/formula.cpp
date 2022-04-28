#include "formula.h"

namespace model::logic {

const Formula Formula::F(Formula::FALSE);
const Formula Formula::T(Formula::TRUE);

const Formula F = Formula::F;
const Formula T = Formula::T;

std::vector<std::unique_ptr<const Formula>> Formula::_formulae;

std::ostream& operator <<(std::ostream &out, const Formula &formula) {
    switch (formula.kind()) {
    case Formula::FALSE:
        return out << "false";
    case Formula::TRUE:
        return out << "true";
    case Formula::VAR:
        return out << "x" << formula.var();
    case Formula::NOT:
        return out << "!(" << formula.arg() << ")";
    case Formula::AND:
        return out << "(" << formula.lhs() << ") && (" << formula.rhs() << ")";
    case Formula::OR:
        return out << "(" << formula.lhs() << ") || (" << formula.rhs() << ")";
    case Formula::XOR:
        return out << "(" << formula.lhs() << ") != (" << formula.rhs() << ")";
    case Formula::IMPL:
        return out << "(" << formula.lhs() << ") -> (" << formula.rhs() << ")";
    case Formula::EQ:
        return out << "(" << formula.lhs() << ") == (" << formula.rhs() << ")";
    }

    return out;
}


const Formula& Formula::operator ()(int var_num, bool value) const {
    switch (this->kind()) {
        case Formula::FALSE:
            return F;
        case Formula::TRUE:
            return T;
        case Formula::VAR: {
            if (var_num != this->var())
                return *this;
            return value ? T : F;
        }
        case Formula::NOT: {
            return !this->arg()(var_num, value);
        }
        case Formula::AND:
            return this->lhs()(var_num, value) && this->rhs()(var_num, value);
        case Formula::OR:
            return this->lhs()(var_num, value) || this->rhs()(var_num, value);
        case Formula::XOR:
            return this->lhs()(var_num, value) != this->rhs()(var_num, value);
        case Formula::IMPL:
            return this->lhs()(var_num, value) >> this->rhs()(var_num, value);
        case Formula::EQ:
            return this->lhs()(var_num, value) == this->rhs()(var_num, value);
    }
}


bool Formula::operator ()() const {
    switch (this->kind()) {
        case Formula::FALSE:
            return false;
        case Formula::TRUE:
            return true;
        case Formula::VAR:
            throw std::invalid_argument("Formula has variables");
        case Formula::NOT:
            return not(this->arg()());
        case Formula::AND:
            return this->lhs()() and this->rhs()();
        case Formula::OR:
            return this->lhs()() or this->rhs()();
        case Formula::XOR:
            return this->lhs()() != this->rhs()();
        case Formula::IMPL:
            return not(this->lhs()()) or this->rhs()();
        case Formula::EQ:
            return this->lhs()() == this->rhs()();
    }
}


} // namespace model::logic
