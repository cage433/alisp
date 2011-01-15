#include "token.h"
#include <boost/shared_ptr.hpp>
#include <vector>
#include <istream>
using namespace boost;

vector<shared_ptr<Token> > readTokens(istream& s);
vector<shared_ptr<Token> > readTokens(const string& str);
