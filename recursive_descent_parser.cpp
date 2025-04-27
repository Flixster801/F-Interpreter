#include "recursive_descent_parser.h"

RecursiveDescentParser::RecursiveDescentParser() {
    error_token = new Token("-", "-", 1, 1);
    error_message = "";
    error_depth = 0;
    max_error_depth = 0;
}

bool RecursiveDescentParser::check_errors(Token* head, std::stringstream& ss) {
    Token* token = head;
    if (head->type() == "EOF" || !isProgram(token) || token->type() != "EOF") {
        if (head->type() == "EOF") {
            error_message = "error: no tokens";
        }
        else if (token->type() != "EOF" && token != head) {
            error_message = "error: exprected eof";
            error_token = token;
        }
        // for (auto p : saved_path) {
        //     std::cerr << p << std::endl;
        // }
        std::cerr << "[" << std::to_string(error_token->line_number()) + ":" + std::to_string(error_token->col_number()) << "] " <<
                     error_message << std::endl;
        std::string line;
        std::stringstream sstream(ss.str());
        for (int i = 0; i < error_token->line_number(); i++) {
            std::getline(sstream, line);
        }
        std::cerr << std::string(3, ' ') << error_token->line_number() << " | " << line << std::endl;
        std::cerr << std::string(std::to_string(error_token->line_number()).length() + 3, ' ') << " | " <<
                     std::string(error_token->col_number() - 1, ' ') << "^" + std::string(error_token->value().length() - 1, '~') << std::endl;
        return false;
    }

    return true;
}

void RecursiveDescentParser::setErrorMessage(std::string message, Token* token) {
    if (error_depth > max_error_depth) {
        max_error_depth = error_depth;
        error_message = message;
        saved_path = path;
        error_token = token;
    }
}

bool RecursiveDescentParser::isType(Token*& token, std::string type) {
    path.push_back("isType");
    if (token == nullptr) { setErrorMessage("error: unexpected eof", token);; path.pop_back(); return false; }
    if (token->type() == type) {
        error_depth += 1;
        token = token->next();
        path.pop_back();
        return true;
    }
    setErrorMessage("error: unexpected token '" + token->value() + "'", token);
    path.pop_back();
    return false;
}

bool RecursiveDescentParser::isValue(Token*& token, std::string value) {
    path.push_back("isValue");
    if (token == nullptr) { setErrorMessage("error: unexpected eof", token);; path.pop_back(); return false; }
    if (token->value() == value) {
        error_depth += 1;
        token = token->next();
        path.pop_back();
        return true;
    }
    setErrorMessage("error: unexpected token '" + token->value() + "'", token);
    path.pop_back();
    return false;
}

// <DOUBLE_QUOTE> <STRING> <DOUBLE_QUOTE>
bool RecursiveDescentParser::isDoubleQuotedString(Token*& token) {
    path.push_back("isDoubleQuotedString");
    if (token == nullptr) { setErrorMessage("error: unexpected eof", token);; path.pop_back(); return false; }
    error_depth += 1;
    int d_init = error_depth;
    Token* init = token;

    if (isType(token, "DOUBLE_QUOTE") && isType(token, "STRING") && isType(token, "DOUBLE_QUOTE")) {
        path.pop_back();
        return true;
    }

    error_depth = d_init;
    token = init;
    // setErrorMessage("ERROR: Something bad happened in isDoubleQuotedString on line " + std::to_string(token->line_number()) + ", col " + std::to_string(token->col_number()) + ".", token);
    path.pop_back();
    return false;
}

// Reserved Identifier Names
// main, void,
// int, bool, char,
// if, else, while, for,
// print, getchar,
// return
bool RecursiveDescentParser::isIdentifier(Token*& token) {
    path.push_back("isIdentifier");
    if (token == nullptr) { setErrorMessage("error: unexpected eof", token);; path.pop_back(); return false; }
    
    if (token->type() != "IDENTIFIER") {
        setErrorMessage("error: unexpected token '" + token->value() + "'", token);
        path.pop_back();
        return false;
    }

    if (token->value() == "main" || token->value() == "void" ||
        token->value() == "int" || token->value() == "bool" || token->value() == "char" ||
        token->value() == "if" || token->value() == "else" || token->value() == "while" || token->value() == "for" ||
        token->value() == "print" || token->value() == "getchar" ||
        token->value() == "return") {
        setErrorMessage("error: " + token->value() + " is a reserved identifier", token);
        path.pop_back();
        return false;
    }

    error_depth += 1;
    token = token->next();
    path.pop_back();
    return true;
}

// <IDENTIFIER> <L_BRACKET> <WHOLE_NUMBER> <R_BRACKET>
bool RecursiveDescentParser::isSimpleIdentifierArray(Token*& token) {
    path.push_back("isSimpleIdentifierArray");
    if (token == nullptr) { setErrorMessage("error: unexpected eof", token);; path.pop_back(); return false; }
    error_depth += 1;
    int d_init = error_depth;
    Token* init = token;

    error_depth = d_init;
    token = init;
    if (isIdentifier(token) && isType(token, "L_BRACKET") && isType(token, "WHOLE_NUMBER") && isType(token, "R_BRACKET")) {
        path.pop_back();
        return true;
    }

    error_depth = d_init;
    token = init;
    // setErrorMessage("ERROR: Something bad happened in isIdentifierArray on line " + std::to_string(token->line_number()) + ", col " + std::to_string(token->col_number()) + ".", token);
    path.pop_back();
    return false;
}

