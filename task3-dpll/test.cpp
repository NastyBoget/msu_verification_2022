#include <cassert>
#include <iostream>
#include <ctime>
#include <fstream>
#include "cnf.hpp"


void test_directory(std::string path, int n1, int n2, bool is_sat, std::string test_name) {
    clock_t t_start = clock();
    for (int i = n1; i <= n2; i++) {
        std::string full_path = path + std::to_string(i) + ".cnf";
        std::ifstream fin(full_path);
        CNF cnf(fin);
        assert(cnf.is_sat() == is_sat);
    }
    clock_t t_end = clock();
    double time = double(t_end - t_start) / CLOCKS_PER_SEC;
    std::cout << test_name << ": " << time << " sec (" << (time / (n2 - n1 + 1) ) << " sec per iteration)" << std::endl;
}

int main() {
    test_directory("data/uf50/uf50-0", 1, 1000, true, "uf50");
    test_directory("data/uuf50/uuf50-0", 1, 1000, false, "uuf50");
    test_directory("data/pigeon-hole/hole", 6, 8, false, "pigeon");
    test_directory("data/hanoi/hanoi", 4, 4, true, "hanoi");
    return 0;
}
