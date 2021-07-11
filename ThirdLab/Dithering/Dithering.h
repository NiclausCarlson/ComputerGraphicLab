//
// Created by Nik Carlson on 27.03.2021.
//

#ifndef THIRDLAB_DITHERING_H
#define THIRDLAB_DITHERING_H


#include "../Image/Image.h"
#include <vector>
#include <algorithm>
#include <functional>
#include <random>
#include <array>

class Dithering {
private:
    const double BITS = 255.;
    const double GAMMA = 0;

    std::vector<std::pair<double, double>> border;

    void set_border(int32_t bits);

    [[nodiscard]] double from_gamma(double value) const;

    [[nodiscard]] double to_gamma(double value) const;

    void dither(double *&arr, size_t height, size_t width,
                const std::function<double(size_t, size_t, double, double, double)> &f);

    static void divide(std::vector<double> &v, double divider);

    static void set_result(double &result, double &delta, double value, double left, double right);

public:

    explicit Dithering(double gamma);

    void convert(Image *&image, int32_t dithering, int32_t bits);
};


#endif //THIRDLAB_DITHERING_H