// <IDENTIFIER> <L_BRACKET> <NUMERICAL_EXPRESSION> <R_BRACKET>
bool RecursiveDescentParser::isIdentifierArray(Token*& token) {
    path.push_back("isIdentifierArray");
    if (token == nullptr) { setErrorMessage("error: unexpected eof", token);; path.pop_back(); return false; }
    error_depth += 1;
    int d_init = error_depth;
    Token* init = token;

    error_depth = d_init;
    token = init;
    if (isIdentifier(token) && isType(token, "L_BRACKET") && isNumericalExpression(token) && isType(token, "R_BRACKET")) {
        path.pop_back();
        return true;
    }

    error_depth = d_init;
    token = init;
    // setErrorMessage("ERROR: Something bad happened in isIdentifierArray on line " + std::to_string(token->line_number()) + ", col " + std::to_string(token->col_number()) + ".", token);
    path.pop_back();
    return false;
}

// <SIMPLE_IDENTIFIER_ARRAY> <COMMA> <SIMPLE_IDENTIFIER_AND_ARRAY_LIST> |
// <SIMPLE_IDENTIFIER_ARRAY> |
// <IDENTIFIER> <COMMA> <SIMPLE_IDENTIFIER_AND_ARRAY_LIST> |
// <IDENTIFIER>
bool RecursiveDescentParser::isSimpleIdentifierAndArrayList(Token*& token) {
    path.push_back("isSimpleIdentifierAndArrayList");
    if (token == nullptr) { setErrorMessage("error: unexpected eof", token);; path.pop_back(); return false; }
    error_depth += 1;
    int d_init = error_depth;
    Token* init = token;

    if (isSimpleIdentifierArray(token) && isType(token, "COMMA") && isSimpleIdentifierAndArrayList(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isSimpleIdentifierArray(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isIdentifier(token) && isType(token, "COMMA") && isSimpleIdentifierAndArrayList(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isIdentifier(token)) {
        path.pop_back();
        return true;
    }

    error_depth = d_init;
    token = init;
    // setErrorMessage("ERROR: Something bad happened in isIdentiferAndArrayList on line " + std::to_string(token->line_number()) + ", col " + std::to_string(token->col_number()) + ".", token);
    path.pop_back();
    return false;
}

// <IDENTIFIER_ARRAY> <COMMA> <IDENTIFIER_AND_ARRAY_LIST>
// <IDENTIFIER_ARRAY> |
// <IDENTIFIER> <COMMA> <IDENTIFIER_AND_ARRAY_LIST> |
// <IDENTIFIER> |
bool RecursiveDescentParser::isIdentifierAndArrayList(Token*& token) {
    path.push_back("isIdentifierAndArrayList");
    if (token == nullptr) { setErrorMessage("error: unexpected eof", token);; path.pop_back(); return false; }
    error_depth += 1;
    int d_init = error_depth;
    Token* init = token;

    if (isIdentifierArray(token) && isType(token, "COMMA") && isIdentifierAndArrayList(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isIdentifierArray(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isIdentifier(token) && isType(token, "COMMA") && isIdentifierAndArrayList(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isIdentifier(token)) {
        path.pop_back();
        return true;
    }

    error_depth = d_init;
    token = init;
    // setErrorMessage("ERROR: Something bad happened in isIdentiferAndArrayList on line " + std::to_string(token->line_number()) + ", col " + std::to_string(token->col_number()) + ".", token);
    path.pop_back();
    return false;
}

// int | bool | char
bool RecursiveDescentParser::isDatatypeSpecifier(Token*& token) {
    path.push_back("isDatatypeSpecifier");
    if (token == nullptr) { setErrorMessage("error: unexpected eof", token);; path.pop_back(); return false; }

    if (token->value() == "int" || token->value() == "bool" || token->value() == "char") {
        error_depth += 1;
        token = token->next();
        path.pop_back();
        return true;
    }

    setErrorMessage("error: unexpected token '" + token->value() + "'", token);
    path.pop_back();
    return false;
}

// <WHOLE_NUMBER> |
// <NEGATIVE_NUMBER> |
// <GETCHAR_FUNCTION> |
// <USER_DEFINED_FUNCTION> |
// <IDENTIFIER_ARRAY> |
// <IDENTIFIER> |
// <SINGLE_QUOTE> <CHARACTER> <SINGLE_QUOTE> |
// <SINGLE_QUOTE> <ESCAPED_CHARACTER> <SINGLE_QUOTE>
bool RecursiveDescentParser::isNumericalOperand(Token*& token) {
    path.push_back("isNumericalOperand");
    if (token == nullptr) { setErrorMessage("error: unexpected eof", token);; path.pop_back(); return false; }
    error_depth += 1;
    int d_init = error_depth;
    Token* init = token;

    if (isType(token, "WHOLE_NUMBER")) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isType(token, "NEGATIVE_NUMBER")) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isGetcharFunction(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isUserDefinedFunction(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isIdentifierArray(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isIdentifier(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isType(token, "SINGLE_QUOTE") && isType(token, "CHARACTER") && isType(token, "SINGLE_QUOTE")) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isType(token, "SINGLE_QUOTE") && isType(token, "ESCAPED_CHARACTER") && isType(token, "SINGLE_QUOTE")) {
        path.pop_back();
        return true;
    }

    error_depth = d_init;
    token = init;
    // setErrorMessage("ERROR: Something bad happened in isNumericalOperand on line " + std::to_string(token->line_number()) + ", col " + std::to_string(token->col_number()) + ".", token);
    path.pop_back();
    return false;
}

// <PLUS> | <MINUS> | <ASTERISK> | <DIVIDE> | <MODULO> | <CARET>
bool RecursiveDescentParser::isNumericalOperator(Token*& token) {
    path.push_back("isNumericalOperator");
    if (token == nullptr) { setErrorMessage("error: unexpected eof", token);; path.pop_back(); return false; }
    error_depth += 1;
    int d_init = error_depth;
    Token* init = token;
    
    if (isType(token, "PLUS")) {
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isType(token, "MINUS")) {
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isType(token, "ASTERISK")) {
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isType(token, "DIVIDE")) {
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isType(token, "MODULO")) {
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isType(token, "CARET")) {
        return true;
    }

    error_depth = d_init;
    token = init;
    // setErrorMessage("ERROR: Something bad happened in isNumericalOperator on line " + std::to_string(token->line_number()) + ", col " + std::to_string(token->col_number()) + ".", token);
    path.pop_back();
    return false;
}

// <NUMERICAL_OPERAND> <NUMERICAL_OPERATOR> <NUMERICAL_EXPRESSION> |
// <NUMERICAL_OPERAND> |
// <L_PAREN> <NUMERICAL_EXPRESSION> <R_PAREN> <NUMERICAL_OPERATOR> <NUMERICAL_EXPRESSION> |
// <L_PAREN> <NUMERICAL_EXPRESSION> <R_PAREN>
bool RecursiveDescentParser::isNumericalExpression(Token*& token) {
    path.push_back("isNumericalExpression");
    if (token == nullptr) { setErrorMessage("error: unexpected eof", token);; path.pop_back(); return false; }
    error_depth += 1;
    int d_init = error_depth;
    Token* init = token;

    if (isNumericalOperand(token) && isNumericalOperator(token) && isNumericalExpression(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isNumericalOperand(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isType(token, "L_PAREN") && isNumericalExpression(token) && isType(token, "R_PAREN") && isNumericalOperator(token) && isNumericalExpression(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isType(token, "L_PAREN") && isNumericalExpression(token) && isType(token, "R_PAREN")) {
        path.pop_back();
        return true;
    }

    error_depth = d_init;
    token = init;
    // setErrorMessage("ERROR: Something bad happened in isNumericalExpression on line " + std::to_string(token->line_number()) + ", col " + std::to_string(token->col_number()) + ".", token);
    path.pop_back();
    return false;
}

// <USER_DEFINED_FUNCTION> | <IDENTIFIER> | <BOOLEAN_TRUE> | <BOOLEAN_FALSE> | <BOOLEAN_NOT> <BOOLEAN_OPERAND>
bool RecursiveDescentParser::isBooleanOperand(Token*& token) {
    path.push_back("isBooleanExpression");
    if (token == nullptr) { setErrorMessage("error: unexpected eof", token);; path.pop_back(); return false; }
    error_depth += 1;
    int d_init = error_depth;
    Token* init = token;

    if (isUserDefinedFunction(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isIdentifier(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isType(token, "BOOLEAN_TRUE")) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isType(token, "BOOLEAN_FALSE")) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isType(token, "BOOLEAN_NOT") && isBooleanOperand(token)) {
        path.pop_back();
        return true;
    }

    error_depth = d_init;
    token = init;
    // setErrorMessage("ERROR: Something bad happened in isBooleanOperand on line " + std::to_string(token->line_number()) + ", col " + std::to_string(token->col_number()) + ".", token);
    path.pop_back();
    return false;
}

// <BOOLEAN_AND> | <BOOLEAN_OR>
bool RecursiveDescentParser::isBooleanOperator(Token*& token) {
    path.push_back("isBooleanOperator");
    if (token == nullptr) { setErrorMessage("error: unexpected eof", token);; path.pop_back(); return false; }

    if (token->type() == "BOOLEAN_AND" || token->type() == "BOOLEAN_OR") {
        error_depth += 1;
        token = token->next();
        path.pop_back();
        return true;
    }

    setErrorMessage("error: unexpected token '" + token->value() + "'", token);
    path.pop_back();
    return false;
}

// <LT> | <LT_EQUAL> | <GT> | <GT_EQUAL> | <BOOLEAN_EQUAL> | <BOOLEAN_NOT_EQUAL>
bool RecursiveDescentParser::isRelationalExpression(Token*& token) {
    path.push_back("isRelationalExpression");
    if (token == nullptr) { setErrorMessage("error: unexpected eof", token);; path.pop_back(); return false; }
    
    if (token->type() == "LT" || token->type() == "LT_EQUAL" || token->type() == "GT" || token->type() == "GT_EQUAL" ||
        token->type() == "BOOLEAN_EQUAL" || token->type() == "BOOLEAN_NOT_EQUAL") {
        error_depth += 1;
        token = token->next();
        path.pop_back();
        return true;
    }

    setErrorMessage("error: unexpected token '" + token->value() + "'", token);
    path.pop_back();
    return false;
}

// <BOOLEAN_OPERAND> <BOOLEAN_OPERATOR> <BOOLEAN_EXPRESSION> |
// <L_PAREN> <BOOLEAN_EXPRESSION> <R_PAREN> <BOOLEAN_OPERATOR> <BOOLEAN_EXPRESSION> |
// <L_PAREN> <BOOLEAN_EXPRESSION> <R_PAREN> |
// <NUMERICAL_EXPRESSION> <RELATIONAL_EXPRESSION> <NUMERICAL_EXPRESSION> <BOOLEAN_OPERATOR> <BOOLEAN_EXPRESSION>|
// <NUMERICAL_EXPRESSION> <RELATIONAL_EXPRESSION> <NUMERICAL_EXPRESSION> |
// <BOOLEAN_OPERAND> |
bool RecursiveDescentParser::isBooleanExpression(Token*& token) {
    path.push_back("isBooleanExpression");
    if (token == nullptr) { setErrorMessage("error: unexpected eof", token);; path.pop_back(); return false; }
    error_depth += 1;
    int d_init = error_depth;
    Token* init = token;

    if (isBooleanOperand(token) && isBooleanOperator(token) && isBooleanExpression(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isType(token, "L_PAREN") && isBooleanExpression(token) && isType(token, "R_PAREN") && isBooleanOperator(token) && isBooleanExpression(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isType(token, "L_PAREN") && isBooleanExpression(token) && isType(token, "R_PAREN")) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isNumericalExpression(token) && isRelationalExpression(token) && isNumericalExpression(token) && isBooleanOperator(token) && isBooleanExpression(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isNumericalExpression(token) && isRelationalExpression(token) && isNumericalExpression(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isBooleanOperand(token)) {
        path.pop_back();
        return true;
    }

    error_depth = d_init;
    token = init;
    // setErrorMessage("ERROR: Something bad happened in isBooleanExpression on line " + std::to_string(token->line_number()) + ", col " + std::to_string(token->col_number()) + ".", token);
    path.pop_back();
    return false;
}

// <NUMERICAL_EXPRESSION> <COMMA> <EXPRESSION_LIST> | <BOOLEAN_EXPRESSION> <COMMA> <EXPRESSION_LIST>
// <NUMERICAL_EXPRESSION> | <BOOLEAN_EXPRESSION> |
bool RecursiveDescentParser::isExpressionList(Token*& token) {
    path.push_back("isExpressionList");
    if (token == nullptr) { setErrorMessage("error: unexpected eof", token);; path.pop_back(); return false; }
    error_depth += 1;
    int d_init = error_depth;
    Token* init = token;

    if (isNumericalExpression(token) && isType(token, "COMMA") && isExpressionList(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isBooleanExpression(token) && isType(token, "COMMA") && isExpressionList(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isNumericalExpression(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isBooleanExpression(token)) {
        path.pop_back();
        return true;
    }
    
    error_depth = d_init;
    token = init; 
    // setErrorMessage("ERROR: Something bad happened in isExpressionList on line " + std::to_string(token->line_number()) + ", col " + std::to_string(token->col_number()) + ".", token);
    path.pop_back();
    return false;
}

// getchar <L_PAREN> <R_PAREN>
bool RecursiveDescentParser::isGetcharFunction(Token*& token) {
    path.push_back("isGetcharFunction");
    if (token == nullptr) { setErrorMessage("error: unexpected eof", token);; path.pop_back(); return false; }
    error_depth += 1;
    int d_init = error_depth;
    Token* init = token;

    if (isValue(token, "getchar") && isType(token, "L_PAREN") && isType(token, "R_PAREN")) {
        path.pop_back();
        return true;
    }

    error_depth = d_init;
    token = init;
    // setErrorMessage("ERROR: Something bad happened in isGetcharFunction on line " + std::to_string(token->line_number()) + ", col " + std::to_string(token->col_number()) + ".", token);
    path.pop_back();
    return false;
}

// xx <IDENTIFIER> <L_PAREN> <IDENTIFIER_AND_ARRAY_LIST> <R_PAREN> |
// xx <IDENTIFIER> <L_PAREN> <NUMERICAL_EXPRESSION> <R_PAREN> |
// xx <IDENTIFIER> <L_PAREN> <BOOLEAN_EXPRESSION> <R_PAREN>
// -> <IDENTIFIER> <L_PAREN> <EXPRESSION_LIST> <R_PAREN> |
// -> <IDENTIFIER> <L_PAREN> <R_PAREN>
bool RecursiveDescentParser::isUserDefinedFunction(Token*& token) {
    path.push_back("isUserDefinedFunction");
    if (token == nullptr) { setErrorMessage("error: unexpected eof", token);; path.pop_back(); return false; }
    error_depth += 1;
    int d_init = error_depth;
    Token* init = token;

    // if (isIdentifier(token) && isType(token, "L_PAREN") && isIdentifierAndArrayList(token) && isType(token, "R_PAREN")) {
    //     path.pop_back();
    //     return true;
    // }
    // error_depth = d_init;
    // token = init;
    // if (isIdentifier(token) && isType(token, "L_PAREN") && isNumericalExpression(token) && isType(token, "R_PAREN")) {
    //     path.pop_back();
    //     return true;
    // }
    // error_depth = d_init;
    // token = init;
    // if (isIdentifier(token) && isType(token, "L_PAREN") && isBooleanExpression(token) && isType(token, "R_PAREN")) {
    //     path.pop_back();
    //     return true;
    // }

    if (isIdentifier(token) && isType(token, "L_PAREN") && isExpressionList(token) && isType(token, "R_PAREN")) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isIdentifier(token) && isType(token, "L_PAREN") && isType(token, "R_PAREN")) {
        path.pop_back();
        return true;
    }

    error_depth = d_init;
    token = init;
    // setErrorMessage("ERROR: Something bad happened in isUserDefinedFunction on line " + std::to_string(token->line_number()) + ", col " + std::to_string(token->col_number()) + ".", token);
    path.pop_back();
    return false;
}

// <IDENTIFIER_ARRAY> <ASSIGNMENT_OPERATOR> <NUMERICAL_EXPRESSION> |
// <IDENTIFIER_ARRAY> <ASSIGNMENT_OPERATOR> <BOOLEAN_EXPRESSION> |
// xx <IDENTIFIER_ARRAY> <ASSIGNMENT_OPERATOR> <DOUBLE_QUOTED_STRING> |
// <IDENTIFIER> <ASSIGNMENT_OPERATOR> <NUMERICAL_EXPRESSION> |
// <IDENTIFIER> <ASSIGNMENT_OPERATOR> <BOOLEAN_EXPRESSION> |
// <IDENTIFIER> <ASSIGNMENT_OPERATOR> <DOUBLE_QUOTED_STRING> 
bool RecursiveDescentParser::isInitializationExpression(Token*& token) {
    path.push_back("isInitializationExpression");
    if (token == nullptr) { setErrorMessage("error: unexpected eof", token);; path.pop_back(); return false; }
    error_depth += 1;
    int d_init = error_depth;
    Token* init = token;

    if (isIdentifierArray(token) && isType(token, "ASSIGNMENT_OPERATOR") && isNumericalExpression(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isIdentifierArray(token) && isType(token, "ASSIGNMENT_OPERATOR") && isBooleanExpression(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    // if (isIdentifierArray(token) && isType(token, "ASSIGNMENT_OPERATOR") && isDoubleQuotedString(token)) {
    //     path.pop_back();
    //     return true;
    // }
    // error_depth = d_init;
    // token = init;
    if (isIdentifier(token) && isType(token, "ASSIGNMENT_OPERATOR") && isNumericalExpression(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isIdentifier(token) && isType(token, "ASSIGNMENT_OPERATOR") && isBooleanExpression(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isIdentifier(token) && isType(token, "ASSIGNMENT_OPERATOR") && isDoubleQuotedString(token)) {
        path.pop_back();
        return true;
    }

    error_depth = d_init;
    token = init;
    // setErrorMessage("ERROR: Something bad happened in isInitializationExpression on line " + std::to_string(token->line_number()) + ", col " + std::to_string(token->col_number()) + ".", token);
    path.pop_back();
    return false;
}

// <DATATYPE_SPECIFIER> <SIMPLE_IDENTIFIER_AND_ARRAY_LIST> <SEMICOLON>
bool RecursiveDescentParser::isDeclarationStatement(Token*& token) {
    path.push_back("isDeclarationStatement");
    if (token == nullptr) { setErrorMessage("error: unexpected eof", token);; path.pop_back(); return false; }
    error_depth += 1;
    int d_init = error_depth;
    Token* init = token;

    if (isDatatypeSpecifier(token) && isSimpleIdentifierAndArrayList(token) && isType(token, "SEMICOLON")) {
        path.pop_back();
        return true;
    }

    error_depth = d_init;
    token = init;
    // setErrorMessage("ERROR: Something bad happened in isDeclarationStatement on line " + std::to_string(token->line_number()) + ", col " + std::to_string(token->col_number()) + ".", token);
    path.pop_back();
    return false;
}

// <INITIALIZATION_EXPRESSION> <SEMICOLON>
bool RecursiveDescentParser::isAssignmentStatement(Token*& token) {
    path.push_back("isAssignmentStatement");
    if (token == nullptr) { setErrorMessage("error: unexpected eof", token);; path.pop_back(); return false; }
    error_depth += 1;
    int d_init = error_depth;
    Token* init = token;

    if (isInitializationExpression(token) && isType(token, "SEMICOLON")) {
        path.pop_back();
        return true;
    }

    error_depth = d_init;
    token = init;
    // setErrorMessage("ERROR: Something bad happened in isAssignmentStatement on line " + std::to_string(token->line_number()) + ", col " + std::to_string(token->col_number()) + ".", token);
    path.pop_back();
    return false;
}

// if <L_PAREN> <BOOLEAN_EXPRESSION> <R_PAREN> <STATEMENT> else <BLOCK_STATEMENT> |
// if <L_PAREN> <BOOLEAN_EXPRESSION> <R_PAREN> <STATEMENT> else <STATEMENT> |
// if <L_PAREN> <BOOLEAN_EXPRESSION> <R_PAREN> <STATEMENT> |
// if <L_PAREN> <BOOLEAN_EXPRESSION> <R_PAREN> <BLOCK_STATEMENT> else <BLOCK_STATEMENT>
// if <L_PAREN> <BOOLEAN_EXPRESSION> <R_PAREN> <BLOCK_STATEMENT> else <STATEMENT> |
// if <L_PAREN> <BOOLEAN_EXPRESSION> <R_PAREN> <BLOCK_STATEMENT> |
bool RecursiveDescentParser::isSelectionStatement(Token*& token) {
    path.push_back("isSelectionStatement");
    if (token == nullptr) { setErrorMessage("error: unexpected eof", token);; path.pop_back(); return false; }
    error_depth += 1;
    int d_init = error_depth;
    Token* init = token;

    if (isValue(token, "if") && isType(token, "L_PAREN") && isBooleanExpression(token) && isType(token, "R_PAREN") && isStatement(token) && isValue(token, "else") && isBlockStatement(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isValue(token, "if") && isType(token, "L_PAREN") && isBooleanExpression(token) && isType(token, "R_PAREN") && isStatement(token) && isValue(token, "else") && isStatement(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isValue(token, "if") && isType(token, "L_PAREN") && isBooleanExpression(token) && isType(token, "R_PAREN") && isStatement(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isValue(token, "if") && isType(token, "L_PAREN") && isBooleanExpression(token) && isType(token, "R_PAREN") && isBlockStatement(token) && isValue(token, "else") && isBlockStatement(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isValue(token, "if") && isType(token, "L_PAREN") && isBooleanExpression(token) && isType(token, "R_PAREN") && isBlockStatement(token) && isValue(token, "else") && isStatement(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isValue(token, "if") && isType(token, "L_PAREN") && isBooleanExpression(token) && isType(token, "R_PAREN") && isBlockStatement(token)) {
        path.pop_back();
        return true;
    }

    error_depth = d_init;
    token = init;
    // setErrorMessage("ERROR: Something bad happened in isSelectionStatement on line " + std::to_string(token->line_number()) + ", col " + std::to_string(token->col_number()) + ".", token);
    path.pop_back();
    return false;
}

// for <L_PAREN> <INITIALIZATION_EXPRESSION> <SEMICOLON> <BOOLEAN_EXPRESSION> <SEMICOLON> <INITIALIZATION_EXPRESSION> <R_PAREN> <STATEMENT> |
// for <L_PAREN> <INITIALIZATION_EXPRESSION> <SEMICOLON> <BOOLEAN_EXPRESSION> <SEMICOLON> <INITIALIZATION_EXPRESSION> <R_PAREN> <BLOCK_STATEMENT> |
// while <L_PAREN> <BOOLEAN_EXPRESION> <R_PAREN> <STATEMENT> |
// while <L_PAREN> <BOOLEAN_EXPRESION> <R_PAREN> <BLOCK_STATEMENT>
bool RecursiveDescentParser::isIterationStatement(Token*& token) {
    path.push_back("isIterationStatement");
    if (token == nullptr) { setErrorMessage("error: unexpected eof", token);; path.pop_back(); return false; }
    error_depth += 1;
    int d_init = error_depth;
    Token* init = token;

    if (isValue(token, "for") && isType(token, "L_PAREN") && isInitializationExpression(token) && isType(token, "SEMICOLON") && isBooleanExpression(token) && isType(token, "SEMICOLON") && isInitializationExpression(token) && isType(token, "R_PAREN") && isStatement(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isValue(token, "for") && isType(token, "L_PAREN") && isInitializationExpression(token) && isType(token, "SEMICOLON") && isBooleanExpression(token) && isType(token, "SEMICOLON") && isInitializationExpression(token) && isType(token, "R_PAREN") && isBlockStatement(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isValue(token, "while") && isType(token, "L_PAREN") && isBooleanExpression(token) && isType(token, "R_PAREN") && isStatement(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isValue(token, "while") && isType(token, "L_PAREN") && isBooleanExpression(token) && isType(token, "R_PAREN") && isBlockStatement(token)) {
        path.pop_back();
        return true;
    }

    error_depth = d_init;
    token = init;
    // setErrorMessage("ERROR: Something bad happened in isIterationStatement on line " + std::to_string(token->line_number()) + ", col " + std::to_string(token->col_number()) + ".", token);
    path.pop_back();
    return false;
}

// print <L_PAREN> <DOUBLE_QUOTED_STRING> <COMMA> <EXPRESSION_LIST> <R_PAREN> <SEMICOLON>
// print <L_PAREN> <DOUBLE_QUOTED_STRING> <R_PAREN> <SEMICOLON> |
bool RecursiveDescentParser::isPrintStatement(Token*& token) {
    path.push_back("isPrintStatement");
    if (token == nullptr) { setErrorMessage("error: unexpected eof", token);; path.pop_back(); return false; }
    error_depth += 1;
    int d_init = error_depth;
    Token* init = token;

    if (isValue(token, "print") && isType(token, "L_PAREN") && isDoubleQuotedString(token) && isType(token, "COMMA") && isExpressionList(token) && isType(token, "R_PAREN") && isType(token, "SEMICOLON")) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isValue(token, "print") && isType(token, "L_PAREN") && isDoubleQuotedString(token) && isType(token, "R_PAREN") && isType(token, "SEMICOLON")) {
        path.pop_back();
        return true;
    }

    error_depth = d_init;
    token = init;
    // setErrorMessage("ERROR: Something bad happened in isPrintStatement on line " + std::to_string(token->line_number()) + ", col " + std::to_string(token->col_number()) + ".", token);
    path.pop_back();
    return false;
}

// return <NUMERICALEXPRESSION> <SEMICOLON> |
// return <BOOLEAN_EXPRESSION> <SEMICOLON> |
// return <DOUBLE_QUOTED_STRING> <SEMICOLON> |
// return <SEMICOLON>
bool RecursiveDescentParser::isReturnStatement(Token*& token) {
    path.push_back("isReturnStatement");
    if (token == nullptr) { setErrorMessage("error: unexpected eof", token);; path.pop_back(); return false; }
    error_depth += 1;
    int d_init = error_depth;
    Token* init = token;

    if (isValue(token, "return") && isNumericalExpression(token) && isType(token, "SEMICOLON")) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isValue(token, "return") && isBooleanExpression(token) && isType(token, "SEMICOLON")) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isValue(token, "return") && isDoubleQuotedString(token) && isType(token, "SEMICOLON")) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isValue(token, "return") && isType(token, "SEMICOLON")) {
        path.pop_back();
        return true;
    }

    error_depth = d_init;
    token = init;
    // setErrorMessage("ERROR: Something bad happened in isReturnStatement on line " + std::to_string(token->line_number()) + ", col " + std::to_string(token->col_number()) + ".", token);
    path.pop_back();
    return false;
}

// <DECLARATION_STATEMENT> |
// <ASSIGNMENT_STATEMENT> |
// <SELECTION_STATEMENT> |
// <ITERATION_STATEMENT> |
// <PRINT_STATEMENT> |
// <USER_DEFINED_FUNCTION> <SEMICOLON> |
// <RETURN_STATEMENT>
bool RecursiveDescentParser::isStatement(Token*& token) {
    path.push_back("isStatement");
    if (token == nullptr) { setErrorMessage("error: unexpected eof", token);; path.pop_back(); return false; }
    error_depth += 1;
    int d_init = error_depth;
    Token* init = token;

    if (isDeclarationStatement(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isAssignmentStatement(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isSelectionStatement(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isIterationStatement(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isPrintStatement(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isUserDefinedFunction(token) && isType(token, "SEMICOLON")) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isReturnStatement(token)) {
        path.pop_back();
        return true;
    }

    error_depth = d_init;
    token = init;
    // setErrorMessage("ERROR: Something bad happened in isStatement on line " + std::to_string(token->line_number()) + ", col " + std::to_string(token->col_number()) + ".", token);
    path.pop_back();
    return false;
}

// <STATEMENT> <COMPOUND_STATEMENT> | <STATEMENT>
bool RecursiveDescentParser::isCompoundStatement(Token*& token) {
    path.push_back("isCompoundStatement");
    if (token == nullptr) { setErrorMessage("error: unexpected eof", token);; path.pop_back(); return false; }
    error_depth += 1;
    int d_init = error_depth;
    Token* init = token;

    if (isStatement(token) && isCompoundStatement(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isStatement(token)) {
        path.pop_back();
        return true;
    }

    error_depth = d_init;
    token = init;
    // setErrorMessage("ERROR: Something bad happened in isCompoundStatement on line " + std::to_string(token->line_number()) + ", col " + std::to_string(token->col_number()) + ".", token);
    path.pop_back();
    return false;
}

// <L_BRACE> <R_BRACE>
// <L_BRACE> <COMPOUND_STATEMENT> <R_BRACE>
bool RecursiveDescentParser::isBlockStatement(Token*& token) {
    path.push_back("isBlockStatement");
    if (token == nullptr) { setErrorMessage("error: unexpected eof", token);; path.pop_back(); return false; }
    error_depth += 1;
    int d_init = error_depth;
    Token* init = token;

    if (isType(token, "L_BRACE") && isType(token, "R_BRACE")) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isType(token, "L_BRACE") && isCompoundStatement(token) && isType(token, "R_BRACE")) {
        path.pop_back();
        return true;
    }
    
    error_depth = d_init;
    token = init;
    // setErrorMessage("ERROR: Something bad happened in isBlockStatement on line " + std::to_string(token->line_number()) + ", col " + std::to_string(token->col_number()) + ".", token);
    path.pop_back();
    return false;
}

// <DATATYPE_SPECIFIER> <IDENTIFIER> <COMMA> <PARAMETER_LIST> | <DATATYPE_SPECIFIER> <IDENTIFIER>
bool RecursiveDescentParser::isParameterList(Token*& token) {
    path.push_back("isParameterList");
    if (token == nullptr) { setErrorMessage("error: unexpected eof", token);; path.pop_back(); return false; }
    error_depth += 1;
    int d_init = error_depth;
    Token* init = token;

    if (isDatatypeSpecifier(token) && isIdentifier(token) && isType(token, "COMMA") && isParameterList(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isDatatypeSpecifier(token) && isIdentifier(token)) {
        path.pop_back();
        return true;
    }

    error_depth = d_init;
    token = init;
    // setErrorMessage("ERROR: Something bad happened in isParameterList on line " + std::to_string(token->line_number()) + ", col " + std::to_string(token->col_number()) + ".", token);
    path.pop_back();
    return false;
}

// <DATATYPE_SPECIFIER> <IDENTIFIER> <L_PAREN> <PARAMETER_LIST> <R_PAREN> <BLOCK_STATEMENT> |
// <DATATYPE_SPECIFIER> <IDENTIFIER> <L_PAREN> <R_PAREN> <BLOCK_STATEMENT>
bool RecursiveDescentParser::isFunctionDeclaration(Token*& token) {
    path.push_back("isFunctionDeclaration");
    if (token == nullptr) { setErrorMessage("error: unexpected eof", token);; path.pop_back(); return false; }
    error_depth += 1;
    int d_init = error_depth;
    Token* init = token;

    if (isDatatypeSpecifier(token) && isIdentifier(token) && isType(token, "L_PAREN") && isParameterList(token) && isType(token, "R_PAREN") && isBlockStatement(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isDatatypeSpecifier(token) && isIdentifier(token) && isType(token, "L_PAREN") && isType(token, "R_PAREN") && isBlockStatement(token)) {
        path.pop_back();
        return true;
    }

    error_depth = d_init;
    token = init;
    // setErrorMessage("ERROR: Something bad happened in isFunctionDeclaration on line " + std::to_string(token->line_number()) + ", col " + std::to_string(token->col_number()) + ".", token);
    path.pop_back();
    return false;
}

// void <IDENTIFIER> <L_PAREN> <PARAMETER_LIST> <R_PAREN> <BLOCK_STATEMENT> |
// void <IDENTIFIER> <L_PAREN> <R_PAREN> <BLOCK_STATEMENT>
bool RecursiveDescentParser::isNonReturningFunctionDeclaration(Token*& token) {
    path.push_back("isNonReturningFunctionDeclaration");
    if (token == nullptr) { setErrorMessage("error: unexpected eof", token);; path.pop_back(); return false; }
    error_depth += 1;
    int d_init = error_depth;
    Token* init = token;

    if (isValue(token, "void") && isIdentifier(token) && isType(token, "L_PAREN") && isParameterList(token) && isType(token, "R_PAREN") && isBlockStatement(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isValue(token, "void") && isIdentifier(token) && isType(token, "L_PAREN") && isType(token, "R_PAREN") && isBlockStatement(token)) {
        path.pop_back();
        return true;
    }

    error_depth = d_init;
    token = init;
    // setErrorMessage("ERROR: Something bad happened in isNonReturningFunctionDeclaration on line " + std::to_string(token->line_number()) + ", col " + std::to_string(token->col_number()) + ".", token);
    path.pop_back();
    return false;
}

// void main <L_PAREN> <R_PAREN> <BLOCK_STATEMENT>
bool RecursiveDescentParser::isMainDeclaration(Token*& token) {
    path.push_back("isMainDeclaration");
    if (token == nullptr) { setErrorMessage("error: unexpected eof", token);; path.pop_back(); return false; }
    error_depth += 1;
    int d_init = error_depth;
    Token* init = token;

    if (isValue(token, "void") && isValue(token, "main") && isType(token, "L_PAREN") && isType(token, "R_PAREN") && isBlockStatement(token)) {
        path.pop_back();
        return true;
    }

    error_depth = d_init;
    token = init;
    // setErrorMessage("ERROR: Something bad happened in isMainDeclaration on line " + std::to_string(token->line_number()) + ", col " + std::to_string(token->col_number()) + ".", token);
    path.pop_back();
    return false;
}

// <MAIN_DECLARATION> |
// <FUNCTION_DECLARATION> <PROGRAM> |
// <NON_RETURNING_FUNCTION_DECLARATION> <PROGRAM> |
// <DECLARATION_STATEMENT> <PROGRAM>
bool RecursiveDescentParser::isProgram(Token*& token) {
    path.push_back("isProgram");
    if (token == nullptr) { setErrorMessage("error: unexpected eof", token);; path.pop_back(); return false; }
    error_depth += 1;
    int d_init = error_depth;
    Token* init = token;

    if (isMainDeclaration(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isFunctionDeclaration(token) && isProgram(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isNonReturningFunctionDeclaration(token) && isProgram(token)) {
        path.pop_back();
        return true;
    }
    error_depth = d_init;
    token = init;
    if (isDeclarationStatement(token) && isProgram(token)) {
        path.pop_back();
        return true;
    }

    error_depth = d_init;
    token = init;
    // setErrorMessage("ERROR: Something bad happened in isProgram on line " + std::to_string(token->line_number()) + ", col " + std::to_string(token->col_number()) + ".", token);
    path.pop_back();
    return false;
}
