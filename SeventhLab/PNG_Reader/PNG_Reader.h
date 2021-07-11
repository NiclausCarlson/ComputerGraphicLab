//
// Created by Nik Carlson on 25.05.2021.
//

#ifndef SEVENTHLAB_PNG_READER_H
#define SEVENTHLAB_PNG_READER_H


#include "../Image/Image.h"
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include "../zlib/zlib.h"

class PNG_Reader {
private:
    size_t width;
    size_t height;
    size_t all_data_chunks_size;
    size_t bit_depth;
    size_t color_type;
    size_t compression_method;
    size_t filtration_method;
    size_t interlace_method;

    const std::string SIGNATURE = "89504e47da1aa";

    std::vector<byte> data;

    void read_signature(std::ifstream &fin);

    void read_chucks(std::ifstream &fin);

    Image *to_portable_anymap();

public:
    Image *read(std::string const &path);
};


#endif //SEVENTHLAB_PNG_READER_H
