#pragma once
#include <vector>
#include <optional>
#include <string>
#include "lexer.hpp"


struct NodeExpr
{
    std::optional<Token> int_lit;
};

struct NodeExit
{
    NodeExpr expr;
};

class Parser
{
public:
    explicit Parser(std::vector<Token> tokens):
        m_tokens(std::move(tokens)) {}

    std::optional<NodeExit> parser();
    std::optional<NodeExpr> parse_expr() 
    {
        if(peek().has_value() && peek().value().type == TokenType::int_lit)
        {
            // this way the token .int_lit will be able to store the stuff
            // else just say the int_lit is also an optional and call it a day
            // depends on working of the code really, even idk
            return NodeExpr{.int_lit = consume()};
        }
        else
        {
            return {};
        }
    }

private:
    Tokenize machine;
    const std::vector<Token> m_tokens;
    size_t m_index = 0;

    [[nodiscard]] std::optional<Token> peek(int ahead = 0) const
    {
        // this is a const func because nothing is being processed on, 
        // we just check some stuff and return the values as they are
        if (m_index + ahead >= m_tokens.size())
        {
            return {};
        }
        else
        {
            return m_tokens.at(m_index);
        }
    }

    std::optional<Token> consume() 
        {
            // this function will be used to actually change the pointer, 
            // hence, it is not a const
            return m_tokens.at(m_index++);
        } 
};

std::optional<NodeExit> Parser::parser()
{
    std::optional<NodeExit> exit_node;
    while(peek().has_value())
    {
        if(peek().value().type == TokenType::_return)
        {
            consume();
            // node_expr will store whatever parse_expr will give it
            // if it is a null expr, node_expr will hold null
            if (auto node_expr = Parser::parse_expr())
            {
                // this code will only run if node_expr will have a value
                exit_node = NodeExit{.expr = node_expr.value()};
            }
            else
            {
                std::cerr << "Invalid expression, please rewrite the code.\n";
                exit(EXIT_FAILURE); 
            }
            if(!peek().has_value() || peek().value().type != TokenType::semi_col)
            {
                std::cerr << "Invalid expression, please rewrite the code.\n";
                exit(EXIT_FAILURE);
            }
            else
            {
                consume();
            }
        }
    }
    m_index = 0;
    return exit_node;
};