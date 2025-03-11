#pragma once
#include "parser.hpp"

class Generator
{
private:
    const NodeExit m_root;

public:
    Generator(NodeExit root) :
    m_root(std::move(root)) {}

    std::string generate()
    {
        stringstream output;
        output << "global _start\n";
        output << "_start:\n";
        output << "   mov rax, 60\n";
        output << "   mov rdi, " << m_root.expr.int_lit.value().value.value() << "\n";
        output << "   syscall\n";
        return output.str();
    };
};


