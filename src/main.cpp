#include <iostream>
#include <fstream>
#include <optional>
#include <sstream>
#include <vector>
#include <string>
#include "lexer.hpp"

using namespace std;



string optionalTostring(const optional<string>& value)
{
    // this checks if the optional region is empty or not
    return value.has_value() ? value.value() : "none"; 
}

string tokensToAsm(const vector<Token>& tokens)
{
    stringstream output;
    output << "global _start\n";

    for(int i = 0; i < tokens.size(); i++)
    { // check for the required conditions (for now)
        const Token& token = tokens.at(i);
        if(token.type == TokenType::_return)
        {
            if(tokens.at(i+1).type == TokenType::int_lit && tokens.at(i+2).type == TokenType::semi_col)
            {   // write the asm required for this:
                output << "_start:\n";
                output << "   mov rax, 60\n";
                output << "   mov rdi, " << tokens.at(i+1).value.value() << "\n";
                output << "   syscall\n";
            }
        }
    }
    return output.str(); // return the asm value in a string format
}

int main(int argc, char* argv[]) 
{
    // argc stores the count of the number of files mentioned in the command line
    // argv stores the actual files inputted on the cmd line
    // both will store the build file algonside the actual file we input
    if (argc < 2) { // if there is no other file apart from the build file
        cerr << "Error, no file found for execution..." << "\n";
        cerr << "method: build_file/<exe input>, requires no flags." << "\n";
        return EXIT_FAILURE;
    }

    // open the file:
    ifstream file(argv[1]); // takes the file path
    if(!file) {
        cerr << "Error, cannot parse the file, or file not given." << "\n";
        return EXIT_FAILURE;
    }

    // process the file.
    string contents;
    {
        stringstream contents_stream; // this is an in-memory string buffer
        fstream input(argv[1], ios::in); // file stream, takes input the first arg of the file in the shell
        contents_stream << input.rdbuf(); // contents of that file are read into the contents_stream using the .rdbuf method
        contents = contents_stream.str(); // assigning the contents of the contents_stream into the contents variable
    }

    Tokenize machine(contents);
    // run the tokenizer:
    vector<Token> result = machine.Tokenizer();

    for (int i = 0; i < result.size(); i++)
    {
        cout << "type: " << machine.tokenTypeToString(result[i].type) 
             << "\t" << "value: " << optionalTostring(result[i].value) << "\n";
    }

    // // creating the asm file content thru this code:
    // string asmFile = tokensToAsm(result);
    // ofstream assembly("test.asm");
    // if(assembly.is_open())
    // {
    //     // write the contents into the file:
    //     assembly << asmFile;
    //     assembly.close();
    //     cout << "File written successfully. can be linked for executable\n";
    // }
    // else
    // {
    //     // return an error:
    //     cout << "The file could not be opened, please try again.\n";
    // }

    file.close();

    // system("nasm -felf64 -o test.o test.asm");
    // system("ld -o test test.o");

    return 0;
}