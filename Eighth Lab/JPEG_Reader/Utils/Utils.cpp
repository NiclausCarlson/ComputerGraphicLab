//
// Created by Nik Carlson on 07.06.2021.
//
#include "Utils.h"

int32_t hex_to_dec(const byte *hex_number, size_t idx1, size_t idx2) {
    std::stringstream ss;
    ss << "0x";
    for (size_t i = idx1; i < idx2; i++) {
        int tmp = static_cast<int>(hex_number[i]);
        if (tmp < 16) ss << "0";
        ss << std::hex << tmp;
    }
    return stoi(ss.str(), nullptr, 16);
}

int32_t hex_to_dec(byte hex_number) {
    byte arr[1] = {hex_number};
    return hex_to_dec(arr, 0, 1);
}

bool get_bit(int32_t pos, byte val) {
    return ((val >> pos) & 1);
}

int32_t bin_to_dec(const bool *bin_number, ptrdiff_t idx1, ptrdiff_t idx2) {
    int32_t res = 0;
    int32_t cur_pow = 1;
    for (ptrdiff_t i = idx2 - 1; i >= idx1; --i) {
        res += bin_number[i] * cur_pow;
        cur_pow *= 2;
    }
    return res;
}
