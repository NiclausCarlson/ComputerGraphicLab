//
// Created by Nik Carlson on 31.05.2021.
//

#include "JPEG_Reader.h"

Image *JPEG_Reader::read(const std::string &path) {
    std::ifstream fin;
    fin.exceptions(std::ifstream::badbit);
    byte *buff;
    std::vector<HuffmanDecoder *> decoders;
    try {
        fin.open(path, std::ios::binary);
        int32_t section_code_1, section_code_2;
        int32_t sections_length;
        byte buf[2];
        fin.read(reinterpret_cast<char *>(buf), 2);
        section_code_1 = hex_to_dec(buf[0]);
        section_code_2 = hex_to_dec(buf[1]);
        if (section_code_1 != SECTION_START || section_code_2 != START) {
            throw std::runtime_error("Invalid jpeg file.");
        }
        while (true) {
            fin.read(reinterpret_cast<char *>(buf), 2);
            section_code_1 = hex_to_dec(buf[0]);
            section_code_2 = hex_to_dec(buf[1]);
            if (section_code_1 != SECTION_START) {
                fin.close();
                throw std::runtime_error("Invalid file.");
            }
            fin.read(reinterpret_cast<char *>(buf), 2);
            sections_length = hex_to_dec(buf, 0, 2) - 2;
            buff = new(std::nothrow) byte[sections_length];
            if (buff == nullptr) {
                fin.close();
                throw std::runtime_error("Can't allocate memory to read jpeg file.");
            }
            try {
                fin.read(reinterpret_cast<char *>(buff), sections_length);
                if (section_code_2 == IMG) {
                    parse_coded_data(fin, decoders);
                    fin.close();

                    std::vector<byte> img;
                    generate_image_blocks(img);
                    byte *image = new(std::nothrow) byte[img.size()];
                    if (image == nullptr) {
                        throw std::runtime_error("Can't allocate memory to convert image.");
                    }
                    size_t new_width = 0;
                    while (img.size() % (this->width + new_width) != 0) {
                        new_width++;
                    }
                    new_width += width;
                    size_t new_height = img.size() / new_width;
                    size_t idx = 0;
                    std::vector<std::vector<byte>> lines(new_height);
                    for (size_t i = 0; i < new_height; i += PRECISION) {
                        for (size_t b = 0; b < new_width; ++b) {
                            for (size_t j = 0; j < PRECISION; ++j) {
                                if (i + j >= new_height) break;
                                lines[i + j].push_back(img[idx++]);
                            }
                        }
                    }
                    idx = 0;
                    for (size_t i = 0; i < new_height; ++i) {
                        for (size_t j = 0; j < new_width; ++j) {
                            image[idx++] = lines[i][j];
                        }
                    }
                    return new Image(image, new_width * new_height, new_height, new_width, 5);
                } else if (section_code_2 == BASELINE) parse_header(buff);
                else if (section_code_2 == QUANTUM) parse_quantum_table(buff);
                else if (section_code_2 == HUFFMAN) decoders.push_back(new HuffmanDecoder(buff, sections_length));
                delete[] buff;
            } catch (const std::ifstream::failure &error) {
                delete[] buff;
                fin.close();
                throw error;
            } catch (const std::runtime_error &error) {
                delete[] buff;
                fin.close();
                throw error;
            }
        }
    } catch (const std::ifstream::failure &err) {
        throw std::runtime_error("Exception opening/reading file.");
    }

}

void JPEG_Reader::parse_header(const byte *buff) {
    int32_t precision = hex_to_dec(buff[0]);
    if (precision != this->PRECISION) throw std::runtime_error("Invalid img.");
    this->height = hex_to_dec(buff, 1, 3);
    this->width = hex_to_dec(buff, 3, 5);
    int32_t channels = hex_to_dec(buff[5]);
    if (channels != this->CHANNELS) throw std::runtime_error("Invalid img");
    int16_t pos = 6;
    int32_t id = hex_to_dec(buff[pos++]);
    pos++;
    int32_t quantum_table_id = hex_to_dec(buff[pos++]);
}

void JPEG_Reader::parse_quantum_table(const byte *buff) {
    table = new QuantumTable(buff);
}

