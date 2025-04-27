#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <iostream>
#include <vector>
#include <stack>
#include <math.h>
#include <cctype>

#include "symbol_table.h"
#include "symbol.h"
#include "abstract_syntax_tree.h"
#include "node.h"

class Interpreter {
    public:
        Interpreter(AbstractSyntaxTree* ast, SymbolTable* symbol_table): ast(ast), symbol_table(symbol_table) {
            scope_stack.push(0);
        }

        bool run();

    private:
        AbstractSyntaxTree* ast;
        SymbolTable* symbol_table;

        std::vector<Node*> functions;
        std::stack<int> scope_stack;

        Node* process_function(Node* node);
        bool process_block(Node*& node);
        bool process_statement(Node*& node);

        void process_declaration(Node*& node);
        void process_assignment(Node*& node);

        bool process_if(Node*& node);
        bool process_while(Node*& node);
        bool process_for(Node*& node);

        void process_print(Node*& node);
        Node* process_getchar(Node*& node);

        Node* process_call(Node*& node);
        void process_return(Node*& node);

        Node* evaluate(Node*& node);
        Node* solver(Node* one, Node* two, Node* three);

        void skip_block(Node*& node);
        void skip_statement(Node*& node);

        bool is_operator(Node* node);
};

#endif