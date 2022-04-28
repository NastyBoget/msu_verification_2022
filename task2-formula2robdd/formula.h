#pragma once

#include <iostream>
#include <memory>
#include <vector>

namespace model::logic {

class Formula final {
public:
    enum Kind {
        FALSE, // Constant false
        TRUE,  // Constant true
        VAR,   // Boolean variable: x[i]
        NOT,   // Negation: ~A
        AND,   // Conjunction: (A1 & A2)
        OR,    // Disjunction: (A1 | A2)
        XOR,   // Exclusive OR: (A1 ^ A2)
        IMPL,  // Implication: (A1 -> A2)
        EQ     // Equivalence: (A1 <-> A2)
    };

    static const Formula F;
    static const Formula T;

    friend std::ostream& operator <<(std::ostream &out, const Formula &formula);

    const Formula& operator ()(int var_num, bool value) const;
    bool operator ()() const;
    const Formula& operator !() const;
    const Formula& operator &&(const Formula &rhs) const;
    const Formula& operator ||(const Formula &rhs) const;
    const Formula& operator >>(const Formula &rhs) const;
    const Formula& operator ==(const Formula &rhs) const;
    const Formula& operator !=(const Formula &rhs) const;

    friend const Formula& x(int i);

    [[nodiscard]] Kind kind() const { return _kind; }
    [[nodiscard]] int var() const { return _var; }

    [[nodiscard]] const Formula& arg() const { return *_lhs; }
    [[nodiscard]] const Formula& lhs() const { return *_lhs; }
    [[nodiscard]] const Formula& rhs() const { return *_rhs; }

private:
    Formula(Kind kind, int var, const Formula *lhs, const Formula *rhs):
        _kind(kind), _var(var), _lhs(lhs), _rhs(rhs) {}

    explicit Formula(int var):
        Formula(VAR, var, nullptr, nullptr) {}

    explicit Formula(Kind kind):
        Formula(kind, -1, nullptr, nullptr) {}

    Formula(Kind kind, const Formula *arg):
        Formula(kind, -1, arg, nullptr) {}

    Formula(Kind kind, const Formula *lhs, const Formula *rhs):
        Formula(kind, -1, lhs, rhs) {}

    static const Formula& alloc(const Formula *formula) {
        _formulae.push_back(std::unique_ptr<const Formula>(formula));
        return *formula;
  }

    static std::vector<std::unique_ptr<const Formula>> _formulae;

    const Kind _kind;
    const int _var;
    const Formula* _lhs;
    const Formula* _rhs;
};

extern const Formula F;
extern const Formula T;

inline const Formula& Formula::operator !() const {
    return alloc(new Formula(NOT, this));
}

inline const Formula& Formula::operator &&(const Formula &rhs) const {
    return alloc(new Formula(AND, this, &rhs));
}

inline const Formula& Formula::operator ||(const Formula &rhs) const {
    return alloc(new Formula(OR, this, &rhs));
}

inline const Formula& Formula::operator >>(const Formula &rhs) const {
    return alloc(new Formula(IMPL, this, &rhs));
}

inline const Formula& Formula::operator ==(const Formula &rhs) const {
    return alloc(new Formula(EQ, this, &rhs));
}

inline const Formula& Formula::operator !=(const Formula &rhs) const {
    return alloc(new Formula(XOR, this, &rhs));
}

inline const Formula& x(int var) {
    return Formula::alloc(new Formula(var));
}

std::ostream& operator <<(std::ostream &out, const Formula &formula);

} // namespace model::logic
