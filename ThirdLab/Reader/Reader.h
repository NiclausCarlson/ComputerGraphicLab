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
    Image* read(const std::string& files);
private:
    void read_one_file(const std::string &file, double *&img, int &size, int &width, int &height);
};


#endif //SECONDLAB_READER_H