void JPEG_Reader::parse_coded_data(std::ifstream &fin,
                                   std::vector<HuffmanDecoder *> &decoders) {
    BitsFlow bf(&fin);
    HuffmanDecoder *dc, *ac;
    for (auto i: decoders) {
        if (i->get_type() == AC) ac = i;
        else dc = i;
    }

    size_t counter = 0;
    auto read_dc = [&](BitResult &bit, bool &is_dc_read) {
        auto bit_res = dc->get_code(bit.res);
        if (bit_res.status == NOT_NEED_BIT) {
            ++counter;
            auto word = hex_to_dec(bit_res.word);
            is_dc_read = true;
            if (word == 0) coeffs.push_back(0);
            else {
                bool *arr = new bool[word];
                for (size_t i = 0; i < word; ++i) {
                    bit = bf.next_bit();
                    arr[i] = bit.res;
                }
                int32_t num = bin_to_dec(arr, 0, word);
                if (!arr[0]) num -= std::pow(2, word) - 1;
                coeffs.push_back(num);
                delete[] arr;
                return;
            }
        }
    };
    auto read_ac = [&](BitResult &bit) {
        auto bit_res = ac->get_code(bit.res);
        if (bit_res.status == NOT_NEED_BIT) {
            auto word = hex_to_dec(bit_res.word);
            if (word == 0) {
                while (counter < BLOCK_SIZE) {
                    coeffs.push_back(0);
                    counter++;
                }
            } else {
                size_t zeroes = word >> 4 & 0xf;
                for (size_t i = 0; i < zeroes; ++i) {
                    coeffs.push_back(0);
                }
                counter += zeroes;
                ptrdiff_t code_length = word & 0xf;
                if (code_length != 0) {
                    bool *arr = new bool[code_length];
                    for (ptrdiff_t i = 0; i < code_length; ++i) {
                        bit = bf.next_bit();
                        arr[i] = bit.res;
                    }
                    int32_t num = bin_to_dec(arr, 0, code_length);
                    if (!arr[0]) num -= std::pow(2, code_length) - 1;
                    coeffs.push_back(num);
                    ++counter;
                    delete[] arr;
                }
                return;
            }
        }
    };

    BitResult bit{};
    bool is_dc_read = false;
    while (bit.status != END) {
        while (counter < BLOCK_SIZE) {
            bit = bf.next_bit();
            if (bit.status == END) break;
            if (bit.status == OK) {
                if (!is_dc_read) read_dc(bit, is_dc_read);
                else read_ac(bit);
            }
        }
        counter = 0;
        is_dc_read = false;
    }
}

void JPEG_Reader::generate_image_blocks(std::vector<byte> &img) {
    std::vector<int32_t> tmp;
    auto iter = coeffs.begin();
    size_t table_idx = 0;
    const double sqr = 1. / std::sqrt(2);
    std::vector<std::vector<double>> cos(PRECISION, std::vector<double>(PRECISION, 0));
    std::vector<int32_t> cheat_array = {
            0, 1, 8, 16, 9, 2, 3, 10,
            17, 24, 32, 25, 18, 11, 4, 5,
            12, 19, 26, 33, 40, 48, 41, 34,
            27, 20, 13, 6, 7, 14, 21, 28,
            35, 42, 49, 56, 57, 50, 43, 36,
            29, 22, 15, 23, 30, 37, 44, 51,
            58, 59, 52, 45, 38, 31, 39, 46,
            53, 60, 61, 54, 47, 55, 62, 63};

    auto get_coef = [&sqr](size_t n) {
        if (n == 0) return sqr;
        return 1.;
    };
    for (size_t i = 0; i < PRECISION; ++i) {
        for (size_t j = 0; j < PRECISION; ++j)
            cos[i][j] = (get_coef(i) * std::cos((2. * j + 1.) * i * M_PI / 16.));
    }
    auto from_zig_zag = [&]() {
        std::vector<int32_t> data(BLOCK_SIZE);
        for (size_t i = 0; i < BLOCK_SIZE; ++i) data[i] = tmp[i];
        size_t idx = 0;
        for (auto i: data) tmp[cheat_array[idx++]] = i;
    };
    auto cosine = [&]() {
        for (int32_t i = 0; i < PRECISION; ++i) {
            for (int32_t j = 0; j < PRECISION; ++j) {
                double res = 0;
                for (int32_t u = 0; u < PRECISION; ++u) {
                    for (int32_t v = 0; v < PRECISION; ++v) {
                        res += tmp[v * PRECISION + u] * cos[v][j] * cos[u][i];
                    }
                }
                int32_t result = res / 4.;
                img.push_back(std::min(std::max(0, result + 128), 255));
            }
        }
    };

    int32_t prev_delta = 0, cur_delta, h_delta, table_value;
    while (iter != coeffs.end()) {
        cur_delta = *iter++;
        table_value = table->get_value(table_idx);
        if (table_idx == 0) {
            h_delta = (cur_delta + prev_delta);
            tmp.push_back(h_delta * table_value);
            prev_delta = h_delta;
        } else {
            tmp.push_back(cur_delta * table_value);
        }
        table_idx++;
        if (table_idx == BLOCK_SIZE) {
            table_idx = 0;
            from_zig_zag();
            cosine();
            tmp.clear();
        }
    }
}

JPEG_Reader::~JPEG_Reader() {
    delete table;
}
