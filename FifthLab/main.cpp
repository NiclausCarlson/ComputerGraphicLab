#include <iostream>
#include <string>
#include "Image/Image.h"
#include "Reader/Reader.h"
#include "Solver/Solver.h"
#include "Writer/Writer.h"

int main(int argc, char **argv) {
    if (argc != 4) {
        std::cerr << "Invalid arguments quantity.";
        return 1;
    }
    std::string input_file = argv[1];
    std::string output_file = argv[2];
    int32_t classes_quantity;
    try {
        classes_quantity = std::stoi(argv[3]);
    } catch (std::logic_error &err) {
        std::cerr << "Invalid classes quantity argument";
        return 1;
    }
    if (classes_quantity <= 0) {
        std::cerr << "Invalid arguments";
        return 1;
    }

    Image *img = nullptr;
    Reader reader;
    try {
        img = reader.read(input_file);
        Solver solver;
        solver.solve(img, classes_quantity);
        Writer writer;
        writer.write(img, output_file);
    } catch (std::runtime_error &err) {
        std::cerr << err.what();
        delete img;
        return 1;
    }
    delete img;
    return 0;
}
