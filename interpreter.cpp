#include "interpreter.h"

bool Interpreter::run() {
    Node* node = ast->head();

    // find main
    while (node != nullptr) {
        if (node->symbol() != nullptr && node->symbol()->name() == "main") {
            break;
        }
        else if (node->symbol() != nullptr && (node->symbol()->type() == "procedure" || node->symbol()->type() == "function")) {
            functions.push_back(node);
            // node = node->child();
            process_declaration(node);
            skip_block(node);
        }
        else {
            process_statement(node);
        }

        if (node->next() != nullptr) {
            node = node->next();
        } else {
            node = node->child();
        }
    }

    process_function(node);

    return true;
}

// node should be a function declaration when called
Node* Interpreter::process_function(Node* node) {
    scope_stack.push(node->symbol()->scope());
    node = node->child(); // to begin block node

    // returns true if on returned value
    // returns false if no returned value
    if (process_block(node)) {
        // scope_stack.pop();
        return node;
    } else {
        // scope_stack.pop();
        return nullptr;
    }
}

// node should be a { when called
bool Interpreter::process_block(Node*& node) {
    node = node->child(); // skip begin block

    // all process functions take node when it is on the statement identifier
    // after calling, node will be on the node to be processed next (unless returning)
    while (node != nullptr && node->type() != "END_BLOCK") {
        if (node->type() == "BEGIN_BLOCK") {
            if (process_block(node)) { return true; }
        }
        else if (node->type() == "END_BLOCK") {
            return false;
        }
        else {
            if (process_statement(node)) { return true; }
        }
    }
    node = node->child(); // skip end block
    return false;
}

bool Interpreter::process_statement(Node*& node) {
    if (node->type() == "DECLARATION") {
        process_declaration(node);
    }
    else if (node->type() == "ASSIGNMENT") {
        process_assignment(node);
    }
    else if (node->type() == "IF") {
        if (process_if(node)) return true;
    }
    else if (node->type() == "WHILE") {
        if (process_while(node)) return true;
    }
    else if (node->type() == "FOR1") {
        if (process_for(node)) return true;
    }
    else if (node->type() == "PRINT") {
        process_print(node);
    }
    else if (node->type() == "CALL") {
        node = node->next();
        process_call(node);
        node = node->child();
    }
    else if (node->type() == "RETURN") {
        process_return(node);
        return true;
    }
    else if (node->type() == "BEGIN_BLOCK") {
        if(process_block(node)) { return true; }
    }
    else {
        std::cerr << "error: unexpected statement" << std::endl;
        std::cout << node->type() << node->value() << std::endl;
        exit(6);
    }
    return false;
}

void Interpreter::process_declaration(Node*& node) {
    node->symbol()->defined() = true;
    node = node->child();
}

void Interpreter::process_assignment(Node*& node) {
    node = node->next();
    evaluate(node);
    node = node->child();
}

bool Interpreter::process_if(Node*& node) {
    node = node->next();
    Node* result = evaluate(node);
    if (result->value() == "0" || result->value() == "FALSE" || (result->symbol() != nullptr && result->symbol()->value()[std::max(0, result->index())] == 0)) {
        // else
        node = node->child();
        skip_statement(node); // skip if
        // node = node->child();
        if (node->type() == "ELSE") {
            node = node->child();
            process_statement(node);
            // node = node->child();
        }
    }
    else {
        // if
        node = node->child();
        if (process_statement(node)) { return true; }
        // node = node->child();
        if (node->type() == "ELSE") { // skip else
            node = node->child();
            skip_statement(node);
            // node = node->child();
        }
    }

    return false;
}

bool Interpreter::process_while(Node*& node) {
    node = node->next();
    Node* expression = node;
    Node* result;
    
    while (true) {
        result = evaluate(node);

        if (result->value() == "0" || result->value() == "FALSE" || (result->symbol() != nullptr && result->symbol()->value()[std::max(0, result->index())] == 0)) {
            // skip while loop
            node = node->child();
            skip_statement(node);
            // node = node->child();
            return false;
        }
        else {
            // do while loop
            node = node->child();
            if (process_statement(node)) { return true; }
            node = expression; // go back to start and re-evaluate the boolean expression
        }
    }

    return false;
}

bool Interpreter::process_for(Node*& node) {
    node = node->next(); // for 1 (initialization)
    evaluate(node);

    node = node->child(); // for 2 (bool expression)
    Node* expression = node;
    evaluate(node);

    node = node->child(); // for 3 (increment)
    Node* increment = node;
    while (node->next() != nullptr) {
        node = node->next();
    }
    
    node = node->child(); // to begin block
    Node* start = node;

    Node* result;
    
    while (true) {
        node = expression; // go to expression and evaluate expression
        result = evaluate(node);

        if (result->value() == "0" || result->value() == "FALSE" || (result->symbol() != nullptr && result->symbol()->value()[std::max(0, result->index())] == 0)) {
            // skip for loop
            node = start;
            skip_statement(node);
            // node = node->child();
            return false;
        }
        else {
            // do for loop
            node = start;
            if (process_statement(node)) { return true; }
            node = increment; // go to increment and increment
            evaluate(node);
        }
    }

    return false;
}

