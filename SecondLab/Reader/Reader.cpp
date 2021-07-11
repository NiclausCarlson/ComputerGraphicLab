//
// Created by Nik Carlson on 27.02.2021.
//

#include "Reader.h"

Image *Reader::read(const std::vector<std::string> &files) {
    byte *img = nullptr;
    int size, width = -1, height = -1;
    if (files.size() == 1) read_one_file(files, img, size, width, height);
    else if (files.size() == 3) read_three_file(files, img, size, width, height);
    else throw std::runtime_error("Exception: Invalid input file quantity.");
    return new Image(img, size, height, width);
}

void Reader::read_one_file(const std::vector<std::string> &files, byte *&img, int &size, int &width, int &height) {
    int type, max, pixel_size = 3;
    std::ifstream fin;
    fin.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        fin.open(files.front(), std::ios::binary);
        std::string tmp;
        fin >> tmp;
        if (tmp[0] != 'P' || !isdigit(tmp[1]) || tmp.size() > 2) {
            fin.close();
            throw std::runtime_error("Invalid file.");
        }
        try {
            type = stoi(tmp.substr(1));
            if (type != 6) {
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
            throw std::runtime_error("Invalid images img.");
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

void Reader::read_three_file(const std::vector<std::string> &files, byte *&img, int &size, int &width, int &height) {
    int type = -1, max = -1;
    int h_type, h_width, h_height, h_max;
    byte *ch_1 = nullptr, *ch_2 = nullptr, *ch_3 = nullptr;
    for (size_t idx = 0; idx < files.size(); ++idx) {
        std::ifstream fin;
        fin.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            fin.open(files[idx], std::ios::binary);
            std::string tmp;
            fin >> tmp;
            if (tmp[0] != 'P' || !isdigit(tmp[1]) || tmp.size() > 2) {
                exit_free_throw(fin, ch_1, ch_2, ch_3, "Invalid file.");
            }
            try {
                h_type = stoi(tmp.substr(1));
                if (h_type != 5) {
                    exit_free_throw(fin, ch_1, ch_2, ch_3, "Invalid format version.");
                }
                fin >> tmp;
                h_width = stoi(tmp);
                fin >> tmp;
                h_height = stoi(tmp);
                fin >> tmp;
                h_max = stoi(tmp);
            } catch (const std::logic_error &e) {
                exit_free_throw(fin, ch_1, ch_2, ch_3, "Invalid images img.");
            }
            if (type == -1) {
                type = h_type;
                width = h_width;
                height = h_height;
                max = h_max;
                size = height * width;
                ch_1 = new byte[size];
                ch_2 = new byte[size];
                ch_3 = new byte[size];
                cond_free(ch_1, ch_2, ch_3);
            } else if (type != h_type || width != h_width || height != h_height || max != h_max) {
                exit_free_throw(fin, ch_1, ch_2, ch_3, "This files isn't equals");
            }
            if (h_max != 255) {
                exit_free_throw(fin, ch_1, ch_2, ch_3, "Invalid max.");
            }
            fin.get();
            if (idx == 0) fin.read(reinterpret_cast<char *>(ch_1), size);
            else if (idx == 1) fin.read(reinterpret_cast<char *>(ch_2), size);
            else if (idx == 2) fin.read(reinterpret_cast<char *>(ch_3), size);
            fin.close();
        } catch (const std::ifstream::failure &e) {
            exit_free_throw(fin, ch_1, ch_2, ch_3, "Exception opening/reading file.");
        }
    }
    img = new(std::nothrow) byte[size * 3];
    if (img == nullptr) throw std::runtime_error("Failed to allocate memory.");
    size_t idx = 0;
    for (size_t i = 0; i < size; ++i) {
        img[idx++] = ch_1[i];
        img[idx++] = ch_2[i];
        img[idx++] = ch_3[i];
    }
    size *= 3;
    h_free(ch_1, ch_2, ch_3);
}

void Reader::cond_free(byte *&c1, byte *&c2, byte *&c3) {
    if (c1 == nullptr || c2 == nullptr || c3 == nullptr) {
        delete[] c1;
        delete[] c2;
        delete[] c3;
        throw std::runtime_error("Failed to allocate memory.");
    }
}

void Reader::h_free(byte *&c1, byte *&c2, byte *&c3) {
    delete[] c1;
    delete[] c2;
    delete[] c3;
    c1 = nullptr;
    c2 = nullptr;
    c3 = nullptr;
}

void Reader::exit_free_throw(std::ifstream &fin, byte *&c1, byte *&c2, byte *&c3, const std::string &message) {
    h_free(c1, c2, c3);
    fin.close();
    throw std::runtime_error(message);
}
