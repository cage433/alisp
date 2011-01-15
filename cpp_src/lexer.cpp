#include "token.h"
#include "lexer.h"
#include <boost/shared_ptr.hpp>
#include <vector>
#include <istream>
#include <iostream>

using namespace std;
using namespace boost;

bool isparen(char ch){
    return ch == '(' || ch == ')';
}

Token* consume_identifier(
    istream& s,
    string& buffer)
{
    char ch;
    while (s.good()){
        ch = s.get();
        if (isspace(ch) || isparen(ch)){
            if (isparen(ch)){
                s.unget();
            }
            return new IdentifierToken(buffer);
        } else {
            buffer += ch;
        }
    }
}

Token* consume_double(
    istream& s,
    string& buffer)
{
    char ch;
    while (s.good()){
        ch = s.get();
        if (isdigit(ch)){
            buffer += ch;
        } else if (isspace(ch) || isparen(ch)){
            if (isparen(ch)){
                s.unget();
            }
            return new DoubleToken(atof(buffer.c_str()));
        } else {
            buffer += ch;
            return consume_identifier(s, buffer);
        }
    }
}

Token* consume_integer(
    istream& s,
    string& buffer)
{
    char ch;
    while (s.good()){
        ch = s.get();
        if (isdigit(ch)){
            buffer += ch;
        } else if (ch == '.'){
            buffer += ch;
            return consume_double(s, buffer);
        } else if (isspace(ch) || isparen(ch)){
            if (isparen(ch)){
                s.unget();
            }
            return new IntegerToken(atoi(buffer.c_str()));
        } else {
            buffer += ch;
            return consume_identifier(s, buffer);
        }
    }
}

vector<shared_ptr<Token> > readTokens(istream& s){
    vector<shared_ptr<Token> > tokens;
    cerr << "Num tokens " << tokens.size() << "\nthats all \n";
    string buffer = "";
    char ch;
    while (s >> ch){
        if (ch == '(')
            tokens.push_back(shared_ptr<Token>(new LeftParenToken()));
        else if (ch == ')')
            tokens.push_back(shared_ptr<Token>(new RightParenToken()));
        else if (isdigit(ch)){
            buffer += ch;
            tokens.push_back(shared_ptr<Token>(consume_integer(s, buffer)));
        } else if (isspace(ch))
        {}
        else {
            buffer += ch;
            tokens.push_back(shared_ptr<Token>(consume_identifier(s, buffer)));
        }
    }

    cerr << "Num tokens " << tokens.size() << "\nthats all \n";
    return tokens;
}
