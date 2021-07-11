//
// Created by Nik Carlson on 07.06.2021.
//

#ifndef EIHTH_LAB_UTILS_H
#define EIHTH_LAB_UTILS_H

#include "../../Image/Image.h"
#include <sstream>

int32_t hex_to_dec(const byte *hex_number, size_t idx1, size_t idx2);

int32_t hex_to_dec(byte hex_number);

bool get_bit(int32_t pos, byte val);

int32_t bin_to_dec(const bool *bin_number, ptrdiff_t idx1, ptrdiff_t idx2);

#endif //EIHTH_LAB_UTILS_H
