#include "tokenizer.h"

Tokenizer::Tokenizer(std::stringstream& sstream) {
    ss.str(sstream.str());
    ss >> std::noskipws;
    prev = nullptr;
    in_string = false;
    in_char = false;
    line_number = 1;
    col_number = 1;
}

Token* Tokenizer::get_token() {
    Token* token = nullptr;

    if (prev && prev->type() == "EOF") {
        return token;
    }

    char c = advance();

    if (ss.eof()) {
        token = new Token("", "EOF", line_number, col_number - 1);
        prev = token;
        return token;
    }

    while (iswspace(c) && !in_string && !in_char) {
        c = advance();
        if (ss.eof()) {
            token = new Token("", "EOF", line_number, col_number - 1);
            prev = token;
            return token;
        }
    }

    int start_col = col_number - 1;

    // if (ss.eof()) {
    //     token = new Token("", "EOF", line_number, start_col);
    // }

    if (in_char && prev->type() == "SINGLE_QUOTE") {
        if (c == '\\') {
            token = new Token(std::string(1, convert_escaped_char(advance())), "ESCAPED_CHARACTER", line_number, start_col);
        } else {
            token = new Token(std::string(1, c), "CHARACTER", line_number, start_col);
        }
    }

    else if (in_string && prev->type() == "DOUBLE_QUOTE") {
        std::string s = "";
        int start_line = line_number;
        while (c != '"') {
            if (c == '\\') {
                s = s + convert_escaped_char(advance());
            } else {
                s = s + c;
            }
            c = advance();
        }
        ss.putback(c);
        col_number -= 1;
        token = new Token(s, "STRING", start_line, start_col);
    }

    else if (isalpha(c) || c == '_') {
        std::string s;
        s = c;
        while (isalpha(ss.peek()) || isdigit(ss.peek()) || ss.peek() == '_') {
            c = advance();
            s = s + c;
        }
        if (s == "TRUE") {
            token = new Token("TRUE", "BOOLEAN_TRUE", line_number, start_col);
        } else if (s == "FALSE") {
            token = new Token("FALSE", "BOOLEAN_FALSE", line_number, start_col);
        } else {
            token = new Token(s, "IDENTIFIER", line_number, start_col);
        }
    }

    else if (isdigit(c)) {
        std::string s;
        s = std::string(1, c);
        while (isdigit(ss.peek())) {
            c = advance();
            s = s + std::string(1, c);
        }
        token = new Token(s, "WHOLE_NUMBER", line_number, start_col);
    }

    else switch (c) {
        case '(':
            token = new Token("(", "L_PAREN", line_number, start_col); break;
        case ')':
            token = new Token(")", "R_PAREN", line_number, start_col); break;
        case '[':
            token = new Token("[", "L_BRACKET", line_number, start_col); break;
        case ']':
            token = new Token("]", "R_BRACKET", line_number, start_col); break;
        case '{':
            token = new Token("{", "L_BRACE", line_number, start_col); break;
        case '}':
            token = new Token("}", "R_BRACE", line_number, start_col); break;
        case '\'':
            token = new Token("'", "SINGLE_QUOTE", line_number, start_col); in_char = !in_char; break;
        case '"':
            token = new Token("\"", "DOUBLE_QUOTE", line_number, start_col); in_string = !in_string; break;
        case ';':
            token = new Token(";", "SEMICOLON", line_number, start_col); break;
        case ',':
            token = new Token(",", "COMMA", line_number, start_col); break;
        case '=':
            if (ss.peek() == '=') {
                advance();
                token = new Token("==", "BOOLEAN_EQUAL", line_number, start_col); break;
            } else {
                token = new Token("=", "ASSIGNMENT_OPERATOR", line_number, start_col); break;
            }
        case '+':
            token = new Token("+", "PLUS", line_number, start_col); break;
        case '-':
            while (iswspace(ss.peek())) { c = advance(); }
            if (isdigit(ss.peek()) && (prev == nullptr || prev->type() == "L_PAREN" || prev->type() == "PLUS" || prev->type() == "MINUS" ||
                                       prev->type() == "ASTERISK" || prev->type() == "DIVIDE" || prev->type() == "MODULO" ||
                                       prev->type() == "CARET" || prev->type() == "ASSIGNMENT_OPERATOR" || prev->type() == "LT" ||
                                       prev->type() == "LT_EQUAL" || prev->type() == "GT" || prev->type() == "GT_EQUAL" ||
                                       prev->type() == "BOOLEAN_EQUAL" || prev->type() == "BOOLEAN_NOT_EQUAL")) {
                c = advance();
                std::string s = "-";
                s = s + std::string(1, c);
                while (isdigit(ss.peek())) {
                    c = advance();
                    s = s + std::string(1, c);
                }
                token = new Token(s, "NEGATIVE_NUMBER", line_number, start_col); break;
            } else {
                token = new Token("-", "MINUS", line_number, start_col); break;
            }
        case '*':
            token = new Token("*", "ASTERISK", line_number, start_col); break;
        case '/':
            token = new Token("/", "DIVIDE", line_number, start_col); break;
        case '%':
            token = new Token("%", "MODULO", line_number, start_col); break;
        case '^':
            token = new Token("^", "CARET", line_number, start_col); break;
        case '<':
            if (ss.peek() == '=') {
                advance();
                token = new Token("<=", "LT_EQUAL", line_number, start_col); break;
            } else {
                token = new Token("<", "LT", line_number, start_col); break;
            }
        case '>':
            if (ss.peek() == '=') {
                advance();
                token = new Token(">=", "GT_EQUAL", line_number, start_col); break;
            } else {
                token = new Token(">", "GT", line_number, start_col); break;
            }
        case '&':
            if (ss.peek() == '&') {
                advance();
                token = new Token("&&", "BOOLEAN_AND", line_number, start_col); break;
            }
            print_error();
        case '|':
            if (ss.peek() == '|') {
                advance();
                token = new Token("||", "BOOLEAN_OR", line_number, start_col); break;
            }
            print_error();
        case '!':
            if (ss.peek() == '=') {
                advance();
                token = new Token("!=", "BOOLEAN_NOT_EQUAL", line_number, start_col); break;
            } else {
                token = new Token("!", "BOOLEAN_NOT", line_number, start_col); break;
            }
    }

    prev = token;
    return token;
}

char Tokenizer::advance() {
    char c;
    ss >> c;
    col_number += 1;
    if (c == '\n') {
        line_number += 1;
        col_number = 1;
    }
    return c;
}

char Tokenizer::convert_escaped_char(char c) {
    switch (c) {
        case '\\': return '\\';
        case 'n': return '\n';
        case '\'': return '\'';
        case '"': return  '"';
        case 'x':
            if (isxdigit(ss.peek())) {
                std::string h;
                h = advance();
                if (isxdigit(ss.peek())) {
                    h = h + advance();
                }
                return char(std::stoi(h, nullptr, 16));
            }
        default:
            print_error();
            exit(2);
    }
}

void Tokenizer::print_error() {
    std::string line;
    std::stringstream sstream(ss.str());
    for (int i = 0; i < line_number; i++) {
        std::getline(sstream, line);
    }
    std::cerr << "[" << line_number << ":" << col_number << "] " << "error: unexpected character" << std::endl;
    std::cerr << std::string(3, ' ') << line_number << " | " << line << std::endl;
    std::cerr << std::string(3 + std::to_string(line_number).length(), ' ') << " | " << std::string(col_number - 2, ' ') << "^" << std::endl;
    exit(2);
}