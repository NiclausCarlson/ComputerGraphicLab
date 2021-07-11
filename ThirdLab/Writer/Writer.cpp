//
// Created by Nik Carlson on 02.03.2021.
//

#include "Writer.h"

void Writer::write(const Image *img, const std::string &path) {
    write_one_file(img, path);
}

void Writer::write_one_file(const Image *img, const std::string &path) {
    double *tmp = img->get_d_image();
    write(tmp, img->get_size(), img->get_height(), img->get_width(), 5, path);
}

void Writer::write(double *&img, size_t size, size_t height, size_t width, size_t type, const std::string &path) {
    std::ofstream of;
    of.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    byte* tmp = new byte[size];
    try {
        of.open(path, std::ios::binary);
        of << 'P' << type << '\n';
        of << width << ' ' << height << '\n';
        of << 255 << '\n';
        for (size_t i = 0; i < size; ++i)
            tmp[i] = (byte)img[i];
        of.write(reinterpret_cast<const char *>(tmp), size);
        of.close();
        delete[] tmp;
    } catch (const std::ofstream::failure &ex) {
        delete[] tmp;
        throw std::runtime_error("Can't write image");
    }
}