// int %i / %d bool %b, char %c / %s (string)
void Interpreter::process_print(Node*& node) {
    node = node->next()->next(); // skip "
    std::string str = node->value();
    node = node->next(); // skip "

    while (node->next() != nullptr) {
        node = node->next();
        if (node->value() == ",") {
            node = node->next();
        }

        Node* var = evaluate(node);

        int var_pos = str.find_first_of('%');

        if (var_pos != std::string::npos) {
            if (var->symbol() == nullptr) {
                if (str[var_pos + 1] == 'i' || str[var_pos + 1] == 'd') {
                    str.replace(var_pos, 2, var->value());
                }
                else if (str[var_pos + 1] == 'b') {
                    str.replace(var_pos, 2, var->value());
                }
                else if (str[var_pos + 1] == 'c') {
                    str.replace(var_pos, 2, var->value());
                }
                else if (str[var_pos + 1] == 's') {
                    str.replace(var_pos, 2, var->value());
                }
            }
            else {
                if (str[var_pos + 1] == 'i' || str[var_pos + 1] == 'd') {
                    str.replace(var_pos, 2, std::to_string(var->symbol()->value()[std::max(0, var->index())]));
                }
                else if (str[var_pos + 1] == 'b') {
                    if (var->symbol()->value()[std::max(0, var->index())]) {
                        str.replace(var_pos, 2, "TRUE");
                    } else {
                        str.replace(var_pos, 2, "FALSE");
                    }
                }
                else if (str[var_pos + 1] == 'c') {
                    str.replace(var_pos, 2, std::string(1, var->symbol()->value()[std::max(0, var->index())]));
                }
                else if (str[var_pos + 1] == 's') {
                    if (var->symbol()->array_size() > 0 && var->index() == -1) {
                        std::string r = "";
                        for (int i = 0; i < var->symbol()->array_size(); i++) {
                            if (var->symbol()->value()[i] == 0) { break; }
                            r += std::string(1, var->symbol()->value()[i]);
                        }
                        str.replace(var_pos, 2, r);
                    }
                    else {
                        str.replace(var_pos, 2, std::string(1, var->symbol()->value()[std::max(0, var->index())]));
                    }
                }
            }
        }
    }

    std::cout << str;
    node = node->child();
}

Node* Interpreter::process_getchar(Node*& node) {
    std::string input;
    std::getline(std::cin, input);
    return new Node("", std::to_string(int(input[0])), nullptr, node->line_number(), node->col_number());
}

// node is function name
// sets variables for the function's parameters
// returns return value of function (see process_function())
Node* Interpreter::process_call(Node*& node) {
    Node* func;
    for (auto f : functions) {
        if (node->symbol()->name() == f->symbol()->name()) {
            func = f;
            break;
        }
    }
    node = node->next()->next(); // to node after (

    Symbol* current = func->symbol()->parameters();

    while (current != nullptr) {
        if (node->value() == ")") {
            std::cerr << "error: not enough arguments\n";
            exit(6);
        }

        Node* result = evaluate(node);
        if (result->symbol() == nullptr) {
            if (result->value() == "TRUE") {
                current->value()[std::max(0, node->index())] = 1;
            } else if (result->value() == "FALSE") {
                current->value()[std::max(0, node->index())] = 0;
            } else {
                current->value()[std::max(0, node->index())] = std::stoi(result->value());
            }
        }
        else {
            current->value()[0] = result->symbol()->value()[std::max(0, result->index())];
        }

        node = node->next(); // comma or )
        if (node->value() == ",") {
            node = node->next(); // next argument or )
        }
        current = current->next();
    }

    if (node->value() != ")") {
        std::cerr << "error: too many arguments : ";
        std::cerr << node->value() << std::endl;
        exit(6);
    }

    return process_function(func);
}

void Interpreter::process_return(Node*& node) {
    node = node->next();
    if (node == nullptr) { return; }
    node = evaluate(node);
}

