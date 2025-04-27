#include "symbol_table.h"

SymbolTable::SymbolTable(std::stringstream& sstream) {
    ss.str(sstream.str());
    scope_counter = 0;
    level = 0;
    can_return = false;
}

void SymbolTable::print(Symbol* start) {
    Symbol* current;
    if (start == nullptr) {
        current = head;
    } else {
        current = start;
    }

    while (current != nullptr) {
        std::cout << "NAME: " << current->name() << std::endl;
        std::cout << "TYPE: " << current->type() << std::endl;
        std::cout << "DATATYPE: " << current->datatype() << std::endl;
        std::cout << "ARRAY_SIZE: " << current->array_size() << std::endl;
        std::cout << "SCOPE: " << current->scope() << std::endl;
        std::cout << "LINE_NUMBER: " << current->line_number() << std::endl;
        std::cout << "COL_NUMBER: " << current->col_number() << std::endl << std::endl;
        if (current->parameters() != nullptr) {
            print(current->parameters());
        }
        current = current->next();
    }
}

bool SymbolTable::create(Token* head_token) {
    // create dummy head
    head = new Symbol("", "", "", 0, -1, -1, -1);

    Symbol* current = head;
    Token* token = head_token;

    while (token != nullptr && token->type() != "EOF") {
        std::string name, type, datatype;
        int array_size, scope, line_number, col_number;
        
        // non returning function (procedure)
        if (token->type() == "IDENTIFIER" && token->value() == "void") {
            can_return = false;
            scope_counter += 1;
            type = "procedure";
            datatype = "void";
            array_size = 0;
            scope = scope_counter;
            token = token->next(); // to func name
            name = token->value();
            line_number = token->line_number();
            col_number = token->col_number();
            token = token->next()->next(); // to token after (

            if (!func_exists(name)) {
                current->next() = new Symbol(name, type, datatype, array_size, scope, line_number, col_number);
                current = current->next();
            } else {
                print_error(name, line_number, col_number);
                return false;
            }

            // dummy
            Symbol* param_head = new Symbol("", "", "", 0, -1, -1, -1);
            Symbol* param_cur = param_head;

            // parameters
            while (token->type() != "R_PAREN") {
                type = "datatype";
                datatype = token->value();
                token = token->next(); // to identifier name
                name = token->value();
                line_number = token->line_number();
                col_number = token->col_number();

                token = token->next(); // to either , or )

                if (get_symbol(name, scope) == nullptr) {
                    param_cur->next() = new Symbol(name, type, datatype, array_size, scope, line_number, col_number);
                    param_cur = param_cur->next();
                    param_cur->defined() = true;
                } else {
                    print_error(name, line_number, col_number);
                    return false;
                }

                if (token->type() == "COMMA") {
                    token = token->next();
                }
            }

            current->parameters() = param_head->next();
        }

        // function / variable declaraction
        else if (token->type() == "IDENTIFIER" && (token->value() == "int" || token->value() == "bool" || token->value() == "char")) {
            datatype = token->value();
            token = token->next(); // to name
            name = token->value();
            line_number = token->line_number();
            col_number = token->col_number();
            token = token->next(); // to either , ; ( [

            // function
            if (token->type() == "L_PAREN") {
                can_return = true;
                scope_counter += 1;
                type = "function";
                array_size = 0;
                scope = scope_counter;
                token = token->next(); // to token after (

                if (!func_exists(name)) {
                    current->next() = new Symbol(name, type, datatype, array_size, scope, line_number, col_number);
                    current = current->next();
                } else {
                    print_error(name, line_number, col_number);
                    return false;
                }

                // dummy
                Symbol* param_head = new Symbol("", "", "", 0, -1, -1, -1);
                Symbol* param_cur = param_head;

                // parameters
                while (token->type() != "R_PAREN") {
                    type = "datatype";
                    datatype = token->value();
                    token = token->next(); // to identifier name
                    name = token->value();
                    line_number = token->line_number();
                    col_number = token->col_number();

                    token = token->next(); // to either , )

                    if (get_symbol(name, scope) == nullptr) {
                        param_cur->next() = new Symbol(name, type, datatype, array_size, scope, line_number, col_number);
                        param_cur = param_cur->next();
                        param_cur->defined() = true;
                    } else {
                        print_error(name, line_number, col_number);
                        return false;
                    }

                    if (token->type() == "COMMA") {
                        token = token->next();
                    }
                }

                current->parameters() = param_head->next();
            }

            // variable declaration
            else {
                type = "datatype";
                array_size = 0;
                // is an array
                if (token->type() == "L_BRACKET") {
                    token = token->next();
                    array_size = std::stoi(token->value());
                    token = token->next()->next(); // to token after ], either , or ;

                    if (array_size == 0) {
                        std::cerr << "error: array size cannot be 0" << std::endl;
                        return false;
                    }
                }

                // is global
                if (level == 0) {
                    scope = 0;
                }

                if (get_symbol(name, scope) == nullptr) {
                    current->next() = new Symbol(name, type, datatype, array_size, scope, line_number, col_number);
                    current = current->next();
                } else {
                    print_error(name, line_number, col_number);
                    return false;
                }

                // multiple declarations
                while (token->type() == "COMMA") {
                    array_size = 0; // reset array size
                    token = token->next(); // to identifier name
                    name = token->value();
                    line_number = token->line_number();
                    col_number = token->col_number();

                    token = token->next(); // to either , [ ;

                    // array
                    if (token->type() == "L_BRACKET") {
                        token = token->next();
                        array_size = std::stoi(token->value());
                        token = token->next()->next(); // to token after ], either , or ;

                        if (array_size == 0) {
                            std::cerr << "error: array size cannot be 0" << std::endl;
                            return false;
                        }
                    }

                    if (get_symbol(name, scope) == nullptr) {
                        current->next() = new Symbol(name, type, datatype, array_size, scope, line_number, col_number);
                        current = current->next();
                    } else {
                        print_error(name, line_number, col_number);
                        return false;
                    }
                }
            }
        }

        else if (token->type() == "L_BRACE") {
            level += 1;
            token = token->next();
        }

        else if (token->type() == "R_BRACE") {
            level -= 1;
            token = token->next();
        }

        else if (token->type() == "IDENTIFIER" && token->value() == "return") {
            if (!can_return && token->next()->type() != "SEMICOLON") {
                std::cerr << "[" << token->line_number() << ":" << token->col_number() << "] error: cannot return in void function" << std::endl;

                std::string line;
                std::stringstream sstream(ss.str());
                for (int i = 0; i < token->line_number(); i++) {
                    std::getline(sstream, line);
                }

                std::cerr << std::string(3, ' ') << token->line_number() << " | " << line << std::endl;
                std::cerr << std::string(std::to_string(token->line_number()).length() + 3, ' ') << " | " <<
                            std::string(token->col_number() - 1, ' ') << "^" + std::string(token->value().length() - 1, '~') << std::endl;
                
                return false;
            }
            token = token->next();
        }

        else {
            token = token->next();
        }
    }
    
    head = head->next(); // get rid of temp head;

    return true;
}

