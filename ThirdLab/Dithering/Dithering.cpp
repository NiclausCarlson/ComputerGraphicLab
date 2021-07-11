//
// Created by Nik Carlson on 27.03.2021.
//

#include "Dithering.h"

Dithering::Dithering(double gamma) : GAMMA(gamma) {
}

void Dithering::convert(Image *&image, int32_t dithering, int32_t bits) {
    set_border(bits);
    size_t height = image->get_height();
    size_t width = image->get_width();
    std::vector<double> right_coefs(2);
    std::vector<double> right_errors(2), actual_err(width), next_error(width), next_next_error(width);
    std::vector<double> next_coefs, next_next_coefs;
    double *arr = image->get_d_image();
    if (dithering == 0) {
        dither(arr, height, width, [](size_t i, size_t j, double value, double left, double right) { return value; });
    } else if (dithering == 1) {
        const size_t N = 8;
        std::vector<std::vector<double>> matrix = {
                {0.,  48., 12., 60., 3.,  51., 15., 63.},
                {32., 16., 44., 28., 35., 19., 47., 31.},
                {8.,  56., 4.,  52., 11., 59., 7.,  55.},
                {40., 24., 36., 20., 43., 27., 39., 23.},
                {2.,  50., 14., 62., 1.,  49., 13., 61.},
                {34., 18., 46., 30., 33., 17., 45., 29.},
                {10., 58., 6.,  54., 9.,  57., 5.,  53.},
                {42., 26., 38., 22., 41., 25., 37., 21.}};
        for (auto &i: matrix) {
            for (auto &j: i) j = (j + 0.5) / 64. - 0.5;
        }
        dither(arr, height, width,
               [&](size_t i, size_t j, double value, double left, double right) {
                   double d = right - left;
                   return value + d * (matrix[j % N][i % N]);
               }
        );
    } else if (dithering == 2) {
        std::random_device rd;
        std::mt19937_64 gen(rd());
        dither(arr, height, width,
               [&gen](size_t i, size_t j, double value, double left, double right) {
                   std::uniform_real_distribution<> uid(-(right - left) / 2, (right - left) / 2);
                   double err = uid(gen);
                   return value + err;
               });
    } else if (dithering == 3) {
        double right_error = 0., right_k = 7. / 16, left_k = 3. / 16, down_k = 5. / 16, right_d_k = 1. / 16;
        dither(arr, height, width,
               [&](size_t i, size_t j, double value, double left, double right) {
                   double result, delta;
                   value += right_error + actual_err[j];
                   actual_err[j] = 0.;
                   set_result(result, delta, value, left, right);
                   next_error[j] += down_k * delta;
                   if (j != 0) next_error[j - 1] += left_k * delta;
                   if (j != width - 1) {
                       right_error = right_k * delta;
                       next_error[j + 1] += right_d_k * delta;
                   } else {
                       std::swap(actual_err, next_error);
                       right_error = 0.;
                   }
                   return result;
               });
    } else if (dithering == 4) {
        right_coefs = {7., 5.};
        divide(right_coefs, 48.);

        next_coefs = {3., 5., 7., 5., 3.};
        divide(next_coefs, 48.);

        next_next_coefs = {1., 3., 5., 3., 1.};
        divide(next_next_coefs, 48.);

        dither(arr, height, width,
               [&](size_t i, size_t j, double value, double left, double right) {
                   double result, delta;
                   value += right_errors[0] + actual_err[j];
                   actual_err[j] = 0.;
                   set_result(result, delta, value, left, right);

                   std::ptrdiff_t left_idx = j - 2;
                   std::ptrdiff_t right_idx = j + 2;
                   if (left_idx < 0) left_idx = 0;
                   if (right_idx >= width - 1) right_idx = width - 1;
                   size_t next_err_idx = left_idx - j + 2;
                   for (std::size_t idx = left_idx; idx <= right_idx; ++idx) {
                       next_error[idx] += next_coefs[next_err_idx] * delta;
                       next_next_error[idx] += next_next_coefs[next_err_idx] * delta;
                       ++next_err_idx;
                   }
                   if (width - j == 1) {
                       std::fill(right_errors.begin(), right_errors.end(), 0.);
                       std::swap(actual_err, next_error);
                       std::swap(next_error, next_next_error);
                   } else {
                       right_errors[0] = right_errors[1] + right_coefs[0] * delta;
                       right_errors[1] = right_coefs[1] * delta;
                   }
                   return result;
               });

    } else if (dithering == 5) {
        right_coefs = {5., 3.};
        divide(right_coefs, 32.);

        next_coefs = {2., 4., 5., 4., 2.};
        divide(next_coefs, 32.);

        next_next_coefs = {2., 3., 2.};
        divide(next_next_coefs, 32.);

        dither(arr, height, width,
               [&](size_t i, size_t j, double value, double left, double right) {
                   double result, delta;
                   value += right_errors[0] + actual_err[j];
                   actual_err[j] = 0.;
                   set_result(result, delta, value, left, right);

                   std::ptrdiff_t left_idx = j - 2;
                   std::ptrdiff_t right_idx = j + 2;
                   if (left_idx < 0) left_idx = 0;
                   if (right_idx >= width) right_idx = width - 1;
                   size_t next_err_idx = left_idx - j + 2;
                   for (size_t idx = left_idx; idx <= right_idx; ++idx) {
                       next_error[idx] += next_coefs[next_err_idx++] * delta;
                   }
                   left_idx = j - 1;
                   right_idx = j + 1;
                   if (left_idx < 0) left_idx = 0;
                   if (right_idx >= width) right_idx = width - 1;
                   next_err_idx = left_idx - j + 1;
                   for (size_t idx = left_idx; idx <= right_idx; ++idx) {
                       next_next_error[idx] += next_next_coefs[next_err_idx++] * delta;
                   }
                   if (width - j == 1) {
                       std::fill(right_errors.begin(), right_errors.end(), 0.);
                       std::swap(actual_err, next_error);
                       std::swap(next_error, next_next_error);
                   } else {
                       right_errors[0] = right_errors[1] + right_coefs[0] * delta;
                       right_errors[1] = right_coefs[1] * delta;
                   }
                   return result;
               });

    } else if (dithering == 6) {
        right_coefs = {1., 1.};
        divide(right_coefs, 8.);

        next_coefs = {1., 1., 1.};
        divide(next_coefs, 8.);

        dither(arr, height, width,
               [&](size_t i, size_t j, double value, double left, double right) {
                   double result, delta;
                   value += right_errors[0] + actual_err[j];
                   actual_err[j] = 0.;
                   set_result(result, delta, value, left, right);

                   std::ptrdiff_t left_idx = j - 1;
                   std::ptrdiff_t right_idx = j + 1;
                   if (left_idx < 0) left_idx = 0;
                   if (right_idx >= width - 1) right_idx = width - 1;
                   size_t next_err_idx = left_idx - j + 1;
                   for (size_t idx = left_idx; idx <= right_idx; ++idx) {
                       next_error[idx] += next_coefs[next_err_idx] * delta;
                   }
                   next_next_error[j] += next_coefs[0] * delta;
                   if (width - j == 1) {
                       std::fill(right_errors.begin(), right_errors.end(), 0.);
                       std::swap(actual_err, next_error);
                       std::swap(next_error, next_next_error);
                   } else {
                       right_errors[0] = right_errors[1] + delta * right_coefs[0];
                       right_errors[1] = delta * right_coefs[0];
                   }
                   return result;
               });
    } else if (dithering == 7) {
        const size_t N = 4;
        const double SQUARE = N * N;
        std::vector<std::vector<double>> matrix{
                {7.,  13., 11., 4.},
                {12., 16., 14., 8.},
                {10., 15., 6.,  2.},
                {5.,  9.,  3.,  1.}
        };
        for (auto &i: matrix) {
            for (auto &j: i) j = (j - 0.5) / SQUARE - 0.5;
        }
        dither(arr, height, width,
               [&](size_t i, size_t j, double value, double left, double right) {
                   double d = right - left;
                   return value + d * matrix[i % N][j % N];
               }
        );
    }
}

