#include <iostream>
#include <unordered_map>
#include <set>
#include "Image/Image.h"
#include "Reader/Reader.h"
#include "Converter/Converter.h"
#include "Writter/Writter.h"

std::vector<std::string> generate_three_path(std::string &file, const std::string &expected_extension) {
    std::vector<std::string> paths(3);
    std::string f = file.substr(0, file.size() - 4);
    f.append("_");
    std::string ext = file.substr(file.size() - 4);
    if (ext != expected_extension) {
        throw std::runtime_error("Invalid file extension");
    }
    for (size_t i = 0; i < 3; ++i) {
        paths[i].append(f).append(std::to_string(i + 1)).append(ext);
    }
    return paths;
}

void parse_arguments(int argc, char *argv[], const std::set<std::string> &arguments_type,
                     std::unordered_map<std::string, std::string> &arguments) {
    size_t i = 1;
    while (i < argc) {
        std::string q = argv[i];
        if (strcmp(argv[i], "-i") == 0) {
            if (i + 2 >= argc) throw std::logic_error("Invalid arguments");
            arguments["-in"] = argv[++i];
            arguments["-if"] = argv[++i];
        } else if (strcmp(argv[i], "-o") == 0) {
            if (i + 2 >= argc) throw std::logic_error("Invalid arguments");
            arguments["-on"] = argv[++i];
            arguments["-of"] = argv[++i];
        } else {
            if (i + 1 >= argc) throw std::logic_error("Invalid arguments");
            arguments[q] = argv[++i];
        }
        ++i;
    }

    for (const auto &arg: arguments_type) {
        const std::string &q = arg;
        if (arguments.count(arg) == 0)throw std::logic_error("Invalid arguments");
    }
}

void parse_color_space(const std::unordered_map<std::string, std::string> &arguments, COLOR_SPACE &input_space,
                       COLOR_SPACE &output_space) {
    std::unordered_map<std::string, COLOR_SPACE> spaces = {{"RGB",       RGB},
                                                           {"HSL",       HSL},
                                                           {"HSV",       HSV},
                                                           {"YCbCr.601", YCbCrdot601},
                                                           {"YCbCr.709", YCbCrdor709},
                                                           {"YCoCg",     YCoCg},
                                                           {"CMY",       CMY}};
    if (spaces.count(arguments.at("-f")) == 0 ||
        spaces.count(arguments.at("-t")) == 0) {
        throw std::invalid_argument("Invalid arguments");
    }
    input_space = spaces[arguments.at("-f")];
    output_space = spaces[arguments.at("-t")];
}

int main(int argc, char *argv[]) {
    if (argc != 11) {
        std::cerr << "Invalid argument quantity.";
        return 1;
    }
    std::set<std::string> arguments_type = {"-f", "-t", "-in", "-if", "-on", "-of"};
    std::unordered_map<std::string, std::string> arguments;
    COLOR_SPACE input_space, output_space;
    int input_files_quantity, output_files_quantity;
    try {
        parse_arguments(argc, argv, arguments_type, arguments);
        parse_color_space(arguments, input_space, output_space);
        input_files_quantity = std::stoi(arguments["-in"]);
        output_files_quantity = std::stoi(arguments["-on"]);
    } catch (std::logic_error &err) {
        std::cerr << err.what();
        return 1;
    }
    if (input_files_quantity != 1 && input_files_quantity != 3 &&
        output_files_quantity != 1 && output_files_quantity != 3) {
        std::cerr << "Invalid arguments";
        return 1;
    }
    Reader reader;
    Image *img;
    Converter converter;
    Writter writter;
    try {
        if (input_files_quantity == 1) img = reader.read({arguments["-if"]});
        else if (input_files_quantity == 3) img = reader.read(generate_three_path(arguments["-if"], ".pgm"));
        converter.solve(img, input_space, output_space);
        if (output_files_quantity == 1) writter.write(img, {arguments["-of"]});
        else writter.write(img, generate_three_path({arguments["-of"]}, ".pgm"));
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
