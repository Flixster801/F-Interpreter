#include "abstract_syntax_tree.h"

AbstractSyntaxTree::AbstractSyntaxTree(std::stringstream& sstream, SymbolTable* s_table) {
    ss.str(sstream.str());
    symbol_table = s_table;
    scope = 0;
}

bool AbstractSyntaxTree::create(Token* head_token) {
    _head = new Node("", "", nullptr, -1, -1); // dummy head

    Token* token = head_token;
    Node* current = _head;

    while (token != nullptr && token->type() != "EOF") {
        // begin block
        if (token->type() == "L_BRACE") {
            current->child() = new Node("BEGIN_BLOCK", "", nullptr, token->line_number(), token->col_number());
            current = current->child();
            token = token->next();
        }

        // end block
        else if (token->type() == "R_BRACE") {
            current->child() = new Node("END_BLOCK", "", nullptr, token->line_number(), token->col_number());
            current = current->child();
            token = token->next();
        }

        else if (token->type() == "IDENTIFIER") {
            // procedure declaration
            if (token->value() == "void") {
                scope += 1;
                token = token->next(); // to name
                current->child() = new Node("DECLARATION", "", symbol_table->get_symbol(token->value(), scope), token->line_number(), token->col_number());
                current = current->child();
                while (token->type() != "R_PAREN") { // skip to end
                    token = token->next();
                }
                token = token->next(); // go to next token
            }

            // function / variable declaration
            else if (token->value() == "int" || token->value() == "bool" || token->value() == "char") {
                token = token->next(); // to name
                std::string name = token->value();
                token = token->next(); // to either , ; [ (
                
                // function declaration
                if (token->type() == "L_PAREN") {
                    scope += 1;
                    current->child() = new Node("DECLARATION", "", symbol_table->get_symbol(name, scope), token->line_number(), token->col_number());
                    current = current->child();
                    while (token->type() != "R_PAREN") { // skip to end
                        token = token->next();
                    }
                    token = token->next(); // go to next token
                }

                // variable declaration
                else {
                    current->child() = new Node("DECLARATION", "", symbol_table->get_symbol(name, scope), token->line_number(), token->col_number());
                    current = current->child();

                    if (token->type() == "L_BRACKET") {
                        token = token->next()->next()->next(); // skip to , or ;
                    }

                    while (token->type() == "COMMA") {
                        token = token->next();

                        current->child() = new Node("DECLARATION", "", symbol_table->get_symbol(token->value(), scope), token->line_number(), token->col_number());
                        current = current->child();

                        token = token->next();
                        if (token->type() == "L_BRACKET") {
                            token = token->next()->next()->next(); // skip to , or ;
                        }
                    }
                    token = token->next(); 
                }
            }

            // if statement
            else if (token->value() == "if") {
                current->child() = new Node("IF", "", nullptr, token->line_number(), token->col_number());
                current = current->child();
                token = token->next()->next();
                shunting_yard(token, current);
                token = token->next();
            }

            // else statement
            else if (token->value() == "else") {
                current->child() = new Node("ELSE", "", nullptr, token->line_number(), token->col_number());
                current = current->child();
                token = token->next();
            }

            // while loop
            else if (token->value() == "while") {
                current->child() = new Node("WHILE", "", nullptr, token->line_number(), token->col_number());
                current = current->child();
                token = token->next()->next();
                shunting_yard(token, current);
                token = token->next();
            }

            // for loop
            else if (token->value() == "for") {
                current->child() = new Node("FOR1", "", nullptr, token->line_number(), token->col_number());
                current = current->child();
                token = token->next()->next();
                shunting_yard(token, current);
                token = token->next();

                current->child() = new Node("FOR2", "", nullptr, token->line_number(), token->col_number());
                current = current->child();
                shunting_yard(token, current);
                token = token->next();

                current->child() = new Node("FOR3", "", nullptr, token->line_number(), token->col_number());
                current = current->child();
                shunting_yard(token, current);
                token = token->next();
            }

            // print statement
            else if (token->value() == "print") {
                current->child() = new Node("PRINT", "", nullptr, token->line_number(), token->col_number());
                current = current->child();
                token = token->next()->next(); // after (

                current->next() = new Node("", "\"", nullptr, token->line_number(), token->col_number());
                current = current->next();
                token = token->next(); // after "

                current->next() = new Node("", token->value(), nullptr, token->line_number(), token->col_number());
                current = current->next();
                token = token->next();

                current->next() = new Node("", "\"", nullptr, token->line_number(), token->col_number());
                current = current->next();
                token = token->next(); // after "

                while (token->type() != "R_PAREN") {
                    shunting_yard(token, current);
                    if (token->type() == "COMMA") {
                        current->next() = new Node("", ",", nullptr, token->line_number(), token->col_number());
                        current = current->next();
                        token = token->next();
                    }
                }
                token = token->next()->next();
            }

            // return statement
            else if (token->value() == "return") {
                current->child() = new Node("RETURN", "", nullptr, token->line_number(), token->col_number());
                current = current->child();
                token = token->next();
                shunting_yard(token, current);
                token = token->next();
            }

            // assignment / call
            else {
                // assignment
                if (token->next()->type() == "ASSIGNMENT_OPERATOR" || token->next()->type() == "L_BRACKET") {
                    current->child() = new Node("ASSIGNMENT", "", symbol_table->get_symbol(token->value(), scope), token->next()->line_number(), token->next()->col_number());
                    current = current->child();
                    shunting_yard(token, current);
                    token = token->next();
                }

                // function call
                else if (token->next()->type() == "L_PAREN") { // extend
                    current->child() = new Node("CALL", "", symbol_table->get_symbol(token->value(), scope), token->next()->line_number(), token->next()->col_number());
                    current = current->child();
                    shunting_yard(token, current);
                    token = token->next();
                    
                }
                
                // error (shouldn't ever get called)
                else {
                    std::cerr << "[" << token->line_number() << ":" << token->col_number() << "] error: expected a statement-" << std::endl;
                    return false;
                }
            }
        }

        // error (shouldn't ever get called)
        else {
            std::cerr << "-[" << token->line_number() << ":" << token->col_number() << "] error: expected a statement" << std::endl;
            return false;
        }
    }

    _head = _head->child(); // remove dummy head

    return true;
}

