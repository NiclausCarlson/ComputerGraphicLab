#include <iostream>
#include "JPEG_Reader/JPEG_Reader.h"
#include "Writer/Writer.h"

int main(int32_t argc, char **argv) {
    if (argc != 3) {
        std::cerr << "Invalid arguments quantity";
        return 1;
    }
    std::string input = argv[1];
    std::string output = argv[2];
    JPEG_Reader reader;
    Image *img = nullptr;
    try {
        img = reader.read(input);
        Writer writer;
        writer.write(img, output);
    } catch (std::runtime_error &error) {
        delete img;
        std::cerr << error.what();
        return 1;
    }
    delete img;
    return 0;
}