// node is the start of an expression
// will continue until end of expression, and return result
// end is when next is: ), ], comma, nullptr
Node*Interpreter:: evaluate(Node*& node) {
    std::stack<Node*> stack;

    if (node->symbol() != nullptr && !node->symbol()->defined()) {
        std::cerr << "error: identifier not defined" << std::endl;
        exit(6);
    }
    //functions
    else if (node->symbol() != nullptr && node->symbol()->type() == "function") {
        stack.push(process_call(node)); // ends on )
    }
    // deny procedures
    else if (node->symbol() != nullptr && node->symbol()->type() == "procedure") {
        std::cerr << "error: function is void" << std::endl;
        exit(6);
    }
    // getchar 
    else if (node->type() == "GETCHAR") {
        stack.push(process_getchar(node));
    }
    else {
        stack.push(node);
    }

    while (node->next() != nullptr && node->next()->value() != ")" && node->next()->value() != "]" && node->next()->value() != ",") {
        node = node->next();
        if (node->symbol() != nullptr && !node->symbol()->defined()) {
            std::cerr << "error: identifier not defined" << std::endl;
            exit(6);
        }

        // arrays
        if (node->value() == "[") {
            node = node->next(); // skip [
            Node* idx = evaluate(node); // get idx for array
            node = node->next(); // skip to ]

            if (idx->symbol() == nullptr) {
                if (idx->value() == "TRUE") {
                    stack.top()->index() = idx->value()[1];
                } else if (idx->value() == "FALSE") {
                    stack.top()->index() = idx->value()[0];
                } else {
                    stack.top()->index() = std::stoi(idx->value());
                }
            }
            else {
                stack.top()->index() = idx->symbol()->value()[std::max(0, idx->index())];
            }
        }
        //functions
        else if (node->symbol() != nullptr && node->symbol()->type() == "function") {
            stack.push(process_call(node)); // ends on )
        }
        // deny procedures
        else if (node->symbol() != nullptr && node->symbol()->type() == "procedure") {
            std::cerr << "error: function is void" << std::endl;
            exit(6);
        }
        // getchar
        else if (node->type() == "GETCHAR") {
            stack.push(process_getchar(node));
        }
        else {
            stack.push(node);
        }

        if (stack.size() < 2) {
            continue;
        }

        Node* one = stack.top();
        stack.pop();

        Node* two = stack.top();
        stack.pop();

        if (one->value() == "!" && !is_operator(two)) {
            stack.push(solver(one, two, new Node("", "0", nullptr, two->line_number(), two->col_number())));
            continue;
        }

        if (stack.size() < 3 - 2) {
            stack.push(two);
            stack.push(one);
            continue;
        }

        Node* three = stack.top();
        stack.pop();

        if (one->value() == "'" && three->value() == "'") {
            stack.push(new Node("", std::to_string(two->value()[0]), nullptr, two->line_number(), two->col_number()));
            continue;
        }

        else if (one->value() == "\"" && three->value() == "\"") {
            stack.push(new Node("STRING", two->value(), nullptr, two->line_number(), two->col_number()));
            continue;
        }

        else if (!is_operator(one) || is_operator(two) || is_operator(three)) {
            stack.push(three);
            stack.push(two);
            stack.push(one);
            continue;
        }

        stack.push(solver(one, two, three));
    }

    return stack.top();
}

