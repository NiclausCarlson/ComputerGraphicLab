#include <iostream>
#include <string>
#include "Reader/Reader.h"
#include "Writer/Writer.h"
#include "Scaler/Scaler.h"

void free(Image *&img1, Image *&img2, Scaler *&img3) {
    delete img1;
    delete img2;
    delete img3;
}

int main(int argc, char **argv) {
    if (argc < 8) {
        std::cerr << "Invalid input";
        return 1;
    }
    std::string input_file = argv[1];
    std::string output_file = argv[2];
    int32_t result_height, result_width;
    Method method;
    [[maybe_unused]] double dx, dy, gamma, b, c;
    try {
        result_width = std::stoi(argv[3]);
        result_height = std::stoi(argv[4]);
        dx = std::stod(argv[5]);
        dy = std::stod(argv[6]);
        gamma = std::stod(argv[7]);
        if (strcmp(argv[8], "0") == 0) method = Near;
        else if (strcmp(argv[8], "1") == 0) method = Bilinear;
        else if (strcmp(argv[8], "2") == 0) method = Lanczos3;
        else if (strcmp(argv[8], "3") == 0 && argc == 11) {
            method = BC;
            b = std::stod(argv[9]);
            c = std::stod(argv[10]);
        } else method = BC;

    } catch (std::invalid_argument &ex) {
        std::cerr << "Invalid arguments";
        return 1;
    }
    Reader reader;
    Writer writer;
    Scaler *scaler;
    if (method == BC && argc == 11) scaler = new Scaler(method, result_height, result_width, dx, dy, b, c);
    else scaler = new Scaler(method, result_height, result_width, dx, dy);
    Image *image = nullptr;
    Image *new_image = nullptr;
    try {
        image = reader.read(input_file);
        new_image = scaler->scale(image);
        writer.write(new_image, output_file);
    } catch (std::runtime_error &ex) {
        free(image, new_image, scaler);
        std::cerr << ex.what();
        return 1;
    } catch (std::logic_error &ex) {
        free(image, new_image, scaler);
        std::cerr << ex.what();
        return 1;
    }
    free(image, new_image, scaler);
    return 0;
}
