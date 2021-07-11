//
// Created by Nik Carlson on 02.03.2021.
//

#include "Writer.h"

void Writer::write(const Image *img, const std::string &path) {
    byte *image = img->get_image();
    std::ofstream of;
    of.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    try {
        of.open(path, std::ios::binary);
        of << 'P' << img->get_type() << '\n';
        of << img->get_width() << ' ' << img->get_height() << '\n';
        of << 255 << '\n';
        of.write(reinterpret_cast<const char *>(image), img->get_size());
        of.close();
    } catch (const std::ofstream::failure &ex) {
        throw std::runtime_error("Can't write image");
    }
}


