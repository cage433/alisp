#include <cpptest.h>
#include <stdlib.h>
#include <token.h>
#include "lexer.h"
#include "parser.h"
#include "expression.h"
#include <iostream>
#include <sstream>
using namespace std;

class TestSuite : public Test::Suite 
{
public:
    TestSuite(){
        TEST_ADD(TestSuite::test_token_equality);
        TEST_ADD(TestSuite::test_token_recognition);
        TEST_ADD(TestSuite::test_parse_integer);
        TEST_ADD(TestSuite::test_parse_double);
        TEST_ADD(TestSuite::test_parse_identifier);
        TEST_ADD(TestSuite::test_parse_function_call);
    }
private:
    void test_token_equality();
    void test_token_recognition();
    void test_parse_integer();
    void test_parse_double();
    void test_parse_identifier();
    void test_parse_function_call();

};

void TestSuite::test_token_equality(){
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
        

void TestSuite::test_token_recognition(){
    vector<shared_ptr<Token> > tokens = readTokens("(");
    TEST_ASSERT(tokens.size() == 1);
    TEST_ASSERT(*tokens[0] == LeftParenToken());

    tokens = readTokens("0.5 ( fred) 34 ");
    TEST_ASSERT(tokens.size() == 5);
    TEST_ASSERT(*tokens[0] == DoubleToken(0.5));
    TEST_ASSERT(*tokens[1] == LeftParenToken());
    TEST_ASSERT(*tokens[2] == IdentifierToken("fred"));
    TEST_ASSERT(*tokens[3] == RightParenToken());
    TEST_ASSERT(*tokens[4] == IntegerToken(34));
    //TEST_ASSERT(*tokens[5] == IdentifierToken("34a"));
    tokens = readTokens(")34");
    TEST_ASSERT(tokens.size() == 2);
    TEST_ASSERT(*tokens[0] == RightParenToken());
    TEST_ASSERT(*tokens[1] == IntegerToken(34));

    tokens = readTokens("34a");
    TEST_ASSERT(tokens.size() == 1);
    TEST_ASSERT(*tokens[0] == IdentifierToken("34a"));

    tokens = readTokens("12 34a");
    TEST_ASSERT(tokens.size() == 2);
    TEST_ASSERT(*tokens[0] == IntegerToken(12));
    TEST_ASSERT(*tokens[1] == IdentifierToken("34a"));
}

void TestSuite::test_parse_integer(){
    vector<shared_ptr<Expression> > exps = parseExpressions("12");
    TEST_ASSERT(exps.size() == 1);
    TEST_ASSERT(*exps[0] == IntegerExpression(12));
}

void TestSuite::test_parse_double(){
    vector<shared_ptr<Expression> > exps = parseExpressions("12.5");
    TEST_ASSERT(exps.size() == 1);
    TEST_ASSERT(*exps[0] == DoubleExpression(12.5));
}

void TestSuite::test_parse_identifier(){
    vector<shared_ptr<Expression> > exps = parseExpressions("fred");
    TEST_ASSERT(exps.size() == 1);
    TEST_ASSERT(*exps[0] == IdentifierExpression("fred"));
}

void TestSuite::test_parse_function_call(){
    vector<shared_ptr<Expression> > exps = parseExpressions("(fred 3)");
    TEST_ASSERT(exps.size() == 1);
    const FunctionCallExpression& other = dynamic_cast<const FunctionCallExpression&>(*exps[0]);
    IdentifierExpression idExp = other.getName();
    TEST_ASSERT(idExp == IdentifierExpression(string("fred")));
}

int main(){
    Test::TextOutput output(Test::TextOutput::Verbose);
    TestSuite ets;
    return ets.run(output) ? EXIT_SUCCESS : EXIT_FAILURE;
}
