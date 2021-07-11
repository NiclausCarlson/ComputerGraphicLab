//
// Created by Nik Carlson on 20.02.2021.
//

#include "Solver.h"

Solver::Solver(const char *input_path, const char *output_path, int mode) :
        img(nullptr), input_path(input_path), output_path(output_path), mode(mode) {}

void Solver::solve() {
    read_file();
    switch (mode) {
        case 0:
            inverse();
            break;
        case 1:
            horizontal();
            break;
        case 2:
            vertical();
            break;
        case 3:
            rotate_down();
            break;
        case 4:
            rotate_up();
            break;
        default:
            throw "Invalid mode.";
    }
    write_wile();
}

void Solver::read_file() {
    std::ifstream in;
    in.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        in.open(input_path, std::ios::binary);
        std::string tmp;
        in >> tmp;
        if (tmp[0] != 'P' || !isdigit(tmp[1]) || tmp.size() > 2) throw "Invalid file.";
        type = stoi(tmp.substr(1));
        if (type != 5 && type != 6) throw "Invalid format version.";
        in >> tmp;
        try {
            width = stoi(tmp);
            in >> tmp;
            height = stoi(tmp);
            in >> tmp;
            max = stoi(tmp);
            size = height * width;
        } catch (const std::logic_error &e) {
            throw "Invalid images img.";
        }
        if (max != 255) throw "Invalid max.";
        if (type == 6) {
            pixel_size = 3;
            size *= pixel_size;
        } else pixel_size = 1;
        img = new image[size];
        if (img == nullptr) throw std::runtime_error("Failed to allocate memory");
        width_p = width * pixel_size;
        in.get();
        in.read(reinterpret_cast<char *>(img), size);
    } catch (const std::ifstream::failure &e) {
        throw "Exception opening/reading file.";
    }
    in.close();
    if (size == 0 || img == nullptr) throw "Img is empty";
}

void Solver::write_wile() {
    std::ofstream of;
    of.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    try {
        of.open(output_path, std::ifstream::binary);
        of << 'P' << type << '\n'
           << width << ' ' << height << '\n'
           << max << '\n';
        of.write(reinterpret_cast<const char *>(img), size);
    } catch (const std::ofstream::failure &e) {
        throw "Exception creating/opening/writing file.";
    }
    of.close();
}

void Solver::inverse() {
    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width_p; ++j)
            img[width_p * i + j] = ~img[width_p * i + j];
    }
}

void Solver::horizontal() {
    size_t w = std::floor(width * pixel_size / 2);
    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < w; j += pixel_size)
            for (size_t k = 0; k < pixel_size; ++k)
                std::swap(img[width_p * i + j + k],
                          img[width_p * (i + 1) - j - pixel_size + k]);
    }
}

void Solver::vertical() {
    size_t new_height = std::floor(height / 2);
    for (size_t i = 0; i < new_height; ++i) {
        for (size_t j = 0; j < width_p; ++j)
            std::swap(img[width_p * i + j],
                      img[width_p * (height - i - 1) + j]);
    }
}

void Solver::rotate_up() {
    auto tmp = new unsigned char[size];
    size_t idx = 0;
    for (size_t i = width_p; i > 0; i -= pixel_size) {
        for (size_t j = 0; j < height; ++j) {
            for (size_t k = 0; k < pixel_size; ++k)
                tmp[idx++] = img[j * width_p - pixel_size + i + k];
        }
    }
    std::swap(height, width);
    std::swap(img, tmp);
    delete[] tmp;
}

void Solver::rotate_down() {
    auto tmp = new unsigned char[size];
    size_t idx = 0;
    for (size_t i = 0; i < width_p; i += pixel_size) {
        for (int j = height - 1; j >= 0; --j) {
            for (size_t k = 0; k < pixel_size; ++k)
                tmp[idx++] = img[j * width_p + i + k];
        }
    }
    std::swap(height, width);
    std::swap(img, tmp);
    delete[] tmp;
}

Solver::~Solver() {
    delete[] img;
}
