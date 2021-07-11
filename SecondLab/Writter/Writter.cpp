//
// Created by Nik Carlson on 02.03.2021.
//

#include "Writter.h"

void Writter::write(const Image *img, const std::vector<std::string> &paths) {
    if (paths.size() == 1) write_one_file(img, paths.front());
    else write_three_files(img, paths);
}

void Writter::write_one_file(const Image *img, const std::string &path) {
    byte *tmp = img->get_image();
    write(tmp, img->get_size(), img->get_height(), img->get_width(), 6, path, false);
}

void Writter::write_three_files(const Image *img, const std::vector<std::string> &paths) {
    byte *image = img->get_image();
    size_t size = img->get_size();
    size_t p_size = size / 3;
    byte *pixels = new(std::nothrow) byte[p_size];
    if (pixels == nullptr) throw std::runtime_error("Can't allocate memory to write image.");
    for (size_t i = 0; i < 3; ++i) {
        size_t idx = 0;
        for (size_t j = i; j < size; j += 3) {
            pixels[idx++] = image[j];
        }
        write(pixels, p_size, img->get_height(), img->get_width(), 5, paths[i], true);
    }
    delete[] pixels;
}

void Writter::write(byte *&img, size_t size, size_t height, size_t width, size_t type, const std::string &path,
                    bool should_free_if_fail) {
    std::ofstream of;
    of.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    try {
        of.open(path, std::ios::binary);
        of << 'P' << type << '\n';
        of << width << ' ' << height << '\n';
        of << 255 << '\n';
        of.write(reinterpret_cast<const char *>(img), size);
        of.close();
    } catch (const std::ofstream::failure &ex) {
        if (should_free_if_fail) delete[] img;
        throw std::runtime_error("Can't write image");
    }
}
