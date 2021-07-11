//
// Created by Nik Carlson on 02.03.2021.
//

#ifndef SECONDLAB_WRITTER_H
#define SECONDLAB_WRITTER_H

#include <vector>
#include <fstream>
#include "../Image/Image.h"

class Writer {
public:
    void write(const Image *img, const std::string &path);

};


#endif //SECONDLAB_WRITTER_H
