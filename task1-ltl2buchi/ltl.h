#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include <map>
#include <set>

namespace model::ltl {

class Formula final {
public:
    enum Kind {
        ATOM, // Atomic proposition: p
        NOT,  // Negation: ~A
        AND,  // Conjunction: (A1 & A2)
        OR,   // Disjunction: (A1 | A2)
        IMPL, // Implication: (A1 -> A2)
        X,    // NeXt time: X{A}
        G,    // Globally: G{A}
        F,    // In the Future: F{A}
        U,    // Until: (A1 U A2)
        R     // Release: (A1 R A2)
    };

    enum BoolTernary {
        TRUE,
        FALSE,
        UNKNOWN,
    };

    bool operator ==(const Formula &other) const;
    BoolTernary operator ()(std::map<std::string, bool> &values) const;
    BoolTernary operator ()(std::map<std::string, Formula> &values) const;
    const Formula& operator !() const;
    const Formula& operator &&(const Formula &rhs) const;
    const Formula& operator ||(const Formula &rhs) const;
    const Formula& operator >>(const Formula &rhs) const;

    friend const Formula& P(const std::string &prop);
    friend const Formula& X(const Formula &arg);
    friend const Formula& G(const Formula &arg);
    friend const Formula& F(const Formula &arg);
    friend const Formula& U(const Formula &lhs, const Formula &rhs);
    friend const Formula& R(const Formula &lhs, const Formula &rhs);

    [[nodiscard]] Kind kind() const { return _kind; }
    [[nodiscard]] std::string prop() const { return _prop; }

    [[nodiscard]] const Formula& arg() const { return *_lhs; }
    [[nodiscard]] const Formula& lhs() const { return *_lhs; }
    [[nodiscard]] const Formula& rhs() const { return *_rhs; }

private:
    Formula(Kind kind, std::string prop, const Formula *lhs, const Formula *rhs):
        _kind(kind), _prop(std::move(prop)), _lhs(lhs), _rhs(rhs) {}

    explicit Formula(const std::string &prop):
        Formula(ATOM, prop, nullptr, nullptr) {}

    Formula(Kind kind, const std::string &prop, const Formula *arg):
        Formula(kind, prop, arg, nullptr) {}

    static const Formula& alloc(const Formula *formula) {
        _formulae.push_back(std::unique_ptr<const Formula>(formula));
        return *formula;
    }

    static std::vector<std::unique_ptr<const Formula>> _formulae;

    const Kind _kind;  // kind of operation
    const std::string _prop;  // string representation
    const Formula *_lhs;  // left operand
    const Formula *_rhs;  // right operand
};

inline const Formula& Formula::operator !() const {
    return alloc(new Formula(NOT, "!(" + this->prop() + ")", this));
}

inline const Formula& Formula::operator &&(const Formula &rhs) const {
    return alloc(new Formula(AND, "(" + this->prop() + ")&&(" + rhs.prop() + ")", this, &rhs));
}

inline const Formula& Formula::operator ||(const Formula &rhs) const {
    return alloc(new Formula(OR, "(" + this->prop() + ")||(" + rhs.prop() + ")", this, &rhs));
}

inline const Formula& Formula::operator >>(const Formula &rhs) const {
    return alloc(new Formula(IMPL, "(" + this->prop() + ")->(" + rhs.prop() + ")", this, &rhs));
}

inline const Formula& P(const std::string &prop) {
    return Formula::alloc(new Formula(prop));
}

inline const Formula& X(const Formula &arg) {
    return Formula::alloc(new Formula(Formula::X, "X(" + arg.prop() + ")", &arg));
}

inline const Formula& G(const Formula &arg) {
    return Formula::alloc(new Formula(Formula::G, "G(" + arg.prop() + ")", &arg));
}

inline const Formula& F(const Formula &arg) {
    return Formula::alloc(new Formula(Formula::F, "F(" + arg.prop() + ")", &arg));
}

inline const Formula& U(const Formula &lhs, const Formula &rhs) {
    return Formula::alloc(new Formula(Formula::U, "(" + lhs.prop() + ")U(" + rhs.prop() + ")", &lhs, &rhs));
}

inline const Formula& R(const Formula &lhs, const Formula &rhs) {
    return Formula::alloc(new Formula(Formula::R, "(" + lhs.prop() + ")R(" + rhs.prop() + ")", &lhs, &rhs));
}

std::ostream& operator <<(std::ostream &out, const Formula &formula);

const Formula& make_standard(const Formula& f);

const Formula& move_x_inside(const Formula& f, int x_number = 0);

const Formula& compute_constant_subformulas(const Formula& f);

} // namespace model::ltl
