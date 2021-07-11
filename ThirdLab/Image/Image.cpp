//
// Created by Nik Carlson on 27.02.2021.
//

#include "Image.h"

size_t Image::get_size() const {
    return size;
}

size_t Image::get_height() const {
    return height;
}

size_t Image::get_width() const {
    return width;
}

byte *Image::get_image() const {
    return image;
}

byte *Image::get_image() {
    return image;
}

double *Image::get_d_image() const {
    return d_image;
}

double *Image::get_d_image() {
    return d_image;
}

