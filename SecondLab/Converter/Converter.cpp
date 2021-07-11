//
// Created by Nik Carlson on 25.02.2021.
//

#include "Converter.h"

void Converter::solve(Image *&image, COLOR_SPACE input_color_space, COLOR_SPACE output_color_space) {
    if (input_color_space != output_color_space) {
        byte *img = image->get_image();
        size_t size = image->get_size();
        to_rgb(img, size, input_color_space);
        from_rgb(img, size, output_color_space);
    }
}

void Converter::to_rgb(byte *&img, size_t size, COLOR_SPACE from) {
    switch (from) {
        case RGB:
            break;
        case HSL:
            from_hsl(img, size);
            break;
        case HSV:
            from_hsv(img, size);
            break;
        case YCbCrdot601:
            from_YCbCr(img, size, 0.299, 0.587, 0.114);
            break;
        case YCbCrdor709:
            from_YCbCr(img, size, 0.2126, 0.7152, 0.0722);
            break;
        case YCoCg:
            from_YCoCg(img, size);
            break;
        case CMY:
            cmy(img, size);
            break;
        default:
            throw std::runtime_error("Unexpected color space");
    }
}

void Converter::from_rgb(byte *&img, size_t size, COLOR_SPACE to) {
    switch (to) {
        case RGB:
            break;
        case HSL:
            to_hsl(img, size);
            break;
        case HSV:
            to_hsv(img, size);
            break;
        case YCbCrdot601:
            to_YCbCr(img, size, 0.299, 0.587, 0.114);
            break;
        case YCbCrdor709:
            to_YCbCr(img, size, 0.2126, 0.7152, 0.0722);
            break;
        case YCoCg:
            to_YCoCg(img, size);
            break;
        case CMY:
            cmy(img, size);
            break;
        default:
            throw std::runtime_error("Unexpected color space");
    }
}

void Converter::from_hsl(byte *&img, size_t size) {
    for (size_t i = 0; i < size; i += 3) {
        double h = img[i] / 255.0;
        double s = img[i + 1] / 255.0;
        double l = img[i + 2] / 255.0;
        double q = l < 0.5 ? l * (1.0 + s) : l + s - l * s;
        double p = 2.0 * l - q;
        double t[] = {h + 1.0 / 3, h, h - 1.0 / 3};
        for (size_t j = 0; j < 3; ++j) {
            if (t[j] < 0) t[j] += 1.0;
            if (t[j] > 1) t[j] -= 1.0;
            if (t[j] < 1.0 / 6)
                img[i + j] = (p + ((q - p) * 6.0 * t[j])) * 255.0;
            else if (t[j] >= 1.0 / 6 && t[j] < 1.0 / 2)
                img[i + j] = q * 255.0;
            else if (t[j] >= 1.0 / 2 && t[j] < 2.0 / 3)
                img[i + j] = (p + ((q - p) * (2.0 / 3 - t[j]) * 6.0)) * 255.0;
            else
                img[i + j] = p * 255.0;
        }
    }
}

void Converter::to_hsl(byte *&img, size_t size) {
    for (size_t i = 0; i < size; i += 3) {
        double r = img[i] / 255.0;
        double g = img[i + 1] / 255.0;
        double b = img[i + 2] / 255.0;
        double min = std::min(std::min(r, g), b);
        double max = std::max(std::max(r, g), b);
        double h = 0, s = 0;
        if (min != max) {
            if (max == r && g >= b) h = 60.0 * (g - b) / (max - min);
            else if (max == r && g < b) h = 60.0 * (g - b) / (max - min) + 360.0;
            else if (max == g) h = 60.0 * (b - r) / (max - min) + 120.0;
            else if (max == b) h = 60.0 * (r - g) / (max - min) + 240.0;
            s = (1.0 * (max - min)) / (1 - std::abs(1 - (max + min)));
        }
        double l = (max + min) / 2.0;
        img[i] = h * 255.0 / 360.0;
        img[i + 1] = s * 255.0;
        img[i + 2] = l * 255.0;
    }
}

void Converter::from_hsv(byte *&img, size_t size) {
    for (size_t i = 0; i < size; i += 3) {
        double h = (img[i] / 255.0) * 360.0;
        double s = (img[i + 1] / 255.0) * 100.0;
        double v = (img[i + 2] / 255.0) * 100.0;
        int h_i = static_cast<int>(std::floor(h / 60.0)) % 6;
        double v_min = (100.0 - s) * v / 100.0;
        double a = (v - v_min) * (static_cast<int>(h) % 60) / 60.0;
        double v_inc = v_min + a;
        double v_dec = v - a;
        double d = 255.0 / 100.0;
        switch (h_i) {
            case 0:
                img[i] = v * d;
                img[i + 1] = v_inc * d;
                img[i + 2] = v_min * d;
                break;
            case 1:
                img[i] = v_dec * d;
                img[i + 1] = v * d;
                img[i + 2] = v_min * d;
                break;
            case 2:
                img[i] = v_min * d;
                img[i + 1] = v * d;
                img[i + 2] = v_inc * d;
                break;
            case 3:
                img[i] = v_min * d;
                img[i + 1] = v_dec * d;
                img[i + 2] = v * d;
                break;
            case 4:
                img[i] = v_inc * d;
                img[i + 1] = v_min * d;
                img[i + 2] = v * d;
                break;
            case 5:
                img[i] = v * d;
                img[i + 1] = v_min * d;
                img[i + 2] = v_dec * d;
                break;
            default:
                throw std::logic_error("Lol, how did you get the remainder of 6 more than five.");
        }
    }
}

