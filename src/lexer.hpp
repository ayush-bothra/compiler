#pragma once
#include <vector>
#include <string>
#include <optional>
#include <iostream>
#include "genAsm.hpp"
using namespace std;


// starting with the tokenizing:
enum class TokenType 
{// defines the literals, each is a const with their own identity
// avoiding type and literal errors during reading the file
    _return,
    int_lit,
    semi_col
}; // this class will provide type-safety and scoped naming convention
// very useful indeed

struct Token
{
    TokenType type; // contains an obj of the class containing the literals
    optional<string> value; // represent a value that may or may not be present in the file
}; // this will keep the token-type and their associated value seperately
// this is the lexer/ Tokenizer


class Tokenize
{
    private:
        const string m_src;
        size_t m_index = 0;
        [[nodiscard]] optional<char> peek(int ahead = 1) const
        {
            // this is a const func because nothing is being processed on, 
            // we just check some stuff and return the values as they are
            if (m_index + ahead > m_src.length())
            {
                return {};
            }
            else
            {
                return m_src.at(m_index);
            }
        }

        char consume() 
        {
            // this function will be used to actually change the pointer, 
            // hence, it is not a const
            return m_src.at(m_index++);
        } 

    public:
        Tokenize () {}
        Tokenize (const string& src): m_src(src) {} 

        vector<Token> Tokenizer();
        

        string tokenTypeToString(TokenType type)
        {
            // this is used to print the enums, since they are strongly typed.
            switch(type)
            {
                case TokenType::_return : return "return";
                case TokenType::semi_col : return ";";
                case TokenType::int_lit : return "int_lit";
                default : return "unknown";
            }
        }
};


vector<Token> Tokenize::Tokenizer()
{
    vector<Token> token_buf;
    string buf;

    while (peek().has_value())
    { 
        if (isalpha(peek().value()))
        {
            // takes in only alnum starting with alpha
            buf.push_back(consume()); 
            while (peek().has_value() && isalnum(peek().value()))
            {
                buf.push_back(consume());
            }
            if (buf == "send")
            {
                token_buf.push_back({.type=TokenType::_return, .value=buf});
                buf.clear();
            }
            else
            {
                cerr << "token not identifiable.\n";
                exit(EXIT_FAILURE);
            }
        }
        else if(isdigit(peek().value()))
        {
            // takes only digits
            buf.push_back(consume());
            while (peek().has_value() && isdigit(peek().value()))
            {
                buf.push_back(consume());
            }
            token_buf.push_back({.type=TokenType::int_lit, .value=buf});
            buf.clear();
        }
        else if (peek().has_value() && peek().value() == ';')
        {
            consume();
            token_buf.push_back({.type=TokenType::semi_col, .value=";"});
            buf.clear();
        }
        else if (isspace(peek().value()))
        {
            consume();
            continue;
        }
        else
        {
            cerr << "token not identifiable.\n";
            exit(EXIT_FAILURE);
        }
    }
    m_index = 0;
    return token_buf;  
}