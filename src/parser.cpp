#include "token.h"
#include "expression.h"
#include "parser.h"
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

void eatToken(deque<shared_ptr<Token> >& tokens, const Token& tok){
    if (tokens.empty())
        throw ParserException("No more tokens, can't eat " + tok.toString());
    if (tok != *tokens.front())
        throw ParserException("Expected " + tok.toString() + ", got " + tokens.front()->toString());
    tokens.pop_front();
}

Expression* consumeParenExpression(deque<shared_ptr<Token> >& tokens){
    eatToken(tokens, LeftParenToken());
    if (tokens.empty())
        throw ParserException("Trailing left paren");
    Token& firstToken = *tokens.front();

    if (firstToken == IdentifierToken("def")){
        tokens.push_front(shared_ptr<Token>(new LeftParenToken()));
        return consumeDefinition(tokens);
    } else {
        vector<shared_ptr<Expression> > arguments;
        while(!tokens.empty() && *(tokens.front()) != RightParenToken()){
            arguments.push_back(shared_ptr<Expression>(consumeExpression(tokens)));
        }
        eatToken(tokens, RightParenToken());
        IdentifierExpression name = IdentifierExpression(((IdentifierToken&)firstToken).getIdentifier());
        return new FunctionCallExpression(name, arguments);
    }
}
Expression* consumeExpression(deque<shared_ptr<Token> >& tokens){
    switch(tokens.front()->tokenType()){
        case LeftParenTokenType: 
            return consumeParenExpression(tokens);
        case IntegerTokenType:{
            IntegerToken& integerToken = (IntegerToken&)tokens.front();
            tokens.pop_front();
            return new IntegerExpression(integerToken.getNum());
        }
        case DoubleTokenType: {
            DoubleToken& doubleToken = (DoubleToken&)tokens.front();
            tokens.pop_front();
            return new DoubleExpression(doubleToken.getNum());
        }
        case IdentifierTokenType: {
            IdentifierToken& identifierToken = (IdentifierToken&)tokens.front();
            tokens.pop_front();
            return new IdentifierExpression(identifierToken.getIdentifier());
        }
        case RightParenTokenType:
            throw ParserException("Unexpected right parenthesis");
        default:
            throw ParserException("Unexpected token " + tokens.front()->toString());
    }
    shared_ptr<Token>& tok = tokens.front();
}


vector<shared_ptr<Expression> > parseExpressions(vector<shared_ptr<Token> >& tokens){
}
