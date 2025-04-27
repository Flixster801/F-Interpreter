#ifndef REMOVE_COMMENTS_H
#define REMOVE_COMMENTS_H

#include <iostream>
#include <sstream>
#include <string>

bool remove_comments(std::stringstream& ss) {
    std::stringstream out;

    bool in_string = false;
    int multi_line_start, string_start, line_number = 1;
    int multi_line_col_start, string_col_start, col_number = 1;

    char c;
    while (ss >> c) {
        col_number += 1;
        if (c == '"') {
            in_string = !in_string;
            string_start = line_number;
            string_col_start = col_number;
        }

        if (c == '/' && !in_string) {
            // Single line comment
            if (ss.peek() == '/') {
                while (!ss.eof() && ss.peek() != '\n') {
                    out << ' ';
                    ss >> c;
                    col_number += 1;
                }
            }
            // Multiline comment
            else if (ss.peek() == '*') {
                multi_line_start = line_number;
                multi_line_col_start = col_number;
                while (c != '*' || ss.peek() != '/') {
                    if (ss.eof()) {
                        std::string line;
                        std::stringstream sstream(ss.str());
                        for (int i = 0; i < multi_line_start; i++) {
                            std::getline(sstream, line);
                        }
                        std::cerr << "[" << multi_line_start << ":" << multi_line_col_start << "] " << "error: unterminated multiline comment" << std::endl;
                        std::cerr << std::string(3, ' ') << multi_line_start << " | " << line << std::endl;
                        std::cerr << std::string(3 + std::to_string(multi_line_start).length(), ' ') << " | " << std::string(multi_line_col_start - 2, ' ') << "^" << std::endl;
                        return false;
                    }
                    else if (c == '\n') {
                        out << '\n';
                        line_number += 1;
                        col_number = 1;
                    } else {
                        out << ' ';
                    }
                    ss >> c;
                    col_number += 1;
                }
                ss >> c;
                col_number += 1;
            }

            else {
                out << '/';
            }
        }

        else {
            if (c == '\n') {
                line_number += 1;
                col_number = 1;
            }
            out << c;
        }
    }

    if (in_string) {
        std::string line;
        std::stringstream sstream(ss.str());
        for (int i = 0; i < string_start; i++) {
            std::getline(sstream, line);
        }
        std::cerr << "[" << string_start << ":" << string_col_start << "] " << "error: unterminated string" << std::endl;
        std::cerr << std::string(3, ' ') << string_start << " | " << line << std::endl;
        std::cerr << std::string(3 + std::to_string(string_start).length(), ' ') << " | " << std::string(string_col_start - 2, ' ') << "^" << std::endl;
        return false;
    }

    ss.str(out.str());

    return true;
}

#endif