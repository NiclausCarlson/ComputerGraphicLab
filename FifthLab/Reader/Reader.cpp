//
// Created by Nik Carlson on 27.02.2021.
//

#include "Reader.h"

Image *Reader::read(const std::string &file) {
    byte *img = nullptr;
    int size, width = -1, height = -1;
    read_one_file(file, img, size, width, height);
    return new Image(img, size, height, width, 5);
}

void Reader::read_one_file(const std::string &file,
                           byte *&img,
                           int32_t &width,
                           int32_t &size,
                           int32_t &height) {
    int32_t type, max, pixel_size = 1;
    std::ifstream fin;
    fin.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        fin.open(file, std::ios::binary);
        std::string tmp;
        fin >> tmp;
        if (tmp[0] != 'P' || !isdigit(tmp[1]) || tmp.size() > 2) {
            fin.close();
            throw std::runtime_error("Invalid file.");
        }
        try {
            type = stoi(tmp.substr(1));
            if (type != 5) {
                fin.close();
                throw std::runtime_error("Invalid format version.");
            }
            fin >> tmp;
            width = stoi(tmp);
            fin >> tmp;
            height = stoi(tmp);
            fin >> tmp;
            max = stoi(tmp);
            size = height * width * pixel_size;
            img = new byte[size];
            if (img == nullptr) {
                throw std::runtime_error("Can't allocate memory to read image");
            }
        } catch (const std::logic_error &e) {
            fin.close();
            throw std::runtime_error("Invalid image.");
        }
        if (max != 255) {
            delete[] img;
            img = nullptr;
            fin.close();
            throw std::runtime_error("Invalid max.");
        }
        fin.get();
        fin.read(reinterpret_cast<char *>(img), size);
        fin.close();
    } catch (const std::ifstream::failure &e) {
        delete[] img;
        throw std::runtime_error("Exception opening/reading file.");
    }
}
