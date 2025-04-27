#ifndef NODE_H
#define NODE_H

#include <string>

#include "symbol.h"

class Node {
    public:
        Node(std::string type, std::string value, Symbol* symbol, int line_number, int col_number): 
        _type(type), _value(value), _symbol(symbol), _line_number(line_number), _col_number(col_number) {
            _next = nullptr;
            _child = nullptr;
            _index = -1;
        }

        std::string type() { return _type; }
        std::string value() { return _value; }

        int line_number() { return _line_number; }
        int col_number() { return _col_number; }

        Symbol* symbol() { return _symbol; }

        Node*& next() { return _next; }
        Node*& child() { return _child; }

        int& index() { return _index; }

    private:
        std::string _type, _value;
        int _line_number, _col_number;

        Symbol* _symbol;
        Node* _next;
        Node*_child;

        int _index; // for symbol
};

#endif