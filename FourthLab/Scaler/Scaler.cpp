//
// Created by Nik Carlson on 11.04.2021.
//

#include "Scaler.h"

Scaler::Scaler(Method method, int32_t r_height, int32_t r_width, double dx, double dy) :
        method(method), r_height(r_height), r_width(r_width), dx(dx), dy(dy), b(0.), c(0.5) {
}

Scaler::Scaler(Method method, int32_t r_height, int32_t r_width, double dx, double dy, double b, double c) :
        method(method), r_height(r_height), r_width(r_width), dx(dx), dy(dy), b(b), c(c) {
}

Image *Scaler::scale(const Image *img) {
    auto *image = img->get_image();
    size_t size = r_height * r_width;
    byte *scaled_image = new(std::nothrow) byte[size];
    if (scaled_image == nullptr) {
        throw std::runtime_error("Can't allocate memory to scale");
    }
    double k_1 = 1. * r_height / img->get_height();
    double k_2 = 1. * r_width / img->get_width();
    if (method == Near) {
        near(image, scaled_image, img->get_width(), k_1, k_2);
    } else if (method == Bilinear) {
        bilinear(image, scaled_image, img->get_width(), img->get_height(), k_1, k_2);
    } else if (method == Lanczos3 || method == BC) {
        size_t sc_b_x_size = img->get_height() * r_width;
        byte *scaled_by_x = new(std::nothrow) byte[sc_b_x_size];
        if (scaled_by_x == nullptr) {
            delete[] scaled_image;
            throw std::runtime_error("Can't allocate memory to scale");
        }
        if (method == Lanczos3) {
            lanczos3(image, scaled_by_x, img->get_width(), img->get_height(), k_1, k_2, true);
            lanczos3(scaled_by_x, scaled_image, r_width, img->get_height(), k_1, k_2, false);
        } else {
            bc(image, scaled_by_x, img->get_width(), img->get_height(), k_1, k_2, true);
            bc(scaled_by_x, scaled_image, r_width, img->get_height(), k_1, k_2, false);
        }
        delete[] scaled_by_x;
    }
    return new Image(scaled_image, size, r_height, r_width, img->get_type());
}

void Scaler::near(byte *&original, byte *&scaled, size_t width, double k_1, double k_2) const {
    for (size_t i = 0; i < r_height; ++i) {
        for (size_t j = 0; j < r_width; ++j) {
            size_t idx_1 = std::floor(i / k_1);
            size_t idx_2 = std::floor(j / k_2);
            scaled[i * r_width + j] = original[idx_1 * width + idx_2];
        }
    }
}

void Scaler::bilinear(byte *&original, byte *&scaled, size_t width, size_t height, double k_1, double k_2) const {
    for (size_t i = 0; i < r_height; ++i) {
        for (size_t j = 0; j < r_width; ++j) {
            double center_y = i / k_1;
            double center_x = j / k_2;
            size_t first_pixel_x = std::floor(center_x);
            size_t second_pixel_x = std::floor(center_x) + 1;
            size_t first_pixel_y = std::floor(center_y);
            size_t second_pixel_y = std::floor(center_y) + 1;
            double weight_x = second_pixel_x - center_x;
            double weight_y = second_pixel_y - center_y;
            second_pixel_x = std::fmin(second_pixel_x, width - 1);
            second_pixel_y = std::fmin(second_pixel_y, height - 1);

            double f_1 = (1. - weight_x) * original[second_pixel_y * width + second_pixel_x] +
                         weight_x * original[second_pixel_y * width + first_pixel_x];
            double f_2 = weight_x * original[first_pixel_y * width + first_pixel_x] +
                         (1. - weight_x) * original[first_pixel_y * width + second_pixel_x];

            scaled[i * r_width + j] = std::fmin(255, std::fmax(0., (1. - weight_y) * f_1 + weight_y * f_2));
        }
    }
}

