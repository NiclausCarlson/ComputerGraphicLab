//
// Created by Nik Carlson on 20.02.2021.
//

#ifndef FIRSTLAB_SOLVER_H
#define FIRSTLAB_SOLVER_H

#include <string>
#include <fstream>
#include <algorithm>

typedef unsigned char image;

class Solver {
public:

    Solver(const char *input_path, const char *output_path, int mode);

    ~Solver();

    void solve();

private:
    const char *input_path;
    const char *output_path;
    const int mode;
    size_t type, size, pixel_size, width, height, max;
    size_t width_p;
    image *img;

    void read_file();

    void write_wile();

    void inverse();

    void horizontal();

    void vertical();

    void rotate_up();

    void rotate_down();
};


#endif //FIRSTLAB_SOLVER_H
