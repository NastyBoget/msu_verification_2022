#ifndef TASK3_CNF_HPP
#define TASK3_CNF_HPP

#include <set>
#include <vector>


class CNF {

public:
    explicit CNF(std::istream &fin); // reading CNF from a file in DIMACS format
    [[nodiscard]] bool is_sat() const; // checking the satisfiability of a formula using DPLL algorithm
    friend std::ostream& operator <<(std::ostream &out, const CNF &cnf);

    enum BoolTernary {
        TRUE,
        FALSE,
        UNKNOWN,
    };

private:
    size_t var_num, clause_num;
    std::vector<std::vector<int>> clauses;
    void unit_prop(std::stack<int> &assignments, std::vector<BoolTernary> &var_values) const;
    [[nodiscard]] bool all_true_clauses(std::vector<BoolTernary> &var_values) const;
    [[nodiscard]] bool exists_false_clause(std::vector<BoolTernary> &var_values) const;
    void roll_back(std::stack<int> &assignments,
                   std::stack<std::pair<int, BoolTernary>> &decisions,
                   std::vector<BoolTernary> &var_values) const;
};

#endif //TASK3_CNF_HPP
