//
// Created by Nik Carlson on 11.04.2021.
//

#ifndef FOURTHLAB_SCALER_H
#define FOURTHLAB_SCALER_H

#define _USE_MATH_DEFINES
#include "../Image/Image.h"
#include <cmath>
#include <algorithm>
#include <vector>

enum Method {
    Near,
    Bilinear,
    Lanczos3,
    BC
};

class Scaler {
public:
    Scaler(Method method, int32_t r_height, int32_t r_width, double dx, double dy);

    Scaler(Method method, int32_t r_height, int32_t r_width, double dx, double dy, double b, double c);

    Image *scale(const Image *img);

private:
    Method method;
    int32_t r_height;
    int32_t r_width;
    double dx;
    double dy;
    double b;
    double c;

    void near(byte *&original, byte *&scaled, size_t width, double k_1, double k_2) const;

    void bilinear(byte *&original, byte *&scaled, size_t width, size_t height,  double k_1, double k_2) const;

    void lanczos3(byte *&original, byte *&scaled, size_t width, size_t height, double k_1, double k_2, bool coord) const;

    void bc(byte *&original, byte *&scaled, size_t width, size_t height, double k_1, double k_2, bool coord) const;

};


#endif //FOURTHLAB_SCALER_H
