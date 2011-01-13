#ifndef ALISP_EXPRESSION
#define ALISP_EXPRESSION

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <sstream>

using namespace std;
using namespace boost;

class Expression;

class Expression{
    public:
        virtual string toString() const = 0;
        virtual bool operator== (const Expression& exp) const = 0;
        bool operator != (const Expression& exp) const {
            return ! (*this == exp);
        }
        virtual Expression *newCopy() = 0;
        shared_ptr<Expression> sharedPtr() {return shared_ptr<Expression>(newCopy());}
};

class IntegerExpression : public Expression{
        int num;
    public:
        IntegerExpression(int _num) : num(_num){}
        virtual string toString() const {
            stringstream s;
            s << "Integer Expression " << num;
            return s.str();
        };
        bool operator==(const Expression& exp) const {
            try {
                const IntegerExpression& other = dynamic_cast<const IntegerExpression&>(exp);
                return num == other.num;
            } catch (std::bad_cast& foo) {
                return false;
            }
        }
        virtual Expression *newCopy() {return new IntegerExpression(*this);}
};

class DoubleExpression : public Expression{
        double num;
    public:
        DoubleExpression(double _num) : num(_num){}
        virtual string toString() const {
            stringstream s;
            s << "Double Expression " << num;
            return s.str();
        }
        bool operator==(const Expression& exp) const {
            try {
                const DoubleExpression& other = dynamic_cast<const DoubleExpression&>(exp);
                return num == other.num;
            } catch (std::bad_cast& foo) {
                return false;
            }
        }
        virtual Expression *newCopy() {return new DoubleExpression(*this);}
};


class IdentifierExpression : public Expression{
        string identifier;
    public:
        IdentifierExpression(string _identifier) : identifier(_identifier){}
        virtual string toString() const {
            return "Identifier Expression " + identifier;
        }
        bool operator==(const Expression& exp) const {
            try {
                const IdentifierExpression& other = dynamic_cast<const IdentifierExpression&>(exp);
                return identifier == other.identifier;
            } catch (std::bad_cast& foo) {
                return false;
            }
        }
        virtual Expression *newCopy() {return new IdentifierExpression(*this);}
};

class DefinitionExpression : public Expression{
            IdentifierExpression name; 
            vector<IdentifierExpression> variableNames;
            shared_ptr<Expression> body;
    public:
        DefinitionExpression(
            const IdentifierExpression& _name, 
            const vector<IdentifierExpression>& _variableNames,
            const shared_ptr<Expression>& _body
        ) : name(_name), variableNames(_variableNames), body(_body){}

        virtual string toString() const {
            stringstream s;
            s << "Definition\nName " << name.toString() << "\nVariables\n";
            vector<IdentifierExpression> copy = vector<IdentifierExpression>(variableNames);
            for (vector<IdentifierExpression>::iterator exp = copy.begin(); exp != copy.end(); ++exp){
                s << "\t" << (*exp).toString() << "\n";
            }
            s << "Body\n" << body->toString();
            return s.str();

        }
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
        virtual Expression *newCopy() {return new DefinitionExpression(*this);}
};

class FunctionCallExpression : public Expression{
            IdentifierExpression name; 
            vector<shared_ptr<Expression> > arguments;
    public:
        FunctionCallExpression(
            const IdentifierExpression& _name, 
            const vector<shared_ptr<Expression> >& _arguments
        ) : name(_name), arguments(_arguments){}

        virtual string toString() const {
            stringstream s;
            s << "Function Call\n" << "Name " << name.toString() << "\nArguments\n";
            vector<shared_ptr<Expression> > copy(arguments);

            for (vector<shared_ptr<Expression> >::iterator exp = copy.begin(); exp != copy.end(); ++exp){
                s << "\t" << (*exp)->toString() << "\n";
            }
            return s.str();

        }
        bool operator==(const Expression& exp) const {
            try {
                const FunctionCallExpression& other = dynamic_cast<const FunctionCallExpression&>(exp);
                return name == other.name &&
                    arguments == other.arguments;
            } catch (std::bad_cast& foo) {
                return false;
            }
        }
        virtual Expression *newCopy() {return new FunctionCallExpression(*this);}
};

#endif
