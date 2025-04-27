#ifndef RECURSIVE_DESCENT_PARSER_H
#define RECURSIVE_DESCENT_PARSER_H

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "token.h"

class RecursiveDescentParser {
    public:
        RecursiveDescentParser();

        bool check_errors(Token* head, std::stringstream& ss);

    private:
        Token* error_token;
        std::string error_message;
        int error_depth, max_error_depth;

        std::vector<std::string> path, saved_path;

        void setErrorMessage(std::string message, Token* token);

        bool isType(Token*& token, std::string type);
        bool isValue(Token*& token, std::string value);

        bool isDoubleQuotedString(Token*& token);
        bool isIdentifier(Token*& token);
        bool isSimpleIdentifierArray(Token*& token);
        bool isIdentifierArray(Token*& token);
        bool isSimpleIdentifierAndArrayList(Token*& token);
        bool isIdentifierAndArrayList(Token*& token);
        bool isDatatypeSpecifier(Token*& token);
        bool isNumericalOperand(Token*& token);
        bool isNumericalOperator(Token*& token);
        bool isNumericalExpression(Token*& token);
        bool isBooleanOperand(Token*& token);
        bool isBooleanOperator(Token*& token);
        bool isRelationalExpression(Token*& token);
        bool isBooleanExpression(Token*& token);
        bool isExpressionList(Token*& token);
        bool isGetcharFunction(Token*& token);
        bool isUserDefinedFunction(Token*& token);
        bool isInitializationExpression(Token*& token);
        bool isDeclarationStatement(Token*& token);
        bool isAssignmentStatement(Token*& token);
        bool isSelectionStatement(Token*& token);
        bool isIterationStatement(Token*& token);
        bool isPrintStatement(Token*& token);
        bool isReturnStatement(Token*& token);
        bool isStatement(Token*& token);
        bool isCompoundStatement(Token*& token);
        bool isBlockStatement(Token*& token);
        bool isParameterList(Token*& token);
        bool isFunctionDeclaration(Token*& token);
        bool isNonReturningFunctionDeclaration(Token*& token);
        bool isMainDeclaration(Token*& token);
        bool isProgram(Token*& token);
};

#endif