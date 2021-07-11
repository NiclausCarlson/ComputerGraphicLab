//
// Created by Nik Carlson on 05.06.2021.
//

#ifndef EIHTH_LAB_HUFFMANDECODER_H
#define EIHTH_LAB_HUFFMANDECODER_H


#include "../../Image/Image.h"
#include "../Utils/Utils.h"
#include <vector>
#include <iostream>
#include <bitset>

enum Type {
    AC,
    DC
};
enum Status {
    NOT_NEED_BIT,
    NEED_BIT
};

struct Result {
    Status status;
    byte word;

    Result(Status status, byte word) : status(status), word(word) {
    }
};

class HuffmanDecoder {
private:
    Type type;

    struct Node {
        Node *left, *right;
        byte word{};
        bool used;

        Node() : left(nullptr), right(nullptr), used(false) {
        }
    };

    Node *root, *cur_node;

    void build_tree(std::vector<int32_t> &quantity, const byte *words, size_t words_idx, size_t length);

    void set_word(Node *&node, int32_t depth, byte word, bool &is_set);

    void free(Node *&node);

    void print_tree(const Node *node, std::vector<char> &code) const;
    void throw_ex(bool bit);
public:
    explicit HuffmanDecoder(const byte *code, size_t length);

    Type get_type() const;

    Result get_code(bool bit);

    void print_tree();

    ~HuffmanDecoder();
};


#endif //EIHTH_LAB_HUFFMANDECODER_H
