#pragma once
#include "parser.hpp"
#include <unordered_map>
/*
note:

class Generator {
private:
    const NodePgm m_root;

public:
    Generator(NodePgm root) : m_root(std::move(root)) {}

    [[nodiscard]] std::string genExpr(const NodeExpr& expr, std::ostream& out) const {
        struct ExprVisitor {
            std::ostream& output;

            ExprVisitor(std::ostream& os) : output(os) {}

            void operator()(const NodeExprIntLit& intlit) const {
                output << "   mov rax, " << intlit.value << "\n";
                output << "   push rax\n";
            }

            void operator()(const NodeExprIdent& exprident) const {
                // Simulated variable loading
                output << "   ; load variable " << exprident.name << "\n";
                output << "   mov rax, [" << exprident.name << "]\n";
                output << "   push rax\n";
            }
        };

        ExprVisitor visitor(out);
        std::visit(visitor, expr.var);
        return ""; // You can return "" or omit return if changing signature
    }

    [[nodiscard]] std::string genStmt(const NodeStmt& stmt, std::ostream& out) const {
        struct StmtVisit {
            const Generator& gen;
            std::ostream& output;

            StmtVisit(const Generator& g, std::ostream& os) : gen(g), output(os) {}

            void operator()(const NodeStmtLet& variableDec) const {
                output << "   ; let " << variableDec.name << " = ...\n";
                gen.genExpr(variableDec.expr, output);
                output << "   pop rax\n";
                output << "   mov [" << variableDec.name << "], rax\n";
            }

            void operator()(const NodeStmtExit& stmtExit) const {
                gen.genExpr(stmtExit.exitCode, output);
                output << "   pop rdi\n";
                output << "   mov rax, 60\n";
                output << "   syscall\n";
            }
        };

        StmtVisit visitor(*this, out);
        std::visit(visitor, stmt.s_var);
        return "";
    }

    [[nodiscard]] std::string generate() const {
        std::stringstream output;
        output << "global _start\n";
        output << "_start:\n";
        for (const NodeStmt& stmt : m_root.stmts) {
            genStmt(stmt, output);
        }
        return output.str();
    }
};

this is a much better way to store the output without actually needing to send
the entire class into a single struct, will implement this later
*/

class Generator
{
private:
    struct Variable
    {
        size_t stack_loc;
    };
    const NodePgm m_root;
    std::stringstream m_output;
    std::unordered_map<std::string, Variable> m_table;
    // we will be using size_t
    // size_t is size-safe for memory
    // it is unsigned and is used to store the largest val possible
    size_t m_stack_size;
    // these two funcs will take care of the stack pointer dynamically
    void push(std::string reg)
    {
        m_output << "   push " << reg << "\n";
        m_stack_size++;
    }
    void pop(std::string reg)
    {
        m_output << "   pop " << reg << "\n";
        m_stack_size--;
    }

public:
    Generator(NodePgm root) :
    m_root(std::move(root)) {}

    void genExpr(const NodeExpr& expr)
    {
        struct ExprVisitor
        {
            // this is a pointer to the class itself, now we can use the 
            // objects of the class (not really good prog practice tho)
            Generator* gen;
            // now, we are going to use the stack (yay)
            // in order to push the values from the registers themselves
            // and when we pop, we will have to decide which reg to put
            // the value into
            void operator()(const NodeExprIntLit& intlit) const
            {
                // the reason for such a big expression is as follows:
                // auto temp = intlit.int_lit; //-> optional<Token>;
                // auto type = temp.value(); // -> Gives the token
                // auto last = type.value.value(); // -> gives the final string
                gen->m_output << "   mov rax, " << intlit.int_lit.value().value.value() << "\n";
                gen->push("rax");
            }
            void operator()(const NodeExprIdent& exprident) const
            {
                // check if the variable is present in the table
                if(gen->m_table.find(exprident.ident.value().value.value()) == gen->m_table.end())
                {
                    std::cerr << "variable " << exprident.ident.value().value.value() << " not defined\n";
                    exit(EXIT_FAILURE);
                }
                // here we are trying to access the stack_loc of this identifier for the offset
                const auto& temp = gen->m_table.at(exprident.ident.value().value.value());
                // stringstream to store all info 
                std::stringstream offset;
                // the actual offset (stack grows down in x86 64 and shrink down)
                // QWORD is the operation size required when we are using offsets
                // since our 
                // Instead of: push QWORD [rsp + ...]
                gen->m_output << "   mov rax, QWORD [rsp + " << (gen->m_stack_size-1 - temp.stack_loc)*8 << "]\n";
                gen->push("rax");
                // pushing the copied value so that I can now access that vars value
                // gen->push(offset.str());
            }
        };
        // this, is an example of a designated intializer; allows for direct initialization, works only if this is an aggregate*
        ExprVisitor visitor{.gen = this}; // we write this because we are inside that class itself
        std::visit(visitor, expr.var);
    }

    void genStmt(const NodeStmt& stmt) 
    {
        // the std::visit is a callable entity that will call the suitable
        // overload of the visitor based on the type of value currently held by 
        // the variant, it is used specifically for variants
        struct StmtVisit
        {
            Generator* gen;
            void operator()(const NodeStmtLet& variableDec) const
            {
                // what we are going to do now is to keep track of which variable we wish to exit with
                // the var will be present in the tokens somewhere, we will simply have to map the var
                // with the value, and then during exit we will need to find that val, push its copy on
                // the top and pop THAT copy in order to have correct var precedence (idk if its optimal tho)

                // using std::unordered_map::contains because we are only worried about the key
                if (gen->m_table.find(variableDec.ident.value().value.value()) != gen->m_table.end())
                {
                    std::cerr<<"variable " << variableDec.ident.value().value.value() << " cannot be redeclared.\n";
                    exit(EXIT_FAILURE);
                }
                else
                {
                    // this again, is the designated initializer syntax
                    gen->m_table.insert({variableDec.ident.value().value.value(), Variable{.stack_loc = gen->m_stack_size}});
                    gen->genExpr(variableDec.expr);
                }
            }
            void operator()(const NodeStmtExit& stmtExit) const 
            {
                // generate the exp before exiting
                gen->genExpr(stmtExit.expr);
                gen->m_output << "   mov rax, 60\n"; // syscall
                gen->pop("rdi");
                gen->m_output << "   syscall\n"; 
            }
        };
        StmtVisit visitor{.gen = this};
        // this is the function that is taking in the params:
        // visitor, the obj of the struct to act as the holder of 
        // knowledge, stmt.s_var: this is the actual knowledge (I believe)
        std::visit(visitor, stmt.s_var);
    }

    [[nodiscard]] std::string generate()
    {
        m_output << "global _start\n";
        m_output << "_start:\n";
        for(const NodeStmt& stmt : m_root.stmts )
        {
            genStmt(stmt);
        }
        return m_output.str();
    };
};