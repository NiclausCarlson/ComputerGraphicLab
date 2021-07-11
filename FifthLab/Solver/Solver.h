//
// Created by Nik Carlson on 27.04.2021.
//

#ifndef FIFTHLAB_SOLVER_H
#define FIFTHLAB_SOLVER_H

#include <vector>
#include <array>
#include "../Image/Image.h"
#include "Binary brute force/BinaryBruteForce.h"

class Solver {
private:
    std::array<int64_t, 256> barGraph{};
    std::vector<int64_t> prefix_p, prefix_fp;

    void prepare(Image *const &img);

    std::vector<int32_t> otsu(int32_t classes);

    void paint(std::vector<int32_t>& thresholds, int32_t classes, Image *&img);

public:
    void solve(Image *&img, int32_t classes);

    Solver();
};


#endif //FIFTHLAB_SOLVER_H
