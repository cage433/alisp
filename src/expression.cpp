#include "expression.h"

#include "llvm/DerivedTypes.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Support/IRBuilder.h"
using namespace llvm;

// Definition
string DefinitionExpression::toString() const {
    stringstream s;
    s << "Definition\nName " << name.toString() << "\nVariables\n";
    vector<IdentifierExpression> copy = vector<IdentifierExpression>(variableNames);
    for (vector<IdentifierExpression>::iterator exp = copy.begin(); exp != copy.end(); ++exp){
        s << "\t" << (*exp).toString() << "\n";
    }
    s << "Body\n" << body->toString();
    return s.str();
}

bool DefinitionExpression::operator==(const Expression& exp) const {
    try {
        const DefinitionExpression& other = dynamic_cast<const DefinitionExpression&>(exp);
        return name == other.name &&
                variableNames == other.variableNames &&
                body == other.body;
    } catch (std::bad_cast& foo) {
        return false;
    }
}

// Function Call

string FunctionCallExpression::toString() const {
    stringstream s;
    s << "Function Call\n" << "Name " << name.toString() << "\nArguments\n";
    vector<shared_ptr<Expression> > copy(arguments);

    for (vector<shared_ptr<Expression> >::iterator exp = copy.begin(); exp != copy.end(); ++exp){
        s << "\t" << (*exp)->toString() << "\n";
    }
    return s.str();

}
bool FunctionCallExpression::operator==(const Expression& exp) const {
    try {
        const FunctionCallExpression& other = dynamic_cast<const FunctionCallExpression&>(exp);
        return name == other.name &&
            arguments == other.arguments;
    } catch (std::bad_cast& foo) {
        return false;
    }
}

// Integer

string IntegerExpression::toString() const {
    stringstream s;
    s << "Integer Expression " << num;
    return s.str();
};
bool IntegerExpression::operator==(const Expression& exp) const {
    try {
        const IntegerExpression& other = dynamic_cast<const IntegerExpression&>(exp);
        return num == other.num;
    } catch (std::bad_cast& foo) {
        return false;
    }
}

static IRBuilder<> Builder(getGlobalContext());
Value *IntegerExpression::Codegen() {
  return ConstantInt::get(getGlobalContext(), APInt(32, num, true));
}

// Double

string DoubleExpression::toString() const {
    stringstream s;
    s << "Double Expression " << num;
    return s.str();
}
bool DoubleExpression::operator==(const Expression& exp) const {
    try {
        const DoubleExpression& other = dynamic_cast<const DoubleExpression&>(exp);
        return num == other.num;
    } catch (std::bad_cast& foo) {
        return false;
    }
}

// Identifier

string IdentifierExpression::toString() const {
    return "Identifier Expression " + identifier;
}
bool IdentifierExpression::operator==(const Expression& exp) const {
    try {
        const IdentifierExpression& other = dynamic_cast<const IdentifierExpression&>(exp);
        return identifier == other.identifier;
    } catch (std::bad_cast& foo) {
        return false;
    }
}