int AbstractSyntaxTree::precedence(Token* token) {
    if (token->value() == "!") {
        return 9;
    } else if (token->value() == "^") {
        return 8;
    } else if (token->value() == "*" || token->value() == "/" || token->value() == "%") {
        return 7;
    } else if (token->value() == "+" || token->value() == "-") {
        return 6;
    } else if (token->value() == "<" || token->value() == "<=" || token->value() == ">" || token->value() == ">=") {
        return 5;
    } else if (token->value() == "==" || token->value() == "!=") {
        return 4;
    } else if (token->value() == "&&") {
        return 3;
    } else if (token->value() == "||") {
        return 2;
    } else if (token->value() == "=") {
        return 1;
    } else if (token->value() == "(" || token->value() == ")") {
        return 0;
    } else {
        return -1;
    }
}

bool AbstractSyntaxTree::isLeftAssociative(Token* token) {
    if (token->value() == "!" || token->value() == "^") {
        return false;
    }
    return true;
}

void AbstractSyntaxTree::shunting_yard(Token*& token, Node*& current) {
    std::stack<Token*> stack;
    int level = 1;

    while (level > 0 && token->type() != "SEMICOLON" && token->type() != "COMMA" && token->type() != "R_BRACKET") {
        if (token->type() == "IDENTIFIER" && token->value() == "getchar") {
            current->next() = new Node("GETCHAR", "", nullptr, token->line_number(), token->col_number());
            current = current->next();
            token = token->next()->next()->next(); // getchar() <- skip entire function call
        }

        else if (token->type() == "IDENTIFIER" && token->next()->type() == "L_PAREN") {
            Symbol* s = symbol_table->get_symbol(token->value(), scope);
            if (s == nullptr) {
                std::cerr << "error: undefined function\n";
                exit(5);
            }
            current->next() = new Node("", token->value(), s, token->line_number(), token->col_number());
            current = current->next();
            current->next() = new Node("", "(", nullptr, token->line_number(), token->col_number());
            current = current->next();
            token = token->next()->next(); // after (
            while (token->type() != "R_PAREN") {
                shunting_yard(token, current);
                if (token->type() == "COMMA") {
                    current->next() = new Node("", ",", nullptr, token->line_number(), token->col_number());
                    current = current->next();
                    token = token->next();
                }
            }
            current->next() = new Node("", ")", nullptr, token->line_number(), token->col_number());
            current = current->next();
            token = token->next();
        }

        else if (token->type() == "L_BRACKET") {
            current->next() = new Node("", "[", symbol_table->get_symbol(token->value(), scope), token->line_number(), token->col_number());
            current = current->next();
            token = token->next();
            shunting_yard(token, current);
            current->next() = new Node("", "]", symbol_table->get_symbol(token->value(), scope), token->line_number(), token->col_number());
            current = current->next();
            token = token->next();
        }

        else if (precedence(token) == -1) {
            Symbol* s = symbol_table->get_symbol(token->value(), scope);
            if (s == nullptr && token->type() == "IDENTIFIER") {
                std::cerr << "error: undefined variable\n";
                exit(5);
            }
            current->next() = new Node("", token->value(), s, token->line_number(), token->col_number());
            current = current->next();
            token = token->next();
        }

        else if (token->type() == "L_PAREN") {
            level += 1;
            stack.push(token);
            token = token->next();
        }

        else if (token->type() == "R_PAREN") {
            level -= 1;
            if (level == 0) { break; }
            while(!stack.empty() && stack.top()->type() != "L_PAREN") {
                current->next() = new Node("", stack.top()->value(), nullptr, stack.top()->line_number(), stack.top()->col_number());
                current = current->next();
                stack.pop();
            }
            stack.pop();
            token = token->next();
        }

        else if (precedence(token) > 0) {
            while (!stack.empty() && (precedence(token) < precedence(stack.top()) || (precedence(token) == precedence(stack.top()) && isLeftAssociative(token)))) {
                current->next() = new Node("", stack.top()->value(), nullptr, stack.top()->line_number(), stack.top()->col_number());
                current = current->next();
                stack.pop();
            }
            stack.push(token);
            token = token->next();
        }

        else {
            std::cerr << "uh oh\n";
        }
    }

    while (!stack.empty()) {
        current->next() = new Node("", stack.top()->value(), nullptr, stack.top()->line_number(), stack.top()->col_number());
        current = current->next();
        stack.pop();
    }
}

void AbstractSyntaxTree::print() {
    Node* current = _head;
    int length = 0;
    int buffer = 0;

    std::ofstream out("out.txt", std::ios::out);

    while (current != nullptr) {
        if (current->next() != nullptr) {
            out << current->type() << current->value() << /*(current->symbol() == nullptr ? "" : " (" + current->symbol()->name() + ", " + std::to_string(current->symbol()->array_size()) + ")") <<*/ " → ";
            buffer += current->type().length() + current->value().length() + 3 /*+ (current->symbol() == nullptr ? 0 : current->symbol()->name().length() + std::to_string(current->symbol()->array_size()).length() + 5)*/;
            current = current->next();
        } else {
            out << current->type() << current->value() << /*(current->symbol() == nullptr ? "" : " (" + current->symbol()->name() + ", " + std::to_string(current->symbol()->array_size()) + ")") <<*/ std::endl;
            length = buffer;
            if (current->child() != nullptr) {
                out << std::string(length, ' ') << "↓" << std::endl;
                out << std::string(length, ' ');
            }
            current = current->child();
        }
    }
}