Node* Interpreter::solver(Node* op, Node* two, Node* three) {
    int val1, val2;

    if (three->symbol() != nullptr) {
        val1 = three->symbol()->value()[std::max(0, three->index())];
    }
    else {
        if (three->value() == "TRUE") {
            val1 = 1;
        } else if (three->value() == "FALSE") {
            val1 = 0;
        } else {
            val1 = std::stoi(three->value());
        }
    }

    if (two->symbol() != nullptr) {
        val2 = two->symbol()->value()[std::max(0, two->index())];
    }
    else {
        if (two->value() == "TRUE") {
            val2 = 1;
        } else if (two->value() == "FALSE") {
            val2 = 0;
        } else if (two->type() == "STRING") {
            val2 = two->value()[0];
        } else {
            val2 = std::stoi(two->value());
        }
    }

    Node* result;

    // arithmatic
    if (op->value() == "+") {
        result = new Node("", std::to_string(val1 + val2), nullptr, op->line_number(), op->col_number());
    }
    else if (op->value() == "-") {
        result = new Node("", std::to_string(val1 - val2), nullptr, op->line_number(), op->col_number());
    }
    else if (op->value() == "*") {
        result = new Node("", std::to_string(val1 * val2), nullptr, op->line_number(), op->col_number());
    }
    else if (op->value() == "/") {
        result = new Node("", std::to_string(int(val1 / val2)), nullptr, op->line_number(), op->col_number());
    }
    else if (op->value() == "^") {
        result = new Node("", std::to_string(pow(val1, val2)), nullptr, op->line_number(), op->col_number());
    }
    else if (op->value() == "%") {
        result = new Node("", std::to_string(val1 % val2), nullptr, op->line_number(), op->col_number());
    }
    else if (op->value() == "=") {
        if (two->type() == "STRING" && three->symbol()->array_size() > 0 && three->index() == -1) {
            for (int i = 0; i < std::min(int(two->value().length()), three->symbol()->array_size()); i++) {
                three->symbol()->value()[i] = two->value()[i];
            }
        }
        else if (two->symbol() != nullptr && std::min(two->symbol()->array_size(), three->symbol()->array_size()) > 0 && two->index() == -1) {
            for (int i = 0; i < std::min(two->symbol()->array_size(), three->symbol()->array_size()); i++) {
                three->symbol()->value()[i] = two->symbol()->value()[i];
            }
        }
        else {
            three->symbol()->value()[std::max(0, three->index())] = val2;
        }
        result = nullptr;
    }

    // boolean
    else if (op->value() == "&&") {
        result = new Node("", std::to_string(val1 && val2), nullptr, op->line_number(), op->col_number());
    }
    else if (op->value() == "||") {
        result = new Node("", std::to_string(val1 || val2), nullptr, op->line_number(), op->col_number());
    }
    else if (op->value() == "!") {
        result = new Node("", std::to_string(val2 == 0), nullptr, op->line_number(), op->col_number());
    }

    // relational
    else if (op->value() == "==") {
        result = new Node("", std::to_string(val1 == val2), nullptr, op->line_number(), op->col_number());
    }
    else if (op->value() == "!=") {
        result = new Node("", std::to_string(val1 != val2), nullptr, op->line_number(), op->col_number());
    }
    else if (op->value() == "<") {
        result = new Node("", std::to_string(val1 < val2), nullptr, op->line_number(), op->col_number());
    }
    else if (op->value() == "<=") {
        result = new Node("", std::to_string(val1 <= val2), nullptr, op->line_number(), op->col_number());
    }
    else if (op->value() == ">") {
        result = new Node("", std::to_string(val1 > val2), nullptr, op->line_number(), op->col_number());
    }
    else if (op->value() == ">=") {
        result = new Node("", std::to_string(val1 >= val2), nullptr, op->line_number(), op->col_number());
    }

    return result;
}

// when called, node should be on begin block
// will end with node after end block
void Interpreter::skip_block(Node*& node) {
    int level = 1;
    while (level > 0) {
        if (node->next() != nullptr) {
            node = node->next();
        } else {
            node = node->child();
        }
        
        if (node->type() == "BEGIN_BLOCK") {
            level += 1;
        }

        else if (node->type() == "END_BLOCK") {
            level -= 1;
        }
    }
    node = node->child();
}

// node will end on start of next statement
void Interpreter::skip_statement(Node*& node) {
    if (node->type() == "DECLARATION" || node->type() == "ASSIGNMENT" || node->type() == "PRINT" || node->type() == "CALL" || node->type() == "RETURN") {
        while (node->next() != nullptr) {
            node = node->next();
        }
        node = node->child();
    }

    else if (node->type() == "IF") {
        while (node->next() != nullptr) {
            node = node->next();
        }
        node = node->child();
        skip_statement(node);
        if (node->type() == "ELSE") {
            node = node->child();
            skip_statement(node);
        }

    }

    else if (node->type() == "WHILE") {
        while (node->next() != nullptr) {
            node = node->next();
        }
        node = node->child();
        skip_statement(node);
    }

    else if (node->type() == "FOR1") {
        for (int i = 0; i < 3; i++) {
            while (node->next() != nullptr) {
                node = node->next();
            }
            node = node->child();
        }
        skip_statement(node);
    }

    else if (node->type() == "BEGIN_BLOCK") {
        skip_block(node);
    }

    else {
        std::cerr << "error: unexpected statement" << std::endl;
        std::cout << node->type() << node->value() << std::endl;
        exit(6);
    }
}

bool Interpreter::is_operator(Node* node) {
    // arithmatic
    if (node->value() == "+") {
        return true;
    }
    else if (node->value() == "-") {
        return true;
    }
    else if (node->value() == "*") {
        return true;
    }
    else if (node->value() == "/") {
        return true;
    }
    else if (node->value() == "^") {
        return true;
    }
    else if (node->value() == "%") {
        return true;
    }
    else if (node->value() == "=") {
        return true;
    }

    // boolean
    else if (node->value() == "&&") {
        return true;
    }
    else if (node->value() == "||") {
        return true;
    }

    // relational
    else if (node->value() == "==") {
        return true;
    }
    else if (node->value() == "!=") {
        return true;
    }
    else if (node->value() == "<") {
        return true;
    }
    else if (node->value() == "<=") {
        return true;
    }
    else if (node->value() == ">") {
        return true;
    }
    else if (node->value() == ">=") {
        return true;
    }

    return false;
}