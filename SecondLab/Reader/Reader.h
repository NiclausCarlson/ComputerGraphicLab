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
    Image* read(const std::vector<std::string> &files);

private:
    void read_one_file(const std::vector<std::string> &files, byte *&img, int &size, int &width, int &height);

    void read_three_file(const std::vector<std::string> &files, byte *&img, int &size, int &width,
                         int &height);

    void cond_free(byte *&c1, byte *&c2, byte *&c3);

    void h_free(byte *&c1, byte *&c2, byte *&c3);

    void exit_free_throw(std::ifstream &fin, byte *&c1, byte *&c2, byte *&c3, const std::string &message);
};


#endif //SECONDLAB_READER_H
