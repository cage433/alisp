#include <cpptest.h>
#include <stdlib.h>
#include <token.h>
#include "lexer.h"
#include <iostream>
#include <sstream>
using namespace std;

class ExampleTestSuite : public Test::Suite 
{
public:
    ExampleTestSuite(){
        TEST_ADD(ExampleTestSuite::test_token_equality);
        TEST_ADD(ExampleTestSuite::test_token_recognition);
    }
private:
    void test_token_equality();
    void test_token_recognition();
};

void ExampleTestSuite::test_token_equality(){
    TEST_ASSERT(LeftParenToken() == LeftParenToken());
    TEST_ASSERT(RightParenToken() == RightParenToken());
    TEST_ASSERT(LeftParenToken() != RightParenToken());
    TEST_ASSERT(IntegerToken(3) == IntegerToken(3));
    TEST_ASSERT(IntegerToken(3) != IntegerToken(5));
    TEST_ASSERT(IdentifierToken("fred") == IdentifierToken("fred"));
    TEST_ASSERT(IdentifierToken("fred") != IdentifierToken("mike"));
    TEST_ASSERT(DoubleToken(3.5) == DoubleToken(3.5));
    TEST_ASSERT(DoubleToken(3.5) != DoubleToken(5.2));
};
        

void ExampleTestSuite::test_token_recognition(){
    istringstream s;
    s.str("(");
    istream& foo = (istream&)s;
    vector<shared_ptr<Token> > tokens = readTokens(foo);
    cerr << "Num tokens returned " << tokens.size() << "\nthats all \n";
    for (int i = 0; i < tokens.size(); ++i){
        cerr << tokens[i]->toString() << "\n";
    }
    TEST_ASSERT(tokens.size() == 1);
    
    //vector<shared_ptr<Token> > tokens = readTokens(ss);
}

int main(){
    Test::TextOutput output(Test::TextOutput::Verbose);
    ExampleTestSuite ets;
    return ets.run(output) ? EXIT_SUCCESS : EXIT_FAILURE;
}
