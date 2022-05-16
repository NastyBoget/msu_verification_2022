#include <sstream>
#include <stack>
#include "cnf.hpp"


CNF::CNF(std::istream &fin) {
    std::string line;
    var_num = 0;
    clause_num = 0;

    while (std::getline(fin, line)) {
        if (line == std::string("") or line == std::string("0") or line[0] == 'c' or line[0] == '%')
            continue;

        if (line[0] == 'p') {
            std::string tmp;
            std::stringstream ss(line);
            ss >> tmp >> tmp >> var_num >> clause_num;
            clauses.reserve(clause_num);
            continue;
        }

        std::stringstream ss(line);
        std::vector<int> clause;
        int variable;
        while (ss >> variable and variable != 0)
            clause.push_back(variable);
        clauses.push_back(clause);
    }

    if (clauses.size() != clause_num)
        throw std::invalid_argument("Invalid number of clauses");

}

std::ostream & operator<<(std::ostream &out, const CNF &cnf) {
    out << "Variables number: " << cnf.var_num << std::endl;
    out << "Clauses number: " << cnf.clause_num << std::endl;
    out << "Clauses:" << std::endl;

    bool separator = false;
    for (size_t i = 0; i < cnf.clause_num; i++) {
        if (separator)
            out << " &";
        else
            out << "  ";
        out << " (";

        for (auto it = cnf.clauses[i].begin(); it != cnf.clauses[i].end(); it++) {
            if (it != cnf.clauses[i].begin())
                out << " \\/ ";
            if (*it < 0)
                out << "~x" << (-*it);
            else
                out << "x" << *it;
        }

        out << ")" << std::endl;
        separator = true;
    }
    return out;
}


CNF::BoolTernary get_value(int literal, std::vector<CNF::BoolTernary> &var_values) {
    size_t index = abs(literal) - 1;
    if (literal > 0 or var_values[index] == CNF::BoolTernary::UNKNOWN)
        return var_values[index];
    return var_values[index] == CNF::BoolTernary::TRUE ? CNF::BoolTernary::FALSE : CNF::BoolTernary::TRUE;
}

bool is_unit(const std::vector<int> &clause, std::vector<CNF::BoolTernary> &var_values) {
    int cnt = 0;
    for (auto it = clause.begin(); it != clause.end() and cnt < 2; it++)
        if (get_value(*it, var_values) == CNF::BoolTernary::UNKNOWN)
            cnt++;
    return cnt == 1;
}

bool is_true(const std::vector<int> &clause, std::vector<CNF::BoolTernary> &var_values) {
    for (auto it : clause)
        if (get_value(it, var_values) == CNF::BoolTernary::TRUE)
            return true;
    return false;
}

bool is_false(const std::vector<int> &clause, std::vector<CNF::BoolTernary> &var_values) {
    for (auto it : clause)
        if (get_value(it, var_values) != CNF::BoolTernary::FALSE)
            return false;
    return true;
}

int get_literal(const std::vector<int> &clause, std::vector<CNF::BoolTernary> &var_values) {
    for (auto it : clause)
        if (get_value(it, var_values) == CNF::BoolTernary::UNKNOWN)
            return it;
    throw std::invalid_argument("Clause is not unit");
}


void CNF::unit_prop(std::stack<int> &assignments, std::vector<BoolTernary> &var_values) const {
    bool unit_clause_found = true;

    while (unit_clause_found) {
        unit_clause_found = false;
        for (const auto & clause : clauses) {
            if (is_true(clause, var_values) or !is_unit(clause, var_values))
                continue;

            int literal = get_literal(clause, var_values);
            BoolTernary value = literal > 0 ? BoolTernary::TRUE : BoolTernary ::FALSE;
            var_values[abs(literal) - 1] = value;
            assignments.push(literal);
            unit_clause_found = true;
        }
    }
}

bool CNF::all_true_clauses(std::vector<BoolTernary> &var_values) const {
    for (const auto & clause : clauses)
        if (!is_true(clause, var_values))
            return false;
    return true;
}

bool CNF::exists_false_clause(std::vector<BoolTernary> &var_values) const {
    for (const auto & clause : clauses)
        if (is_false(clause, var_values))
            return true;
    return false;
}

void CNF::roll_back(std::stack<int> &assignments, std::stack<std::pair<int, BoolTernary>> &decisions,
        std::vector<BoolTernary> &var_values) const {
    while (assignments.top() != decisions.top().first) {
        var_values[abs(assignments.top()) - 1] = BoolTernary::UNKNOWN;
        assignments.pop();
    }

    auto &decision = decisions.top();

    if (decision.second == BoolTernary::FALSE) {
        decision.second = BoolTernary::TRUE;
        var_values[abs(decision.first) - 1] = BoolTernary::TRUE;
    } else {
        assignments.pop();
        decisions.pop();
        var_values[abs(decision.first) - 1] = BoolTernary::UNKNOWN;

        if (!decisions.empty()) {
            roll_back(assignments, decisions, var_values);
        }
    }
}


bool CNF::is_sat() const {
    std::stack<int> assignments;
    std::stack<std::pair<int, BoolTernary>> decisions;
    std::vector<BoolTernary> var_values;
    var_values = std::vector<BoolTernary>(var_num, BoolTernary::UNKNOWN);

    while (true) {
        unit_prop(assignments, var_values);

        if (exists_false_clause(var_values)) {
            roll_back(assignments, decisions, var_values);
            if (decisions.empty())
                return false;
            continue;
        }

        if (all_true_clauses(var_values))
            return true;

        int index = 0;
        while (index < var_num and get_value(index + 1, var_values) != BoolTernary::UNKNOWN)
            index++;
        decisions.push(std::make_pair(index + 1, BoolTernary::FALSE));
        assignments.push(index + 1);
        var_values[index] = BoolTernary::FALSE;
    }
}
