//
// Created by Nik Carlson on 05.05.2021.
//

#include "BinaryBruteForce.h"

BinaryBruteForce::BinaryBruteForce(int64_t classes) : thresholds(std::vector<int32_t>(classes + 1)),
                                                      remembered(std::vector<int32_t>(classes + 1))
                                                      {
    int64_t idx = 1;
    for (auto &i: thresholds) {
        i = idx++;
    }
    thresholds.back() = 256;
}

void BinaryBruteForce::add_one() {
    int64_t n = 1;
    int64_t pos = 1;
    for (auto i = thresholds.rbegin() + 1; i != thresholds.rend(); ++i) {
        *i += n;
        if (*i >= 256 - pos + 1) *i = *i - 256 + pos - 1;
        else {
            n = 0;
            break;
        }
        ++pos;
    }
    if (n == 0) {
        for (auto i = thresholds.size() - pos; i < thresholds.size() - 1; ++i) {
            thresholds[i] = thresholds[i - 1] + 1;
        }
    }
}

void BinaryBruteForce::remember() {
    std::copy(thresholds.begin(), thresholds.end(), remembered.begin());
}

bool BinaryBruteForce::is_end() {
    int32_t idx = 256;
    for (auto it = thresholds.crbegin(); it != thresholds.crend(); ++it) {
        if (*it != idx--) return false;
    }
    return true;
}

std::vector<int32_t> BinaryBruteForce::get_thresholds() {
    return thresholds;
}

std::vector<int32_t> BinaryBruteForce::get_remembered() {
    return remembered;
}
