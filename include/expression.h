#include <string>
#include <vector>

using namespace std;

class Expression{
    public:
};

class IntegerExpression : public Expression{
    public:
        IntegerExpression(int _num){
            num = _num;
        }
    private:
        int num;
};

class DoubleExpression : public Expression{
    public:
        DoubleExpression(double _num){
            num = _num;
        }
    private:
        double num;
};


class IdentifierExpression : public Expression{
    public:
        IdentifierExpression(){}
        IdentifierExpression(string _str){
            str = _str;
        }
    private:
        string str;
};

class DefinitionExpression : public Expression{
    public:
        DefinitionExpression(
            const IdentifierExpression& _name, 
            const vector<IdentifierExpression>& _variableNames,
            const Expression& _body
        ){
            name = _name;
            variableNames = _variableNames;
            body = _body;
        }
    private:
            IdentifierExpression name; 
            vector<IdentifierExpression> variableNames;
            Expression body;
};

class FunctionCallExpression : public Expression{
    public:
        FunctionCallExpression(
            const IdentifierExpression& _name, 
            const vector<IdentifierExpression>& _variableNames
        ){
            name = _name;
            variableNames = _variableNames;
        }
    private:
            IdentifierExpression name; 
            vector<IdentifierExpression> variableNames;
};
