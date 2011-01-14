#include <cpptest.h>
#include <stdlib.h>
#include <token.h>

class ExampleTestSuite : public Test::Suite 
{
public:
    ExampleTestSuite(){
        TEST_ADD(ExampleTestSuite::test_token_equality);
    }
private:
    void test_token_equality();
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
        


int main(){
    Test::TextOutput output(Test::TextOutput::Verbose);
    ExampleTestSuite ets;
    return ets.run(output) ? EXIT_SUCCESS : EXIT_FAILURE;
}
