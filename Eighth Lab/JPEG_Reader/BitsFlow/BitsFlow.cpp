//
// Created by Nik Carlson on 09.06.2021.
//

#include "BitsFlow.h"
#include "iostream"

BitsFlow::BitsFlow(std::ifstream *fin)
        : fin(fin), buff(new byte[BUFF_SIZE]), pos(0),
          cur_bit_idx(0), is_end(false) {}


void BitsFlow::read_bits() {
    if (!is_end) {
        pos = 0;
        fin->read(reinterpret_cast<char *>(buff), BUFF_SIZE);
        while (pos < BUFF_SIZE) {
            int32_t ignore = 0;
            if (buff[pos] == SECTION_START) {
                if (pos + 1 < BUFF_SIZE) {
                    if (buff[pos + 1] == END_IMG) {
                        is_end = true;
                        return;
                    } else if (buff[pos + 1] == ZERO) {
                        ignore = 1;
                    } else {
                        throw std::runtime_error("Invalid jpg.");
                    }
                } else {
                    fin->unget();
                    return;
                }
            }
            std::vector<int> a;
            for (ptrdiff_t i = BITS - 1; i >= 0; --i) {
                bits.push_back(get_bit(i, buff[pos]));
                a.push_back(bits.back());
            }
            pos += ignore + 1;
        }
    }
}

BitResult BitsFlow::next_bit() {
    auto f = [&]() {
        bool res = bits[cur_bit_idx++];
        if (cur_bit_idx >= bits.size()) {
            cur_bit_idx = 0;
            bits.clear();
        }
        return res;
    };
    if (!bits.empty()) return BitResult(OK, f());
    read_bits();
    if (is_end && bits.empty()) return BitResult(END, false);
    else if (!is_end && bits.empty()) throw std::runtime_error("Invalid jpg.");
    else return BitResult(OK, f());
}

BitsFlow::~BitsFlow() {
    delete[] buff;
}
