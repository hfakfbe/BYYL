#pragma once

#include "lexical_analysis.h"

constexpr std::array<std::string_view, 8> keywords {
        "int", "double", "if", "then", "while", "do", "scanf", "printf"
    };
constexpr std::array<std::string_view, 20> operators {
        "=", "==", ">", ">=", "<", "<=", "||", "&&", "!", "!=", "+", "-", "*", "/", ",", "(", ")", "{", "}", ";"
    };

constexpr std::array<std::string_view, 52> alphabet {
        "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z",
        "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"
    };
constexpr std::array<std::string_view, 10> numbers {
        "0", "1", "2", "3", "4", "5", "6", "7", "8", "9"
    };
constexpr std::string_view dot_character = ".";
constexpr std::array<std::string_view, 16> operators_characters {
        "=", ">", "<", "!", "+", "-", "*", "/", ",", "(", ")", "{", "}", ";", "|", "&"
    };
constexpr std::array<std::string_view, 4> empty_characters {
        " ", "\t", "\n", "\r"
    };
constexpr std::array<std::string_view, 15> undefined_characters {
        "%", "$", "#", "@", "~", "^", "`", "?", "[", "]", ":", "\"", "\'", "\\", "_"
    };

constexpr std::array<std::string_view, 33> output_types{
        "INTSYM", "DOUBLESYM", "IFSYM", "THENSYM", "WHILESYM", "DOSYM", "SCANFSYM", "PRINTFSYM",
        "AO", "RO", "RO", "RO", "RO", "RO", "LO", "LO", "LO", "RO", "PLUS", "MINUS", "TIMES", "DIVISION", "COMMA", "BRACE", "BRACE", "BRACE", "BRACE", "SEMICOLON",
        "IDENT", 
        "INT", "DOUBLE",
        "NA", "NA" // note
    };
constexpr std::array<std::string_view, 5> output_errs{
        "NA",
        "Malformed number: More than one decimal point in a floating point number.",
        "Malformed number: Decimal point at the beginning or end of a floating point number.",
        "Malformed number: Leading zeros in an integer.",
        "Unrecognizable characters."
    };

class MakeDFA {
    private:
        std::vector<std::map<char, int>> adj_keywords, adj_operators;
        std::map<int, int> attr_keywords, attr_operators;

    public:
        MakeDFA(int num_symbols = 0);
        int insert_keyword(std::string, int);
        int insert_operator(std::string, int);
        DFA make_dfa();
};
