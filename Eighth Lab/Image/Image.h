//
// Created by Nik Carlson on 27.02.2021.
//

#ifndef SECONDLAB_IMAGE_H
#define SECONDLAB_IMAGE_H

#include <fstream>

typedef unsigned char byte;

class Image {
public:
    Image() : image(nullptr), size(0), height(0), width(0), type(0) {}

    Image(byte *image, size_t size, size_t height, size_t width, size_t type) :
            image(image), size(size), height(height), width(width), type(type) {}

    ~Image() {
        delete[] image;
    }

    [[nodiscard]] size_t get_size() const;

    [[nodiscard]] size_t get_height() const;

    [[nodiscard]] size_t get_width() const;

    [[nodiscard]] byte *get_image() const;

    [[nodiscard]] size_t get_type() const;

    byte *get_image();


private:
    byte *image;
    size_t size, height, width, type;
};


#endif //SECONDLAB_IMAGE_H
