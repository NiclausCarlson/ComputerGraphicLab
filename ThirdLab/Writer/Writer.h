//
// Created by Nik Carlson on 02.03.2021.
//

#ifndef SECONDLAB_WRITER_H
#define SECONDLAB_WRITER_H

#include <vector>
#include <fstream>
#include "../Image/Image.h"

class Writer {
public:
    void write(const Image *img, const std::string &path);

private:
    void write(double *&img, size_t size, size_t height, size_t width, size_t type, const std::string &path);

    void write_one_file(const Image *img, const std::string &path);
};


#endif //SECONDLAB_WRITER_H
