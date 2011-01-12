#include "token.h"
#include <boost/shared_ptr.hpp>

#include <deque>
using namespace std;
using namespace boost;
Expression* consumeExpression(deque<shared_ptr<Token> >& tokens);
