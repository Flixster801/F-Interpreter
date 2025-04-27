#ifndef ABSTRACT_SYNTAX_TREE_H
#define ABSTRACT_SYNTAX_TREE_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>

#include "token.h"
#include "symbol.h"
#include "node.h"
#include "symbol_table.h"

class AbstractSyntaxTree {
    public:
        AbstractSyntaxTree(std::stringstream& sstream, SymbolTable* s_table);

        bool create(Token* head_token);

        void print();

        Node* head() { return _head; }

    private:
        std::stringstream ss;
        SymbolTable* symbol_table;

        int scope;

        Node* _head;

        int precedence(Token* token);
        bool isLeftAssociative(Token* token);
        void shunting_yard(Token*& token, Node*& current);
};

#endif

// Types (header/prefix for statements)
//
// begin block
// end block
//
// declaration
// assignment
//
// if
// else
//
// while
// for
//
// print
// getchar
//
// call
// return
//
// token (anything that's not a header. use value if type is token)


// Order of Operations
//
// ()
// ! - (negative)
// ^
// * / %
// + -
// < <= > >=
// == !=
// &&
// ||
// =