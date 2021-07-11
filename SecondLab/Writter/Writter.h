//
// Created by Nik Carlson on 02.03.2021.
//

#ifndef SECONDLAB_WRITTER_H
#define SECONDLAB_WRITTER_H

#include <vector>
#include <fstream>
#include "../Image/Image.h"

class Writter {
public:
    void write(const Image *img, const std::vector<std::string> &paths);

private:
    void write(byte *&img, size_t size, size_t height, size_t width, size_t type, const std::string &path, bool should_free_if_fail);

    void write_one_file(const Image *img, const std::string &path);

    void write_three_files(const Image *img, const std::vector<std::string> &paths);
};


#endif //SECONDLAB_WRITTER_H
