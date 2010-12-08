#include "llvm/DerivedTypes.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/PassManager.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetSelect.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Support/IRBuilder.h"
#include <cstdio>
#include <string>
#include <map>
#include <vector>
#include <list>
using namespace llvm;

// Grammar is
//
// Top Level Expressions := Expression
//                       := Definition
//
// Expression            := (Call) | Number | Identifier | (Operator Call)
//
// Definition            := (def Identifier ( Identifier* ) Expression)
//
// Call                  := Identifier Expression*
//
// Operator Call         := Op Expression*
//
// Op                    := '*' | '+'


//===----------------------------------------------------------------------===//
// Lexer
//===----------------------------------------------------------------------===//

// The lexer returns tokens [0-255] if it is an unknown character, otherwise one
// of these for known things.
enum Token {
  tok_eof = -1,

  // commands
  tok_def = -2, 

  // primary
  tok_identifier = -3, tok_number = -4
};

enum Expression {
    exp_number = 1,
    exp_identifier = 2,
    exp_definition = 3,
    exp_operator_call = 4,
    exp_function_call = 5
};

static std::string IdentifierStr;  // Filled in if tok_identifier
static double NumVal;              // Filled in if tok_number

/// gettok - Return the next token from standard input.
static int gettok() {
  static int LastChar = ' ';

  // Skip any whitespace.
  while (isspace(LastChar))
    LastChar = getchar();

  if (isdigit(LastChar) || LastChar == '.') {   // Number: [0-9.]+
    std::string NumStr;
    do {
      NumStr += LastChar;
      LastChar = getchar();
    } while (isdigit(LastChar) || LastChar == '.');

    NumVal = strtod(NumStr.c_str(), 0);
    return tok_number;
  }

  if (isalpha(LastChar) || LastChar == '+' || LastChar == '*') { // identifier: [a-zA-Z][a-zA-Z0-9]*
    IdentifierStr = LastChar;
    while (isalnum((LastChar = getchar())) || LastChar == '+' || LastChar == '*')
      IdentifierStr += LastChar;

    if (IdentifierStr == "def") return tok_def;
    return tok_identifier;
  }


  if (LastChar == '#') {
    // Comment until end of line.
    do LastChar = getchar();
    while (LastChar != EOF && LastChar != '\n' && LastChar != '\r');
    
    if (LastChar != EOF)
      return gettok();
  }
  
  // Check for end of file.  Don't eat the EOF.
  if (LastChar == EOF)
    return tok_eof;

  // Otherwise, just return the character as its ascii value.
  int ThisChar = LastChar;
  LastChar = getchar();
  return ThisChar;
}

//===----------------------------------------------------------------------===//
// Abstract Syntax Tree (aka Parse Tree)
//===----------------------------------------------------------------------===//
class FunctionAST; 
class PrototypeAST; 

/// ExprAST - Base class for all expression nodes.
class ExprAST {
  int ExprType;
public:
  ExprAST(int exprType) : ExprType(exprType) {}
  virtual ~ExprAST() {}
  virtual Value *Codegen() = 0;
  virtual FunctionAST * TopLevelFunction();
  virtual void HandleTopLevel();
  int GetExprType(){return ExprType;}
};

/// NumberExprAST - Expression class for numeric literals like "1.0".
class NumberExprAST : public ExprAST {
  double Val;
public:
  NumberExprAST(double val) : ExprAST(exp_number), Val(val) {}
  virtual Value *Codegen();
};

/// VariableExprAST - Expression class for referencing a variable, like "a".
class VariableExprAST : public ExprAST {
public:
  std::string Name;
  VariableExprAST(const std::string &name) : ExprAST(exp_identifier), Name(name) {}
  virtual Value *Codegen();
};

/// OperatorCallExprAST - Expression class for operator applications.
class OperatorCallExprAST : public ExprAST {
  std::string Op;
  std::vector<ExprAST*> Args;
public:
  OperatorCallExprAST(std::string op, std::vector<ExprAST*> &args)
    : ExprAST(exp_operator_call), Op(op), Args(args) {}
  Value *BinOpValue(Value* L, Value* R);
  Value *Fold(Value* Init, std::list<Value*> Vals); 
  Value *InitValue();
  virtual Value *Codegen();
};


/// CallExprAST - Expression class for function calls.
class CallExprAST : public ExprAST {
  std::string Callee;
  std::vector<ExprAST*> Args;
public:
  CallExprAST(const std::string &callee, std::vector<ExprAST*> &args)
    : ExprAST(exp_function_call), Callee(callee), Args(args) {}
  virtual Value *Codegen();
};

