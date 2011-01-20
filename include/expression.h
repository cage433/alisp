#ifndef ALISP_EXPRESSION
#define ALISP_EXPRESSION

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <sstream>
#include "llvm/DerivedTypes.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Support/IRBuilder.h"

using namespace std;
using namespace boost;
using namespace llvm;

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
        //virtual Value *Codegen() = 0;
};

class IntegerExpression : public Expression{
        int num;
    public:
        IntegerExpression(int _num) : num(_num){}
        string toString() const;
        bool operator==(const Expression& exp) const;
        virtual Expression *newCopy() {return new IntegerExpression(*this);}
        virtual Value *Codegen();
};

class DoubleExpression : public Expression{
        double num;
    public:
        DoubleExpression(double _num) : num(_num){}
        virtual string toString() const;
        bool operator==(const Expression& exp) const;
        virtual Expression *newCopy() {return new DoubleExpression(*this);}
};


class IdentifierExpression : public Expression{
        string identifier;
    public:
        IdentifierExpression(string _identifier) : identifier(_identifier){}
        virtual string toString() const;
        bool operator==(const Expression& exp) const;
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

        const IdentifierExpression getName() const { return name;}

        virtual string toString() const; 
        bool operator==(const Expression& exp) const;
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
        
        const IdentifierExpression getName() const { return name;}

        virtual string toString() const;
        bool operator==(const Expression& exp) const;
        virtual Expression *newCopy() {return new FunctionCallExpression(*this);}
};

#endif
