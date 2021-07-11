#include <iostream>
#include <cstdlib>

#include "Solver/Solver.h"

int main(int argc, char *args[]) {
    std::string a(args[3]);
    if (argc != 4) {
        std::cerr << "Invalid arguments quantity.";
        return 1;
    }
    size_t number_size = sizeof(args[3]) / sizeof(int);
    int mode;
    if (number_size > 1 || !isdigit(args[3][0])) {
        std::cerr << "Invalid mode argument.";
        return 1;
    } else {
        mode = atoi(args[3]);
    }

    Solver solver(args[1], args[2], mode);
    try {
        solver.solve();
    } catch (const char *msg) {
        std::cerr << msg << std::endl;
        return 1;
    } catch (const std::bad_alloc &msg) {
        std::cerr << "Fails to allocate storage: " << msg.what();
        return 1;
    } catch (const std::runtime_error &msg){
        std::cerr<<msg.what();
        return 1;
    }
    return 0;
}
