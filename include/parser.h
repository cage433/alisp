#ifndef ALISP_PARSER
#define ALISP_PARSER

#include "token.h"
#include "expression.h"
#include <boost/shared_ptr.hpp>

#include <deque>
using namespace std;
using namespace boost;
shared_ptr<Expression> consumeExpression(deque<shared_ptr<Token> >& tokens);
vector<shared_ptr<Expression> > parseExpressions(const string& str);

#endif