/// PrototypeAST - This class represents the "prototype" for a function,
/// which captures its name, and its argument names (thus implicitly the number
/// of arguments the function takes).
class PrototypeAST {
  std::string Name;
  std::vector<std::string> Args;
public:
  PrototypeAST(const std::string &name, const std::vector<std::string> &args)
    : Name(name), Args(args) {}
  
  Function *Codegen();
};

static ExecutionEngine *TheExecutionEngine;

/// FunctionAST - This class represents a function definition itself.
class FunctionAST {
  PrototypeAST *Proto;
  ExprAST *Body;
public:
  FunctionAST(PrototypeAST *proto, ExprAST *body)
    : Proto(proto), Body(body) {}

  Function *Codegen();
};

/// DefinitionAST
class DefinitionAST : public ExprAST{
  PrototypeAST *Proto;
  ExprAST *Body;
public:
  DefinitionAST(PrototypeAST *proto, ExprAST *body) : ExprAST(exp_definition){}
  FunctionAST * TopLevelFunction(){
    return new FunctionAST(Proto, Body);
  }
  Function *Codegen(){
    return TopLevelFunction()->Codegen();
  }
  virtual void HandleTopLevel();
};

FunctionAST *ExprAST::TopLevelFunction(){
  PrototypeAST *Proto = new PrototypeAST("", std::vector<std::string>());
  return new FunctionAST(Proto, this);
}

//===----------------------------------------------------------------------===//
// Parser
//===----------------------------------------------------------------------===//

/// CurTok/getNextToken - Provide a simple token buffer.  CurTok is the current
/// token the parser is looking at.  getNextToken reads another token from the
/// lexer and updates CurTok with its results.
static int CurTok;
static int getNextToken() {
  CurTok = gettok();
  //fprintf(stderr, "Curr tok %d\n", CurTok);
  return CurTok;
}
static void eatParen(){
  gettok();
  if (CurTok != ')')
    throw "Expected right paren";
}


/// Error* - These are little helper functions for error handling.
ExprAST *Error(const char *Str) { fprintf(stderr, "Error: %s\n", Str);return 0;}
PrototypeAST *ErrorP(const char *Str) { Error(Str); return 0; }
FunctionAST *ErrorF(const char *Str) { Error(Str); return 0; }

static ExprAST *ParseExpression();

/// identifierexpr ::= identifier
static ExprAST *ParseIdentifierExpr() {
  std::string IdName = IdentifierStr;
  
  return new VariableExprAST(IdName);
}

/// numberexpr ::= number
static ExprAST *ParseNumberExpr() {
  ExprAST *Result = new NumberExprAST(NumVal);
  return Result;
}

/// call expr ::=  Identifier Expression* 
static ExprAST *ParseCallExpr() {
  //printf("In ParseCallExpr ");

  std::string IdName = IdentifierStr;
  getNextToken();  // eat identifier.
  std::vector<ExprAST*> Args;
  if (CurTok != ')') {
    while (1) {
      ExprAST *Arg = ParseExpression();
      if (!Arg) return 0;
      Args.push_back(Arg);
      getNextToken();

      if (CurTok == ')') break;

    }
  }
  //printf("IDName = %s\n", IdName.c_str());
  if (IdName == "+" || IdName == "*")
    return new OperatorCallExprAST(IdName, Args);
  else
    return new CallExprAST(IdName, Args);
}

static DefinitionAST *ParseDefinition();

static ExprAST *ParseInnerParenExpr(){
  fprintf(stderr, "Parsing def\n");
  //printf("In ParseInnerParenExpr IdentifierStr= %s\n", IdentifierStr.c_str());
  if (IdentifierStr == "def"){
    return ParseDefinition();
  } else {
    return ParseCallExpr();
  }
}

/// expression
///   ::= identifierexpr
///   ::= numberexpr
///   ::= parenexpr
static ExprAST *ParseExpression() {
  switch (CurTok) {
  default: return Error("unknown token when expecting an expression");
  case tok_identifier: return ParseIdentifierExpr();
  case tok_number:     return ParseNumberExpr();
  case '(':            getNextToken();ExprAST *exp = ParseInnerParenExpr(); return exp;
  }
}
//
/// prototype
///   ::= id '(' id* ')'
static PrototypeAST *ParsePrototype() {
  if (CurTok != tok_identifier)
    return ErrorP("Expected function name in prototype");

  std::string FnName = IdentifierStr;
  getNextToken();
  
  if (CurTok != '(')
    return ErrorP("Expected '(' in prototype");
  
  std::vector<std::string> ArgNames;
  while (getNextToken() == tok_identifier)
    ArgNames.push_back(IdentifierStr);
  if (CurTok != ')')
    return ErrorP("Expected ')' in prototype");
  
  // success.
  getNextToken();  // eat ')'.
  
  return new PrototypeAST(FnName, ArgNames);
}

