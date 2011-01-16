#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

using namespace std;
using namespace boost;

class Expression{
    public:
};

class IntegerExpression : public Expression{
        int num;
    public:
        IntegerExpression(int _num) : num(_num){}
};

class DoubleExpression : public Expression{
        double num;
    public:
        DoubleExpression(double _num) : num(_num){}
};


class IdentifierExpression : public Expression{
        string str;
    public:
        IdentifierExpression(string _str) : str(_str){}
};

class DefinitionExpression : public Expression{
            IdentifierExpression name; 
            vector<shared_ptr<IdentifierExpression> > variableNames;
            Expression body;
    public:
        DefinitionExpression(
            const IdentifierExpression& _name, 
            const vector<shared_ptr<IdentifierExpression> >& _variableNames,
            const Expression& _body
        ) : name(_name), variableNames(_variableNames), body(_body){}
};

class FunctionCallExpression : public Expression{
            IdentifierExpression name; 
            vector<shared_ptr<Expression> > arguments;
    public:
        FunctionCallExpression(
            const IdentifierExpression& _name, 
            const vector<shared_ptr<Expression> >& _arguments
        ) : name(_name), arguments(_arguments){}
};
