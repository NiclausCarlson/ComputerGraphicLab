//
// Created by Nik Carlson on 27.04.2021.
//

#include "Solver.h"

Solver::Solver() {
    std::fill(barGraph.begin(), barGraph.end(), 0);
}

void Solver::solve(Image *&img, int32_t classes) {
    prepare(img);
    std::vector<int32_t> remembered = otsu(classes - 1);
    paint(remembered, classes, img);
}

void Solver::prepare(Image *const &img) {
    byte *image = img->get_image();
    for (size_t i = 0; i < img->get_size(); ++i) {
        ++barGraph[image[i]];
    }
    int32_t idx = 1;
    prefix_p.push_back(barGraph[0]);
    prefix_fp.push_back(barGraph[0]);
    for (auto i = barGraph.begin() + 1; i != barGraph.end(); ++i) {
        prefix_p.push_back(prefix_p.back() + *i);
        prefix_fp.push_back(prefix_fp.back() + *i * idx++);
    }
}

std::vector<int32_t> Solver::otsu(int32_t classes) {
    BinaryBruteForce bbf(classes);
    double max_dispersion = -1.;
    bool flag = true;
    while (flag) {
        if (bbf.is_end()) flag = false;
        auto v = bbf.get_thresholds();
        std::vector<int64_t> cur_p_prefix;
        std::vector<int64_t> cur_fp_prefix;
        bool is_empty = false;
        for (auto i = v.begin(); i != v.end(); ++i) {
            if (i != v.begin()) {
                cur_p_prefix.push_back(prefix_p[*i - 1] - prefix_p[*(i - 1) - 1]);
                cur_fp_prefix.push_back(prefix_fp[*i - 1] - prefix_fp[*(i - 1) - 1]);
            } else {
                cur_p_prefix.push_back(prefix_p[*i - 1]);
                cur_fp_prefix.push_back(prefix_fp[*i - 1]);
            }
            if (cur_p_prefix.back() == 0) {
                is_empty = true;
                break;
            }
        }

        double cur_dispersion = 0;
        if (!is_empty) {
            for (size_t i = 0; i < cur_p_prefix.size(); ++i) {
                cur_dispersion += double(cur_fp_prefix[i] * cur_fp_prefix[i]) / cur_p_prefix[i];
            }
            if (cur_dispersion > max_dispersion) {
                max_dispersion = cur_dispersion;
                bbf.remember();
            }
        }
        bbf.add_one();
    }
    return bbf.get_remembered();
}

void Solver::paint(std::vector<int32_t> &thresholds, int32_t classes, Image *&img) {
    auto get_class = [&thresholds](byte pixel) {
        auto upper_bound = std::upper_bound(thresholds.begin(), thresholds.end(), pixel);
        return std::distance(thresholds.begin(), upper_bound) ;
    };
    std::vector<double> colors;
    double cur_color = 0;
    double add = 255. / (classes - 1);
    for (size_t i = 0; i < classes; ++i) {
        colors.push_back(cur_color);
        cur_color += add;
    }
    byte *image = img->get_image();
    for (size_t i = 0; i < img->get_size(); ++i) {
        int32_t cls = get_class(image[i]);
        image[i] = colors[cls];
    }
}