void Scaler::lanczos3(byte *&original, byte *&scaled, size_t width, size_t height, double k_1, double k_2,
                      bool coord) const {
    const int a = 3;
    auto lanczos = [a](double x) {
        if (x == 0) return 1.;
        if (-a <= x && x < a) {
            return a * std::sin(M_PI * x) * sin(M_PI * x / a) / std::pow(M_PI * x, 2);
        }
        return 0.;
    };
    if (coord) { // по x
        for (size_t i = 0; i < height; ++i) {
            for (size_t j = 0; j < r_width; ++j) {
                double x = j / k_2;
                ptrdiff_t lower_bound = std::floor(x) - a + 1;
                ptrdiff_t upper_bound = std::floor(x) + a;
                double result = 0.;
                for (ptrdiff_t idx = lower_bound; idx != upper_bound + 1; ++idx) {
                    size_t cur_idx = std::max(0, std::min(idx, (ptrdiff_t) (width - 1)));
                    result += original[i * width + cur_idx] * lanczos(x - idx);
                }
                scaled[i * r_width + j] = std::max(0., std::min(result, 255.));
            }
        }
    } else {
        for (size_t i = 0; i < r_height; ++i) {
            for (size_t j = 0; j < r_width; ++j) {
                double x = i / k_1;
                ptrdiff_t lower_bound = std::floor(x) - a + 1;
                ptrdiff_t upper_bound = std::floor(x) + a;
                double result = 0.;
                for (ptrdiff_t idx = lower_bound; idx != upper_bound + 1; ++idx) {
                    size_t cur_idx = std::max(0, std::min(idx, (ptrdiff_t) (height - 1)));
                    result += original[cur_idx * r_width + j] * lanczos(x - idx);
                }
                scaled[i * r_width + j] = std::max(0., std::min(result, 255.));
            }
        }
    }
}

void Scaler::bc(byte *&original, byte *&scaled, size_t width, size_t height, double k_1, double k_2, bool coord) const {
    auto p_d = [this](double d, double p_0, double p_1, double p_2, double p_3) {
        double b = this->b;
        double c = this->c;
        return (((-1. / 6) * b - c) * p_0 +
                ((-3. / 2) * b - c + 2.) * p_1 +
                ((3. / 2) * b + c - 2.) * p_2 +
                ((1. / 6) * b + c) * p_3) * std::pow(d, 3) +
               (((1. / 2) * b + 2 * c) * p_0 +
                (2 * b + c - 3) * p_1 +
                ((-5. / 2) * b - 2 * c + 3) * p_2 - c * p_3) * std::pow(d, 2) +
               (((-1. / 2) * b - c) * p_0 +
                ((1. / 2) * b + c) * p_2) * d +
               ((1. / 6) * b * p_0 +
                ((-1. / 3) * b + 1) * p_1 +
                (1. / 6) * b * p_2);
    };
    std::vector<double> p(4);
    if (coord) { // по x
        for (size_t i = 0; i < height; ++i) {
            for (size_t j = 0; j < r_width; ++j) {
                double x = j / k_2;
                ptrdiff_t lower_bound = std::floor(x) - 1;
                ptrdiff_t upper_bound = std::floor(x) + 2;
                double d = std::fabs(x - lower_bound - 1);
                size_t iidx = 0;
                for (ptrdiff_t idx = lower_bound; idx != upper_bound + 1; ++idx) {
                    size_t cur_idx = std::max(0, std::min(idx, (ptrdiff_t) (width - 1)));
                    p[iidx++] = original[i * width + cur_idx];
                }
                scaled[i * r_width + j] = std::max(0., std::min(p_d(d, p[0], p[1], p[2], p[3]), 255.));
            }
        }
    } else {
        for (size_t i = 0; i < r_height; ++i) {
            for (size_t j = 0; j < r_width; ++j) {
                double x = i / k_1;
                ptrdiff_t lower_bound = std::floor(x) - 1;
                ptrdiff_t upper_bound = std::floor(x) + 2;
                double d = std::fabs(x - lower_bound - 1);
                size_t iidx = 0;
                for (ptrdiff_t idx = lower_bound; idx != upper_bound + 1; ++idx) {
                    size_t cur_idx = std::max(0, std::min(idx, (ptrdiff_t) (height - 1)));
                    p[iidx++] = original[cur_idx * r_width + j];
                }
                scaled[i * r_width + j] = std::max(0., std::min(p_d(d, p[0], p[1], p[2], p[3]), 255.));
            }
        }
    }
}
