#pragma once

#include <vector>
#include <string>
#include <optional>
#include <iostream>
using namespace std;

// starting with the tokenizing:
enum class TokenType 
{// defines the literals, each is a const with their own identity
// avoiding type and literal errors during reading the file
    _return,
    int_lit,
    identifier,
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
        int m_index;
        [[nodiscard]] optional<char> peek(const int ahead = 1) const
        {
            // this is a const func because nothing is being processed on, 
            // we just check some stuff and return the values as they are
            if (m_index + ahead >= m_src.length())
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
                case TokenType::identifier : return "identifier";
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
            buf += consume(); 
            while (peek().has_value() && isalnum(peek().value()))
            {
                buf += consume();
            }
        }
        else if(isdigit(peek().value()))
        {
            // takes only digits
            buf += consume();
            if (peek().has_value() && isalpha(peek().value()))
            {
                cerr << "Invalid token: alphabet cannot be preceeded by digits\n";
                break;
            }
        }
        else if(!buf.empty())
        {
            // Handle the previous token before processing the current character
            if (buf == "getout") 
            {
                token_buf.push_back({TokenType::_return});
            }
            else if (isdigit(buf.at(0)))
            {  
                // Numeric literals
                token_buf.push_back({TokenType::int_lit, buf});
            }
            else 
            {
                // Identifiers
                token_buf.push_back({TokenType::identifier, buf});
            }
            buf.clear();
            buf += consume();
        }
        else if (buf == ";")
        {
            token_buf.push_back({TokenType::semi_col});
            consume();
        }
        else
        {
            cerr << "Unkown token type, types allowed are return, int_lit, semi_col and identifiers\n";
        }
    }

    // If there's any leftover token after processing the whole string, add it
    if (!buf.empty()) 
    {
        if (buf == "getout") 
        {
            token_buf.push_back({TokenType::_return});
        }
        else if (std::isdigit(buf.at(0)))
        {
            token_buf.push_back({TokenType::int_lit, buf});
        }
        else
        {
            token_buf.push_back({TokenType::identifier, buf});
        }
    }

    m_index = 0;
    return token_buf;  
}