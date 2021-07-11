//
// Created by Nik Carlson on 05.06.2021.
//

#include "HuffmanDecoder.h"

HuffmanDecoder::HuffmanDecoder(const byte *code, size_t length) : root(new Node()) {
    if (get_bit(4, code[0])) type = AC;
    else type = DC;
    const size_t WORDS_LENGTH = 16;
    std::vector<int32_t> quantity;
    for (size_t i = 1; i <= WORDS_LENGTH; ++i) {
        quantity.push_back(hex_to_dec(code[i]));
    }
    build_tree(quantity, code, WORDS_LENGTH + 1, length);
}

void HuffmanDecoder::set_word(HuffmanDecoder::Node *&node, int32_t depth, byte word, bool &is_set) {
    if (node == nullptr) node = new Node();
    if (depth == 0 && !node->used) {
        node->word = word;
        node->used = true;
        is_set = true;
    } else if (!node->used) {
        if (!is_set && (node->left == nullptr || !node->left->used))
            set_word(node->left, depth - 1, word, is_set);
        if (!is_set && (node->right == nullptr || !node->right->used))
            set_word(node->right, depth - 1, word, is_set);
    }
    if (node->left != nullptr && node->right != nullptr
        && node->left->used && node->right->used) {
        node->used = true;
    }
}

void HuffmanDecoder::build_tree(std::vector<int32_t> &quantity, const byte *words,
                                size_t words_idx, size_t length) {
    for (int32_t i = 0; i < quantity.size(); ++i) {
        int32_t cur_depth = i + 1;
        while (quantity[i] != 0) {
            if (words_idx >= length) throw std::runtime_error("Invalid Huffman table.");
            if (quantity[i] != 0) {
                bool is_set = false;
                set_word(root, cur_depth, words[words_idx++], is_set);
            }
            --quantity[i];
        }
    }
    cur_node = root;
}

Type HuffmanDecoder::get_type() const {
    return type;
}

Result HuffmanDecoder::get_code(bool bit) {
    if (bit) {
        if (cur_node->right == nullptr) throw_ex(bit);
        cur_node = cur_node->right;
    } else {
        if (cur_node->left == nullptr) throw_ex(bit);
        cur_node = cur_node->left;
    }
    if (cur_node->left == nullptr && cur_node->right == nullptr) {
        auto res = Result(NOT_NEED_BIT, cur_node->word);
        cur_node = root;
        return res;
    }
    return Result(NEED_BIT, 0);
}

void HuffmanDecoder::free(Node *&node) {
    if (node != nullptr) {
        free(node->left);
        free(node->right);
        delete node;
    }
}

HuffmanDecoder::~HuffmanDecoder() {
    free(root);
}

void HuffmanDecoder::print_tree(const HuffmanDecoder::Node *node, std::vector<char> &code) const {
    if (node->left != nullptr) {
        code.push_back('0');
        print_tree(node->left, code);
    }
    if (node->right != nullptr) {
        code.push_back('1');
        print_tree(node->right, code);
    }
    if (node->left == nullptr && node->right == nullptr) {
        for (auto i: code) std::cout << i;
        std::cout << ":   " << hex_to_dec(node->word) << std::endl;
    }
    if (!code.empty()) code.pop_back();
}

void HuffmanDecoder::print_tree() {
    if (type == AC) std::cout << "AC" << std::endl;
    else std::cout << "DC" << std::endl;
    std::vector<char> code;
    print_tree(root, code);
    std::cout << std::endl;
}

void HuffmanDecoder::throw_ex(bool bit) {
    std::string ex("Invalid bit: ");
    if (bit) ex += "1. ";
    else ex += "0. ";
    ex += "Type: ";
    if (get_type() == AC) ex += "AC.";
    else ex += "DC.";
    throw std::runtime_error(ex);
}