Symbol* SymbolTable::get_symbol(std::string name, int scope, Symbol* start) {
    Symbol* current;
    if (start == nullptr) {
        current = head;
    } else {
        current = start;
    }

    while (current != nullptr) {
        if (current->name() == name && (current->scope() == scope || current->scope() == 0 || current->type() == "function" || current->type() == "procedure")) {
            return current;
        }
        if (current->parameters() != nullptr) {
            Symbol* result = get_symbol(name, scope, current->parameters());
            if (result != nullptr) {
                return result;
            }
        }
        current = current->next();
    }
    return nullptr;
}

bool SymbolTable::func_exists(std::string name) {
    Symbol* current = head;
    while (current != nullptr) {
        if (current->name() == name && (current->type() == "function" || current->type() == "procedure")) {
            return true;
        }
        current = current->next();
    }
    return false;
}

void SymbolTable::print_error(std::string name, int line_number, int col_number) {
    std::cerr << "[" << line_number << ":" << col_number << "] error: duplicate declaration" << std::endl;

    std::string line;
    std::stringstream sstream(ss.str());
    for (int i = 0; i < line_number; i++) {
        std::getline(sstream, line);
    }

    std::cerr << std::string(3, ' ') << line_number << " | " << line << std::endl;
    std::cerr << std::string(std::to_string(line_number).length() + 3, ' ') << " | " <<
                 std::string(col_number - 1, ' ') << "^" + std::string(name.length() - 1, '~') << std::endl;
}