void Converter::to_hsv(byte *&img, size_t size) {
    for (size_t i = 0; i < size; i += 3) {
        double r = img[i] / 255.0;
        double g = img[i + 1] / 255.0;
        double b = img[i + 2] / 255.0;
        double min = std::min(std::min(r, g), b);
        double max = std::max(std::max(r, g), b);
        double h = 0, s = 0;
        if (min != max) {
            if (max == r && g >= b) h = 60.0 * (g - b) / (max - min);
            else if (max == r && g < b) h = 60.0 * (g - b) / (max - min) + 360.0;
            else if (max == g) h = 60.0 * (b - r) / (max - min) + 120.0;
            else if (max == b) h = 60.0 * (r - g) / (max - min) + 240.0;
            s = max == 0 ? 0 : 1.0 - min / max;
        }
        img[i] = h * 255.0 / 360.0;
        img[i + 1] = s * 255.0;
        img[i + 2] = max * 255.0;
    }
}

void Converter::from_YCoCg(byte *&img, size_t size) {
    for (size_t i = 0; i < size; i += 3) {
        double y = img[i] / 255.0;
        double co = img[i + 1] / 255.0 - 0.5;
        double cg = img[i + 2] / 255.0 - 0.5;
        img[i] = std::min(std::max(y + co - cg, 0.0), 1.0) * 255.0;
        img[i + 1] = std::min(std::max(y + cg, 0.0), 1.0) * 255.0;
        img[i + 2] = std::min(std::max(y - co - cg, 0.0), 1.0) * 255.0;
    }
}

void Converter::to_YCoCg(byte *&img, size_t size) {
    for (size_t i = 0; i < size; i += 3) {
        double r = img[i] / 255.0;
        double g = img[i + 1] / 255.0;
        double b = img[i + 2] / 255.0;
        img[i] = (r / 4.0 + g / 2.0 + b / 4.0) * 255.0;
        img[i + 1] = ((r / 2.0 - b / 2.0) + 0.5) * 255.0;
        img[i + 2] = ((-r / 4.0 + g / 2.0 - b / 4.0) + 0.5) * 255.0;
    }
}

void Converter::cmy(byte *&img, size_t size) {
    for (size_t i = 0; i < size; i += 3) {
        img[i] = (1.0 - img[i] / 255.0) * 255.0;
        img[i + 1] = (1.0 - img[i + 1] / 255.0) * 255.0;
        img[i + 2] = (1.0 - img[i + 2] / 255.0) * 255.0;
    }
}

void Converter::from_YCbCr(byte *&img, size_t size, double k_r, double k_g, double k_b) {
    for (size_t i = 0; i < size; i += 3) {
        double y = img[i];
        double cb = img[i + 1];
        double cr = img[i + 2];
        double y_ = (y - 16) / 219;
        double pb = (cb - 128) / 224;
        double pr = (cr - 128) / 224;
        double r = y_ + (2 - 2 * k_r) * pr;
        double g = y_ + (2 * k_b - 2) * (k_b / k_g) * pb + (2 * k_r - 2) * (k_r / k_g) * pr;
        double b = y_ + (2 - 2 * k_b) * pb;
        img[i] = std::max(0.0, std::min(r, 1.0)) * 255.0;
        img[i + 1] = std::max(0.0, std::min(g, 1.0)) * 255.0;
        img[i + 2] = std::max(0.0, std::min(b, 1.0)) * 255.0;
    }
}

void Converter::to_YCbCr(byte *&img, size_t size, double k_r, double k_g, double k_b) {
    for (size_t i = 0; i < size; i += 3) {
        double r = img[i] / 255.0;
        double g = img[i + 1] / 255.0;
        double b = img[i + 2] / 255.0;
        double y_ = k_r * r + k_g * g + k_b * b;
        double pb = 1.0 / 2 * ((b - y_) / (1 - k_b));
        double pr = 1.0 / 2 * ((r - y_) / (1 - k_r));
        img[i] = 16 + y_ * 219;
        img[i + 1] = 128 + pb * 224;
        img[i + 2] = 128 + pr * 224;
    }
}
