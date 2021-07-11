//
// Created by Nik Carlson on 31.05.2021.
//

#ifndef EIHTH_LAB_JPEG_READER_H
#define EIHTH_LAB_JPEG_READER_H

#define _USE_MATH_DEFINES


#include "../Image/Image.h"
#include "Constants.h"
#include "Huffman/HuffmanDecoder.h"
#include "Utils/Utils.h"
#include "BitsFlow/BitsFlow.h"
#include <fstream>
#include <vector>
#include <cmath>


class JPEG_Reader {
private:
    struct QuantumTable {
        int32_t SIZE{64};
        int32_t *table{};

        QuantumTable() = default;

        explicit QuantumTable(const byte *data) : table(new int32_t[SIZE]) {
            for (size_t i = 0; i < SIZE; ++i) {
                table[i] = hex_to_dec(data[i + 1]);
            }
        }

        int32_t get_value(size_t idx) const {
            return table[idx];
        }

        ~QuantumTable() {
            delete[] table;
        }
    };

    size_t height = -1;
    size_t width = -1;
    const int32_t CHANNELS = 1;
    const int32_t PRECISION = 8;
    const size_t BLOCK_SIZE = 64;
    std::vector<int32_t> coeffs;
    QuantumTable *table;

    void parse_header(const byte *buff);

    void parse_quantum_table(const byte *buff);

    void parse_coded_data(std::ifstream &fin, std::vector<HuffmanDecoder *> &decoders);

    void generate_image_blocks(std::vector<byte> &img);

public:
    Image *read(const std::string &path);

    ~JPEG_Reader();
};


#endif //EIHTH_LAB_JPEG_READER_H
