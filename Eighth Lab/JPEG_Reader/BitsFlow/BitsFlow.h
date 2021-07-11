//
// Created by Nik Carlson on 09.06.2021.
//

#ifndef EIHTH_LAB_BITSFLOW_H
#define EIHTH_LAB_BITSFLOW_H


#include <fstream>
#include <vector>
#include <algorithm>
#include "../Constants.h"
#include "../Utils/Utils.h"
#include "../../Image/Image.h"
#include "ReturnValue.h"

class BitsFlow {
private:
    const int32_t BITS = 8;
    const size_t BUFF_SIZE = 256;
    byte *buff;
    std::ifstream *fin;
    size_t pos, cur_bit_idx;
    std::vector<bool> bits;
    bool is_end;

    void read_bits();

public:
    explicit BitsFlow(std::ifstream *fin);

    BitResult next_bit();

    ~BitsFlow();
};


#endif //EIHTH_LAB_BITSFLOW_H
