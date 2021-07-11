//
// Created by Nik Carlson on 27.02.2021.
//

#ifndef SECONDLAB_READER_H
#define SECONDLAB_READER_H

#include <vector>
#include <string>
#include <fstream>
#include "../Image/Image.h"


class Reader {
public:
    Image* read(const std::string &file);
private:
    void read_one_file(const std::string &file,
                       byte *&img,
                       int32_t &size,
                       int32_t &width,
                       int32_t &height);
};


#endif //SECONDLAB_READER_H
