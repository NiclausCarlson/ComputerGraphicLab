#include <iostream>
#include "PNG_Reader/PNG_Reader.h"
#include "Writer/Writer.h"

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "Invalid argument quantity\n";
        return 1;
    }
    std::string input_file = argv[1];
    std::string output_file = argv[2];
    Image *img = nullptr;
    try {
        PNG_Reader reader;
        img = reader.read(input_file);
        Writer writer;
        writer.write(img, output_file);
    } catch (std::runtime_error &error) {
        std::cerr << error.what();
        delete img;
        return 1;
    }
    delete img;
    return 0;
}
