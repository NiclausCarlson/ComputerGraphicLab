//
// Created by Nik Carlson on 05.05.2021.
//

#ifndef FIFTHLAB_BINARYBRUTEFORCE_H
#define FIFTHLAB_BINARYBRUTEFORCE_H

#include <vector>

class BinaryBruteForce {
private:
    std::vector<int32_t> thresholds;
    std::vector<int32_t> remembered;
public:
    explicit BinaryBruteForce(int64_t classes);

    void add_one();

    void remember();

    std::vector<int32_t> get_remembered();

    std::vector<int32_t> get_thresholds();

    bool is_end();
};


#endif //FIFTHLAB_BINARYBRUTEFORCE_H
