#pragma once
#include <vector>
#include <optional>
#include <variant>
#include <string>
#include "lexer.hpp"
/*A comment for future me:
    change the consume function and add args:
    1. expected token
    2. error if token not present
    this will clean up the code by alot*/

// current syntax style:
// exit(ident/value)
// set ident = value

struct NodeExprIntLit {
    // the variable that will store the actual value
    std::optional<Token> int_lit;
};

struct NodeExprIdent {
    std::optional<Token> ident;
};

struct NodeExpr
{
    std::variant<NodeExprIntLit, NodeExprIdent> var;
    std::optional<Token> o_parens;
};

struct NodeStmtExit {
    // exit(expr)
    NodeExpr expr;
};

struct NodeStmtLet {
    // let ident = [expr]
    std::optional<Token> ident;
    NodeExpr expr;
};

struct NodeStmt {
    std::variant<NodeStmtExit, NodeStmtLet> s_var;
};

struct NodePgm {
    std::vector<NodeStmt> stmts;
};

class Parser
{
public:
    explicit Parser(std::vector<Token> tokens):
        m_tokens(std::move(tokens)) {}

    // std::optional<NodeExit> parser();
    std::optional<NodeStmt> parse_stmt();
    std::optional<NodePgm> parse_pgm();


    std::optional<NodeExpr> parse_expr() 
    {
        if(peek().has_value() && peek().value().type == TokenType::int_lit)
        {
            // this way the token .int_lit will be able to store the stuff
            // else just say the int_lit is also an optional and call it a day
            // depends on working of the code really, even idk

            // the way this return works is that first we know that nodeexpr can have
            // either a variant or an optional, so we first specify that (if u write var it becomes a variant else its an optional)
            // then we need to assign the var with an int_lit to show that var is an int lit, this way it will get correctly stored in the
            // struct NodeExpr 
            return NodeExpr{.var = NodeExprIntLit {.int_lit = consume()} };
        }
        else if(peek().has_value() && peek().value().type == TokenType::ident)
        {
            return NodeExpr{.var = NodeExprIdent {.ident = consume()} };
        }
        else
        {
            return {};
        }
        return std::nullopt;
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
            return m_tokens.at(m_index + ahead);
        }
        return std::nullopt;
    }

    std::optional<Token> consume() 
        {
            // this function will be used to actually change the pointer, 
            // hence, it is not a const
            return m_tokens.at(m_index++);
        } 
    string tokenTypeToString(TokenType type)
        {
            // this is used to print the enums, since they are strongly typed.
            switch(type)
            {
                case TokenType::_return : return "return";
                case TokenType::set : return "set ident";
                case TokenType::asign : return "assign operator";
                case TokenType::semi_col : return "semi_col";
                case TokenType::int_lit : return "int_lit";
                case TokenType::ident : return "ident";
                default : return "space";
            }
        }
};

std::optional<NodeStmt> Parser::parse_stmt()
{
    if(peek().has_value())
    {
        std::cout << tokenTypeToString(peek().value().type) << "\n";
        if(peek().value().type == TokenType::_return && 
        peek(1).has_value() && 
        peek(1).value().type == TokenType::o_parens)
        {
            consume();
            consume();
            // consumed exit(
            NodeStmtExit stmt_exit;
            // node_expr will store whatever parse_expr will give it
            // if it is a null expr, node_expr will hold null
            if (auto node_expr = Parser::parse_expr())
            {
                // if there is a statement inside the exit()
                stmt_exit = {.expr = node_expr.value() };
            }
            else
            {
                std::cerr << "Expect args, please rewrite the code.\n";
                exit(EXIT_FAILURE); 
            }

            if (peek().has_value() && peek().value().type == TokenType::cl_parens)
            {
                consume();
            }
            else
            {
                std::cerr << "Expect ')' after the token, please rewrite the code\n";
                exit(EXIT_FAILURE);
            }

            if(!peek().has_value() || peek().value().type != TokenType::semi_col)
            {
                // after consuming ), a ; shld be there
                std::cerr << "Expect ';'.\n";
                exit(EXIT_FAILURE);
            }
            else
            {
                consume();
            }
            return NodeStmt{.s_var = stmt_exit};
        }
        else if(peek().has_value() && peek().value().type == TokenType::set)
        {
            NodeStmtLet stmt;
            consume();
            // consumed set ; we are checking the value after set
            if(!(peek().has_value() && peek().value().type == TokenType::ident))
            {
                std::cerr<<"expect identifier after keyword 'set'\n";
                m_index = 0;
                return std::nullopt;
            }
            stmt.ident = consume(); // storing the identifier for further use
            if(!(peek().has_value() && peek().value().type == TokenType::asign))
            {
                std::cerr<<"expect assignment operator after identifier\n";
                m_index = 0;
                return std::nullopt;
            }
            consume();
            if(peek().has_value() && peek().value().type == TokenType::ident)
            {
                    //   need to do this
            }
            if(!(peek().has_value() && peek().value().type == TokenType::int_lit))
            {
                std::cerr<<"expect identifier to hold some value\n";
                m_index = 0;
                return std::nullopt;
            }
            stmt.expr.var = NodeExprIntLit{consume().value()};
            if(!peek().has_value() || peek().value().type != TokenType::semi_col)
            {
                std::cerr << "Expect ';'.\n";
                exit(EXIT_FAILURE);
            }
            consume();
            return NodeStmt{.s_var = stmt};
        }
        else
        {
            std::cerr << "Syntax error: either 'send' or '(' or both are missing, please rewrite the code\n";
            m_index = 0;
            return std::nullopt;
        }
    }  
    return std::nullopt;   
}

std::optional<NodePgm> Parser::parse_pgm()
{
    NodePgm pgmstmts;
    while(peek().has_value())
    {
        // below code will return a true value only if the parse
        // happens correctly, else goes into the cerr section
        if(auto stmt = parse_stmt())
        {
            pgmstmts.stmts.push_back(stmt.value());
        }
        else
        {
            std::cerr<<"Not a valid statement\n";
            exit(EXIT_FAILURE);
        }
    }
    return pgmstmts;
}