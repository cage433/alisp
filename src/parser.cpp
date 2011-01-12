#include "token.h"
#include "expression.h"
#include <boost/shared_ptr.hpp>
#include <vector>
#include <istream>
#include <exception>
#include <string>
#include <deque>

using namespace std;
using namespace boost;

class ParserException : public std::exception {
    string error_message;
    public:
        ParserException(const string& message) : error_message(message) {}
        virtual const char* what( ) const throw() { return error_message.c_str(); } 
        ~ParserException() throw(){}
    
};

Expression* consumeExpression(deque<Token>& tokens){
}

vector<shared_ptr<Expression> > parseExpressions(vector<shared_ptr<Token> >& tokens){
}