void Dithering::set_border(int32_t bits) {
    // генерирует числа на равном расстоянии в линейном пространстве и переводит их в гамма-скоректированное пространство
    border.clear();
    double q = std::pow(2, bits);
    double step = 1. / (q - 1); // делим отрезок [0,1] на 2^n - 1 равных частей
    double cur = 0.;
    for (size_t i = 0; i < q; ++i) {
        border.emplace_back(to_gamma(cur), cur);
        cur += step;
    }
}

double Dithering::to_gamma(double value) const {
    if (GAMMA == 0) {
        if (value <= 0.0031308) return value * 12.92;
        else return (1.055 * std::pow(value, 1 / 2.4) - 0.055);
    } else return std::pow(value, 1 / GAMMA);
}

double Dithering::from_gamma(double value) const {
    if (GAMMA == 0) {
        if (value <= 0.04045) return value / 12.92;
        else return (std::pow((value + 0.055) / 1.055, 2.4));
    } else return std::pow(value, GAMMA);
}

void Dithering::dither(double *&arr, size_t height, size_t width,
                       const std::function<double(size_t, size_t, double, double, double)> &f) {
    auto comp = [](const std::pair<double, double> &p1, const std::pair<double, double> &p2) -> bool {
        return p1.first < p2.first;
    };
    std::pair<double, double> e(1., 1.);
    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {
            // значение в гамма-скоректированном пространстве
            double value = arr[i * width + j] / BITS;
            // значение в линейном пространстве
            double linear_value = from_gamma(value);
            // ближайшие значения в гамма-скорректированно пространстве
            auto left_right = std::equal_range(border.begin(), border.end(), std::make_pair(value, linear_value), comp);
            // ближайшие значения в линейном пространстве
            auto left = left_right.first == border.end() ? e : *left_right.first;
            auto right = left_right.second == border.end() ? e : *left_right.second;
            if (left == right && left_right.first != border.begin()) left = *(left_right.first - 1);
            if (left == right && left_right.second == border.begin()) right = *(left_right.second + 1);
            // применяю функцию ошибки
            double linear_updated_value = f(i, j, linear_value, left.second, right.second);
            // смотрю, какой из предпосчитаных значений ближе
            if (std::fabs(left.second - linear_updated_value) < std::fabs(right.second - linear_updated_value)) {
                arr[i * width + j] = left.second * BITS;
            } else {
                arr[i * width + j] = right.second * BITS;
            }
        }
    }
}

void Dithering::divide(std::vector<double> &v, double divider) {
    for (auto &i: v) i = i / divider;
}

void Dithering::set_result(double &result, double &delta, double value, double left, double right) {
    if (std::fabs(value - left) < std::fabs(value - right)) {
        result = left;
        delta = value - left;
    } else {
        result = right;
        delta = value - right;
    }
}
