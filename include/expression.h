#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

using namespace std;
using namespace boost;

class Expression{
    public:
        virtual bool operator== (const Expression& exp) const = 0;
        bool operator != (const Expression& exp) const {
            return ! (*this == exp);
        }
};

class IntegerExpression : public Expression{
        int num;
    public:
        IntegerExpression(int _num) : num(_num){}
        bool operator==(const Expression& exp) const {
            try {
                const IntegerExpression& other = dynamic_cast<const IntegerExpression&>(exp);
                return num == other.num;
            } catch (std::bad_cast& foo) {
                return false;
            }
        }
};

class DoubleExpression : public Expression{
        double num;
    public:
        DoubleExpression(double _num) : num(_num){}
        bool operator==(const Expression& exp) const {
            try {
                const DoubleExpression& other = dynamic_cast<const DoubleExpression&>(exp);
                return num == other.num;
            } catch (std::bad_cast& foo) {
                return false;
            }
        }
};


class IdentifierExpression : public Expression{
        string str;
    public:
        IdentifierExpression(string _str) : str(_str){}
        bool operator==(const Expression& exp) const {
            try {
                const IdentifierExpression& other = dynamic_cast<const IdentifierExpression&>(exp);
                return str == other.str;
            } catch (std::bad_cast& foo) {
                return false;
            }
        }
};

class DefinitionExpression : public Expression{
            IdentifierExpression name; 
            vector<shared_ptr<IdentifierExpression> > variableNames;
            shared_ptr<Expression> body;
    public:
        DefinitionExpression(
            const IdentifierExpression& _name, 
            const vector<shared_ptr<IdentifierExpression> >& _variableNames,
            const shared_ptr<Expression>& _body
        ) : name(_name), variableNames(_variableNames), body(_body){}

        bool operator==(const Expression& exp) const {
            try {
                const DefinitionExpression& other = dynamic_cast<const DefinitionExpression&>(exp);
                return name == other.name &&
                        variableNames == other.variableNames &&
                        body == other.body;
            } catch (std::bad_cast& foo) {
                return false;
            }
        }
};

class FunctionCallExpression : public Expression{
            IdentifierExpression name; 
            vector<shared_ptr<Expression> > arguments;
    public:
        FunctionCallExpression(
            const IdentifierExpression& _name, 
            const vector<shared_ptr<Expression> >& _arguments
        ) : name(_name), arguments(_arguments){}

        bool operator==(const Expression& exp) const {
            try {
                const FunctionCallExpression& other = dynamic_cast<const FunctionCallExpression&>(exp);
                return name == other.name &&
                    arguments == other.arguments;
            } catch (std::bad_cast& foo) {
                return false;
            }
        }
};
