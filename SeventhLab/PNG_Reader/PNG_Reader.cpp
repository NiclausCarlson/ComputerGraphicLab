//
// Created by Nik Carlson on 25.05.2021.
//

#include "PNG_Reader.h"

Image *PNG_Reader::read(const std::string &path) {
    std::ifstream fin;
    fin.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        fin.open(path, std::ios::binary);
        read_signature(fin);
        read_chucks(fin);
    } catch (const std::ifstream::failure &e) {
        throw std::runtime_error("Exception opening/reading file.");
    }
    return to_portable_anymap();
}

void PNG_Reader::read_signature(std::ifstream &fin) {
    const size_t signature_size = 8;
    byte *signature = new byte[signature_size];
    fin.read(reinterpret_cast<char *>(signature), signature_size);
    std::stringstream ss;
    for (int i = 0; i < signature_size; i++)
        ss << std::hex << static_cast<int32_t>(signature[i]);
    delete[] signature;
    if (SIGNATURE != ss.str()) {
        throw std::runtime_error("Invalid png file.");
    }
}

void PNG_Reader::read_chucks(std::ifstream &fin) {
    auto from_hex = [](const byte *hex_number, size_t idx1, size_t idx2) {
        std::stringstream ss;
        ss << "0x";
        for (size_t i = idx1; i < idx2; i++) {
            int tmp = static_cast<int>(hex_number[i]);
            if (tmp < 16) ss << "0";
            ss << std::hex << tmp;
        }
        return stoi(ss.str(), nullptr, 16);
    };
    while (true) {
        byte buff4[4];
        size_t chunk_length;
        std::string chunk_name;
        fin.read(reinterpret_cast<char *>(buff4), 4);
        chunk_length = from_hex(buff4, 0, 4);
        fin.read(reinterpret_cast<char *>(buff4), 4);
        for (byte i : buff4) chunk_name += static_cast<char>(i);
        if (chunk_name == "IEND") break;

        byte *buff = new(std::nothrow) byte[chunk_length];
        if (buff == nullptr) {
            throw std::runtime_error("Can't allocate memory to image.");
        }
        fin.read(reinterpret_cast<char *>(buff), chunk_length);
        if (chunk_name == "IHDR") {
            width = from_hex(buff, 0, 4);
            height = from_hex(buff, 4, 8);
            bit_depth = buff[8];
            color_type = buff[9];
            if (color_type == 2) color_type = 3;
            else if (color_type == 0) color_type = 1;
            compression_method = buff[10];
            filtration_method = buff[11];
            interlace_method = buff[12];
        } else if (chunk_name == "IDAT") {
            data.insert(data.end(), buff, buff + chunk_length);
        }
        delete[] buff;
        //reading CRC
        fin.read(reinterpret_cast<char *>(buff4), 4);
    }
}

Image *PNG_Reader::to_portable_anymap() {
    size_t uncompressed_width = color_type * width + 1;
    size_t size = color_type * width * height;
    byte *uncompressed = new(std::nothrow) byte[uncompressed_width * height];
    byte *array_data = new(std::nothrow) byte[size];

    auto free_and_throw = [&](const std::string &message) {
        delete[] uncompressed;
        delete[] array_data;
        throw std::runtime_error(message);
    };

    if (uncompressed == nullptr || array_data == nullptr) free_and_throw("Can't allocate memory.");
    std::copy(data.begin(), data.end(), array_data);
    data.clear();

    z_stream inf;
    inf.zalloc = Z_NULL;
    inf.zfree = Z_NULL;
    inf.opaque = Z_NULL;
    inf.avail_in = size;
    inf.next_in = (Bytef *) array_data;
    inf.avail_out = uncompressed_width * height;
    inf.next_out = (Bytef *) uncompressed;

    auto res = inflateInit(&inf);
    if (res != Z_OK) {
        if (res == Z_MEM_ERROR) free_and_throw("Can't allocate memory to inflate");
        else if (res == Z_VERSION_ERROR) free_and_throw("Invalid zlib version");
    }
    res = inflate(&inf, Z_NO_FLUSH);
    if (res == Z_DATA_ERROR || res == Z_STREAM_ERROR || res == Z_MEM_ERROR || res == Z_BUF_ERROR)
        free_and_throw("Can't inflate image.");
    res = inflateEnd(&inf);
    if (res == Z_STREAM_ERROR) free_and_throw("Can't inflate image.");

    byte *img = new(std::nothrow) byte[size];
    if (img == nullptr) free_and_throw("Can't allocate memory.");
    size_t idx = 0;
    for (size_t i = 0; i < height; i++) {
        auto filter = static_cast<int32_t> (uncompressed[i * uncompressed_width]);
        for (size_t j = 1; j < uncompressed_width; j++) {
            int32_t left = 0, upper = 0, left_upper = 0;
            int32_t uncompressed_value = uncompressed[i * uncompressed_width + j];
            if (j > color_type) left = img[i * width * color_type + j - 1 - color_type];
            if (i != 0) upper = img[(i - 1) * width * color_type + j - 1];
            if (i != 0 && j > color_type) left_upper = img[(i - 1) * width * color_type + j - 1 - color_type];
            if (filter == 0) {
                img[idx++] = uncompressed_value;
            } else if (filter == 1) {
                img[idx++] = (uncompressed_value + left) % 256;
            } else if (filter == 2) {
                img[idx++] = (uncompressed_value + upper) % 256;
            } else if (filter == 3) {
                img[idx++] = static_cast<byte> (uncompressed_value + std::floor((left + upper) / 2)) % 256;
            } else {
                int32_t p = left + upper - left_upper;
                auto pa = std::make_pair(std::abs(p - left), left);
                auto pb = std::make_pair(std::abs(p - upper), upper);
                auto pc = std::make_pair(std::abs(p - left_upper), left_upper);
                auto comp = [](const std::pair<int32_t, int32_t> &p1, const std::pair<int32_t, int32_t> &p2) {
                    return p1.first < p2.first;
                };
                auto nearest = std::min(pa, std::min(pb, pc, comp), comp);
                img[idx++] = (uncompressed_value + nearest.second) % 256;
            }
        }
    }
    return new Image(img, size, height, width, color_type == 3 ? 6 : 5);
}
