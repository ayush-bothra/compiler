#pragma once
#include "parser.hpp"

class Generator
{
private:
    const NodePgm m_root;

public:
    Generator(NodePgm root) :
    m_root(std::move(root)) {}

    [[nodiscard]] std::string genStmt(const NodeStmt& stmt) const 
    {
        struct StmtVisit
        {
            void operator()(const NodeStmtLet& varDec)
            {

            }
            void operator()(const NodeStmtExit& stmtExit)
            {

            }
        };
        StmtVisit visitor;
        std::visit(visitor, stmt.s_var);
    }

    [[nodiscard]] std::string generate() const
    {
        stringstream output;
        output << "global _start\n";
        output << "_start:\n";
        for(const NodeStmt& stmt : m_root.stmts )
        {
            output << genStmt(stmt);
        }
        output << "   mov rax, 60\n";
        output << "   mov rdi, 0\n";
        output << "   syscall\n";
        return output.str();
    };
};