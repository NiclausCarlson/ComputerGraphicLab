#include <iostream>
#include "Reader/Reader.h"
#include "Dithering/Dithering.h"
#include "Writer/Writer.h"

std::pair<int32_t, std::string::iterator> get_number(std::string::iterator &it, const std::string::iterator &end) {
    std::string number;
    while (it != end && isdigit(*it)) {
        number.push_back(*it);
        it++;
    }
    std::pair<int32_t, std::string::iterator> p;
    p.first = std::stoi(number);
    p.second = it;
    while (p.second != end && !isdigit(*p.second)) ++p.second;
    return p;
}

void generate_gradient(double *&image, int32_t height, int32_t width) {
    double step = 255. / (width - 1);
    for (size_t i = 0; i < height; ++i) {
        image[i * width] = 0.;
        for (size_t j = 1; j < width; ++j) {
            image[i * width + j] = image[i * width + j - 1] + step;
        }
    }
}

int main(int argc, char **argv) {
    if (argc != 7) {
        std::cerr << "Invalid arguments quantity";
        return 1;
    }
    std::string input_file_name = argv[1];
    std::string output_file_name = argv[2];
    int32_t grad, dithering, bits;
    double gamma;
    try {
        grad = atoi(argv[3]);
        dithering = atoi(argv[4]);
        bits = atoi(argv[5]);
        gamma = atof(argv[6]);
    } catch (std::logic_error &ex) {
        std::cerr << "Invalid arguments";
        return 1;
    }
    if ((grad != 0 && grad != 1)
        || (dithering < 0 || dithering > 7)
        || (bits < 1 || bits > 8)
        || gamma < 0) {
        std::cerr << "Invalid arguments";
        return 1;
    }

    Reader reader;
    Image *img = nullptr;
    Dithering converter(gamma);
    Writer writer;
    try {
        if (grad == 0) {
            img = reader.read(input_file_name);
        } else {
            int32_t width, height;
//            size_t idx = input_file_name.find_last_of('\\');
//            if (idx == std::string::npos) idx = 0;
//            else ++idx;
//            std::string::iterator it = input_file_name.begin() + idx;
//            std::pair<int32_t, std::string::iterator> p = get_number(it, input_file_name.end());
//            width = p.first;
//            height = get_number(p.second, input_file_name.end()).first;
            img = reader.read(input_file_name);
            width = img->get_width();
            height = img->get_height();
            delete img;
            auto *image = new(std::nothrow) double[width * height];
            if (image == nullptr) throw std::bad_alloc();
            generate_gradient(image, height, width);
            img = new Image(image, width * height, height, width);
        }
        converter.convert(img, dithering, bits);
        writer.write(img, output_file_name);
    } catch (std::bad_alloc &err) {
        std::cerr << err.what();
        return 1;
    } catch (std::runtime_error &err) {
        std::cerr << err.what();
        return 1;
    }
    delete img;
    return 0;
}