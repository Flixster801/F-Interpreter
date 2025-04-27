#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <iostream>
#include <sstream>

#include "token.h"
#include "symbol.h"

class SymbolTable {
    public:
        SymbolTable(std::stringstream& sstream);

        bool create(Token* head_token);

        Symbol* get_symbol(std::string name, int scope, Symbol* start = nullptr);

        void print(Symbol* start = nullptr);

    private:
        Symbol* head;

        std::stringstream ss;

        int scope_counter, level;
        bool can_return;

        bool func_exists(std::string name);
        void print_error(std::string name, int line_number, int col_number);
};

#endif