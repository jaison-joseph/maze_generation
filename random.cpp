#include <iostream>
#include <random>
#include <array>
int main() {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> getNum(0,100); // distribution in range [0, 64]
    for (int i = 0 ; i < 100 ; i++) {
        std::cout << getNum(rng) << ", ";
    }
    std::cout << std::endl;
    const float Pm_ = 0.01F;
    const float Pc_ = 0.05F;
    uint_fast32_t rng_upper_bound_ = 1'000'000;
    uint_fast32_t new_pm_ = rng_upper_bound_ * Pm_;
    uint_fast32_t new_pc_ = rng_upper_bound_ * Pc_;
    std::cout << new_pc_ << ", " << new_pm_;

    std::array<bool, 5> foo;
    std::array<bool, 5> bar;
    for (auto& i : foo) {
        i = true;
    }
    for (auto& i : bar) {
        i = true;
    }

    foo[1] = false;

    for (auto& i : foo) {
        if (i) std::cout << "true ";
        else std::cout << "false ";
    }
    std::cout << std::endl;

    for (auto& i : bar) {
        if (i) std::cout << "true ";
        else std::cout << "false ";
    }
    std::cout << std::endl;

    for (auto& j : bar) {    
        for (auto& i : foo) {
            if (!i) {
                bool s = i;
                i = j;
                j = s;
            }
        }
    }

    for (auto& i : foo) {
        if (i) std::cout << "true ";
        else std::cout << "false ";
    }
    std::cout << std::endl;

    for (auto& i : bar) {
        if (i) std::cout << "true ";
        else std::cout << "false ";
    }
    std::cout << std::endl;
}