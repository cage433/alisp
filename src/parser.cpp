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

shared_ptr<IdentifierExpression> consumeIdentifierExpression(deque<shared_ptr<Token> >& tokens){
    if (tokens.empty())
        throw ParserException("No more tokens, can't consume identifier expression ");
    if (tokens.front()->tokenType() != IdentifierTokenType)
        throw ParserException("Unexpected token " + tokens.front()->toString());
    IdentifierToken& tok = (IdentifierToken&)(*tokens.front());
    tokens.pop_front();
    return shared_ptr<IdentifierExpression>(new IdentifierExpression(tok.getIdentifier()));
}

shared_ptr<Expression> consumeFunctionCallExpression(deque<shared_ptr<Token> >& tokens){
    eatToken(tokens, LeftParenToken());
    IdentifierToken& functionNameToken = (IdentifierToken&)(*tokens.front());
    IdentifierExpression functionNameExpression = IdentifierExpression(functionNameToken.getIdentifier());
    vector<shared_ptr<Expression> > arguments;

    while(!tokens.empty() && *(tokens.front()) != RightParenToken()){
        arguments.push_back(shared_ptr<Expression>(consumeExpression(tokens)));
    }
    eatToken(tokens, RightParenToken());

    return shared_ptr<Expression>(new FunctionCallExpression(functionNameExpression, arguments));
}

shared_ptr<Expression> consumeDefinitionExpression(deque<shared_ptr<Token> >& tokens){
    eatToken(tokens, LeftParenToken());
    eatToken(tokens, IdentifierToken("def"));
    shared_ptr<IdentifierExpression> functionNameExpression = consumeIdentifierExpression(tokens);
    vector<shared_ptr<IdentifierExpression> > variables;
    eatToken(tokens, LeftParenToken());

    while(!tokens.empty() && *(tokens.front()) != RightParenToken()){
        variables.push_back(consumeIdentifierExpression(tokens));
    }
    eatToken(tokens, RightParenToken());
    shared_ptr<Expression> body = consumeExpression(tokens);
    eatToken(tokens, RightParenToken());

    return shared_ptr<Expression>(new DefinitionExpression(*functionNameExpression, variables, *body));
}

shared_ptr<Expression> consumeParenExpression(deque<shared_ptr<Token> >& tokens){
    eatToken(tokens, LeftParenToken());
    if (tokens.empty())
        throw ParserException("Trailing left paren");
    Token& firstToken = *tokens.front();

    if (firstToken == IdentifierToken("def")){
        tokens.push_front(shared_ptr<Token>(new LeftParenToken()));
        return consumeDefinitionExpression(tokens);
    } else {
        tokens.push_front(shared_ptr<Token>(new LeftParenToken()));
        return consumeFunctionCallExpression(tokens);
    }
}

shared_ptr<Expression> consumeExpression(deque<shared_ptr<Token> >& tokens){
    switch(tokens.front()->tokenType()){
        case LeftParenTokenType: 
            return consumeParenExpression(tokens);
        case IntegerTokenType:{
            IntegerToken& integerToken = (IntegerToken&)tokens.front();
            tokens.pop_front();
            return shared_ptr<Expression>(new IntegerExpression(integerToken.getNum()));
        }
        case DoubleTokenType: {
            DoubleToken& doubleToken = (DoubleToken&)tokens.front();
            tokens.pop_front();
            return shared_ptr<Expression>(new DoubleExpression(doubleToken.getNum()));
        }
        case IdentifierTokenType: {
            return consumeIdentifierExpression(tokens);
//
//            IdentifierToken& identifierToken = (IdentifierToken&)tokens.front();
//            tokens.pop_front();
//            return shared_ptr<Expression>(new IdentifierExpression(identifierToken.getIdentifier()));
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
