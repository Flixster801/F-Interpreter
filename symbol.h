#ifndef SYMBOL_H
#define SYMBOL_H

#include <string>

class Symbol {
    public:
        Symbol(std::string name, std::string type, std::string datatype, int array_size, int scope, int line_number, int col_number):
               _name(name), _type(type), _datatype(datatype), _array_size(array_size), _scope(scope), _line_number(line_number), _col_number(col_number) {
            _next = nullptr;
            _defined = false;
            _paramters = nullptr;
            if (array_size > 0) {
                _value = new int[array_size];
                for (int i = 0; i < array_size; i++) {
                    _value[i] = 0;
                }
            } else {
                _value = new int[1];
                _value[0] = 0;
            }
        }

        std::string name() { return _name; }
        std::string type() { return _type; }
        std::string datatype() { return _datatype; }

        int*& value() { return _value; }
        int array_size() { return _array_size; }

        int scope() { return _scope; }
        int line_number() { return _line_number; }
        int col_number() { return _col_number; }
        bool& defined() { return _defined; }

        Symbol*& next() { return _next; }
        Symbol*& parameters() { return _paramters; }

    private:
        std::string _name;
        std::string _type;
        std::string _datatype;

        int* _value;
        int _array_size;

        int _scope;
        int _line_number;
        int _col_number;
        bool _defined;

        Symbol* _next;
        Symbol* _paramters;
};

#endif