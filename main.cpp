#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

#include "remove_comments.h"
#include "token.h"
#include "tokenizer.h"
#include "recursive_descent_parser.h"
#include "symbol_table.h"
#include "abstract_syntax_tree.h"
#include "interpreter.h"

int main(int argc, char* argv[]) {
    // Ensures input file was passed as argument
    if (argc < 2) {
        std::cout << "error: no input file specified" << std::endl;
        return 1;
    }

    // Check file uses .f extension
    if (argv[1][strlen(argv[1]) - 2] != '.' || argv[1][strlen(argv[1]) - 1] != 'f') {
        std::cerr << "error: input file must end in '.f'" << std::endl;
        return 1;
    }

    // Open the file
    std::ifstream in(argv[1], std::ios::in);
    if (!in.is_open()) {
        std::cerr << "error: could not open input file" << std::endl;
        return 1;
    }

    // Load file contents to stringstream
    std::stringstream ss;
    ss >> std::noskipws;
    ss << in.rdbuf();
    in.close();

    // Remove comments
    if (!remove_comments(ss)) {
        return 2;
    }

    // std::cout << ss.str() << std::endl;

    // Tokenize
    Tokenizer tokenizer(ss);

    // Make linked list of tokens
    Token* head = tokenizer.get_token();
    Token* token = tokenizer.get_token();
    Token* prev = head;
    while (token) {
        prev->next() = token;
        prev = token;
        token = tokenizer.get_token();
    }

    // token = head;
    // while (token) {
    //     std::cout << token->value() << " " << token->type() << " " << token->line_number() << " " << token->col_number() << std::endl;
    //     token = token->next();
    // }

    RecursiveDescentParser* rdp = new RecursiveDescentParser();
    
    if (!rdp->check_errors(head, ss)) {
        return 3;
    }

    SymbolTable* symbol_table = new SymbolTable(ss);

    if (!symbol_table->create(head)) {
        return 4;
    }

    // symbol_table->print();

    AbstractSyntaxTree* ast = new AbstractSyntaxTree(ss, symbol_table);

    if (!ast->create(head)) {
        return 5;
    }

    ast->print();

    Interpreter* interpreter = new Interpreter(ast, symbol_table);

    if (!interpreter->run()) {
        return 6;
    }
}