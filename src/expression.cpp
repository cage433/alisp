#include "expression.h"
#include <map>

using namespace llvm;

static Module *TheModule;
static std::map<std::string, Value*> NamedValues;
static IRBuilder<> Builder(getGlobalContext());

int Error(const char *Str) { fprintf(stderr, "Error: %s\n", Str);return 0;}
Value *ErrorV(const char *Str) { Error(Str); return 0; }

// AtomicExpression

Function *AtomicExpression::topLevelFunction(){
}

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

Value *FunctionCallExpression::Codegen() {
  // Look up the name in the global module table.
  Function *CalleeF = TheModule->getFunction(name.getIdentifier());
  if (CalleeF == 0)
    return ErrorV("Unknown function referenced");
  
  // If argument mismatch error.
  if (CalleeF->arg_size() != arguments.size())
    return ErrorV("Incorrect # arguments passed");

  std::vector<Value*> ArgsV;
  for (unsigned i = 0, e = arguments.size(); i != e; ++i) {
    ArgsV.push_back(arguments[i]->Codegen());
    if (ArgsV.back() == 0) return 0;
  }
  
  return Builder.CreateCall(CalleeF, ArgsV.begin(), ArgsV.end(), "calltmp");
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

Value *DoubleExpression::Codegen() {
  return ConstantFP::get(getGlobalContext(), APFloat(num));
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

Value *IdentifierExpression::Codegen() {
  // Look this variable up in the function.
  Value *V = NamedValues[identifier];
  return V ? V : ErrorV("Unknown variable name");
}
