//
// Created by Nik Carlson on 25.02.2021.
//

#ifndef SECONDLAB_CONVERTER_H
#define SECONDLAB_CONVERTER_H

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "../Image/Image.h"

enum COLOR_SPACE {
    RGB,
    HSL,
    HSV,
    YCbCrdot601,
    YCbCrdor709,
    YCoCg,
    CMY
};

class Converter {
public:
    void solve(Image *&image, COLOR_SPACE input_color_space, COLOR_SPACE output_color_space);

private:
    void from_rgb(byte *&img, size_t size, COLOR_SPACE to);

    void to_rgb(byte *&img, size_t size, COLOR_SPACE from);

    void from_hsl(byte *&img, size_t size);

    void to_hsl(byte *&img, size_t size);

    void from_hsv(byte *&img, size_t size);

    void to_hsv(byte *&img, size_t size);

    void from_YCbCr(byte *&img, size_t size, double k_r, double k_g, double k_b);

    void to_YCbCr(byte *&img, size_t size, double k_r, double k_g, double k_b);

    void from_YCoCg(byte *&img, size_t size);

    void to_YCoCg(byte *&img, size_t size);

    void cmy(byte *&img, size_t size);
};


#endif //SECONDLAB_CONVERTER_H
