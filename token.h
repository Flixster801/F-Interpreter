#ifndef TOKEN_H
#define TOKEN_H

#include <string>

class Token {
    public:
        Token(std::string val, std::string type, int lineNum, int colNum):_value(val), _type(type), _line_number(lineNum), _col_number(colNum), _next(nullptr) {}

        std::string& value() { return _value; }
        std::string& type() { return _type; }
        int& line_number() { return _line_number; }
        int& col_number() { return _col_number; }
        Token*& next() { return _next; }

    private:
        std::string _value, _type;
        int _line_number, _col_number;

        Token* _next;
};

#endif