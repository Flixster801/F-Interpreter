#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <iostream>
#include <sstream>
#include <cctype>

#include "token.h"

class Tokenizer {
    public:
        Tokenizer(std::stringstream& sstream);

        Token* get_token();

    private:
        std::stringstream ss;

        Token* prev;

        bool in_string, in_char;
        int line_number;
        int col_number;

        char advance();
        char convert_escaped_char(char c);
        void print_error();
};

#endif