/// definition ::= 'def' prototype expression
static DefinitionAST *ParseDefinition() {

  fprintf(stderr, "ParseDefinition being\n");
  getNextToken();  // eat def.
  fprintf(stderr, "ParsePrototype being\n");
  PrototypeAST *Proto = ParsePrototype();

  if (Proto == 0) return 0;
  fprintf(stderr, "ParseExpression being\n");

  if (ExprAST *Body = ParseExpression())
    return new DefinitionAST(Proto, Body);
  fprintf(stderr, "Returning 0\n");
  return 0;
}

/// toplevelexpr ::= expression
static FunctionAST *ParseTopLevelExpr() {
  //printf("In ParseTopLevelExpr\n");
  if (ExprAST *E = ParseExpression()) {
    // Make an anonymous proto.
    PrototypeAST *Proto = new PrototypeAST("", std::vector<std::string>());
    return new FunctionAST(Proto, E);
  }
  return 0;
}
//
//===----------------------------------------------------------------------===//
// Code Generation
//===----------------------------------------------------------------------===//

static Module *TheModule;
static IRBuilder<> Builder(getGlobalContext());
static std::map<std::string, Value*> NamedValues;
static FunctionPassManager *TheFPM;

Value *ErrorV(const char *Str) { Error(Str); return 0; }

Value *NumberExprAST::Codegen() {
  return ConstantFP::get(getGlobalContext(), APFloat(Val));
}

Value *VariableExprAST::Codegen() {
  // Look this variable up in the function.
  Value *V = NamedValues[Name];
  return V ? V : ErrorV("Unknown variable name");
}
Value *OperatorCallExprAST::BinOpValue(Value* L, Value* R){
  if (Op == "+"){
    return Builder.CreateFAdd(L, R, "addtmp");
  } else if (Op == "*") {
    return Builder.CreateFMul(L, R, "multmp");
  } else {
    return ErrorV("invalid binary operator");
  }
}
Value *OperatorCallExprAST::Fold(Value* Init, std::list<Value*> Vals) {
  if (Vals.empty())
    return Init;
  else{
    Value* RHS = Vals.front();
    Vals.pop_front();
    return Fold(BinOpValue(Init, RHS), Vals);
  }
}
Value *OperatorCallExprAST::InitValue(){
  if (Op == "+"){
    return ConstantFP::get(getGlobalContext(), APFloat(0.0));
  } else if (Op == "*") {
    return ConstantFP::get(getGlobalContext(), APFloat(1.0));
  } else {
    return ErrorV("invalid binary operator");
  }
}

Value *OperatorCallExprAST::Codegen() {
  std::list<Value*> Vals;
  for(std::vector<ExprAST*>::iterator it = Args.begin(); it != Args.end(); ++it){
    Vals.push_front((*it)->Codegen());
  }
  return Fold(InitValue(), Vals);
}

Value *CallExprAST::Codegen() {
  // Look up the name in the global module table.
  Function *CalleeF = TheModule->getFunction(Callee);
  if (CalleeF == 0)
    return ErrorV("Unknown function referenced");
  
  // If argument mismatch error.
  if (CalleeF->arg_size() != Args.size())
    return ErrorV("Incorrect # arguments passed");

  std::vector<Value*> ArgsV;
  for (unsigned i = 0, e = Args.size(); i != e; ++i) {
    ArgsV.push_back(Args[i]->Codegen());
    if (ArgsV.back() == 0) return 0;
  }
  
  return Builder.CreateCall(CalleeF, ArgsV.begin(), ArgsV.end(), "calltmp");
}

