#include <cpptest.h>
#include <stdlib.h>
#include <token.h>

class ExampleTestSuite : public Test::Suite 
{
public:
    ExampleTestSuite(){
        TEST_ADD(ExampleTestSuite::test_paren_token_equality);
    }
private:
    void test_paren_token_equality();
};

void ExampleTestSuite::test_paren_token_equality(){
    LeftParenToken tok1, tok2;
    TEST_ASSERT(tok1 == tok2);
    RightParenToken tok3;
    TEST_ASSERT(tok1 != tok3);
};
        


int main(){
    Test::TextOutput output(Test::TextOutput::Verbose);
    ExampleTestSuite ets;
    return ets.run(output) ? EXIT_SUCCESS : EXIT_FAILURE;
}