Function *PrototypeAST::Codegen() {
  // Make the function type:  double(double,double) etc.
  std::vector<const Type*> Doubles(Args.size(),
                                   Type::getDoubleTy(getGlobalContext()));
  FunctionType *FT = FunctionType::get(Type::getDoubleTy(getGlobalContext()),
                                       Doubles, false);
  
  Function *F = Function::Create(FT, Function::ExternalLinkage, Name, TheModule);
  
  // If F conflicted, there was already something named 'Name'.  If it has a
  // body, don't allow redefinition or reextern.
  if (F->getName() != Name) {
    // Delete the one we just made and get the existing one.
    F->eraseFromParent();
    F = TheModule->getFunction(Name);
    
    // If F already has a body, reject this.
    if (!F->empty()) {
      ErrorF("redefinition of function");
      return 0;
    }
    
    // If F took a different number of args, reject.
    if (F->arg_size() != Args.size()) {
      ErrorF("redefinition of function with different # args");
      return 0;
    }
  }
  
  // Set names for all arguments.
  unsigned Idx = 0;
  for (Function::arg_iterator AI = F->arg_begin(); Idx != Args.size();
       ++AI, ++Idx) {
    AI->setName(Args[Idx]);
    
    // Add arguments to variable symbol table.
    NamedValues[Args[Idx]] = AI;
  }
  
  return F;
}

Function *FunctionAST::Codegen() {
  NamedValues.clear();
  
  Function *TheFunction = Proto->Codegen();
  if (TheFunction == 0)
    return 0;
  
  // Create a new basic block to start insertion into.
  BasicBlock *BB = BasicBlock::Create(getGlobalContext(), "entry", TheFunction);
  Builder.SetInsertPoint(BB);
  
  if (Value *RetVal = Body->Codegen()) {
    // Finish off the function.
    Builder.CreateRet(RetVal);

    // Validate the generated code, checking for consistency.
    verifyFunction(*TheFunction);

    // Optimize the function.
    TheFPM->run(*TheFunction);
    
    return TheFunction;
  }
  
  // Error reading body, remove function.
  TheFunction->eraseFromParent();
  return 0;
}


void ExprAST::HandleTopLevel() {
   //Evaluate a top-level expression into an anonymous function.
  if (FunctionAST *F = TopLevelFunction()) {
    if (Function *LF = F->Codegen()) {
       //JIT the function, returning a function pointer.
      void *FPtr = TheExecutionEngine->getPointerToFunction(LF);
      
       //Cast it to the right type (takes no arguments, returns a double) so we
       //can call it as a native function.
      double (*FP)() = (double (*)())(intptr_t)FPtr;
      fprintf(stderr, "Evaluated to %f\n", FP());
    }
  } 
}

void DefinitionAST::HandleTopLevel(){
  if (FunctionAST *F = TopLevelFunction()) {
    if (Function *LF = F->Codegen()) {
      LF->dump();
    }
  } 
}


//===----------------------------------------------------------------------===//
// Top-Level parsing and JIT Driver
//===----------------------------------------------------------------------===//

static void MainLoop() {
  while (1) {
    ExprAST *E = ParseExpression();
    E->HandleTopLevel();
    fprintf(stderr, "ready> ");
    getNextToken();
  }
}

//
//===----------------------------------------------------------------------===//
// Main driver code.
//===----------------------------------------------------------------------===//

int main() {
  InitializeNativeTarget();
  LLVMContext &Context = getGlobalContext();

  // Prime the first token.
  fprintf(stderr, "ready> ");
  getNextToken();

  // Make the module, which holds all the code.
  TheModule = new Module("my cool jit", Context);

  // Create the JIT.  This takes ownership of the module.
  std::string ErrStr;
  TheExecutionEngine = EngineBuilder(TheModule).setErrorStr(&ErrStr).create();
  if (!TheExecutionEngine) {
    fprintf(stderr, "Could not create ExecutionEngine: %s\n", ErrStr.c_str());
    exit(1);
  }

  FunctionPassManager OurFPM(TheModule);

  // Set up the optimizer pipeline.  Start with registering info about how the
  // target lays out data structures.
  OurFPM.add(new TargetData(*TheExecutionEngine->getTargetData()));
  // Do simple "peephole" optimizations and bit-twiddling optzns.
  OurFPM.add(createInstructionCombiningPass());
  // Reassociate expressions.
  OurFPM.add(createReassociatePass());
  // Eliminate Common SubExpressions.
  OurFPM.add(createGVNPass());
  // Simplify the control flow graph (deleting unreachable blocks, etc).
  OurFPM.add(createCFGSimplificationPass());

  OurFPM.doInitialization();

  // Set the global so the code gen can use this.
  TheFPM = &OurFPM;

  // Run the main "interpreter loop" now.
  MainLoop();

  TheFPM = 0;

  // Print out all of the generated code.
  TheModule->dump();

  return 0;
}
