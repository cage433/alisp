#ifndef TOKEN_INCLUDE
#define TOKEN_INCLUDE

#include <string>
#include <sstream>

using namespace std;

class Token {
    public:
        virtual string toString() = 0;
};

class IntegerToken : public Token{
    public:
        IntegerToken(int _num){
            num = _num;
        }

        virtual string toString() {
            stringstream s;
            s << num;
            return s.str();
        }
    private:
        int num;
};

class DoubleToken : public Token{
    public:
        DoubleToken(double _num){
            num = _num;
        }

        virtual string toString() {
            stringstream s;
            s << num;
            return s.str();
        }
    private:
        double num;
};

class LeftParenToken : public Token{
    public:
        virtual string toString() {
            return string("Left Paren");
        }
    private:
};

class RightParenToken : public Token{
    public:
        virtual string toString() {
            return string("Right Paren");
        }
    private:
};

class IdentifierToken : public Token{
    public:
        IdentifierToken(string _identifier){
            identifier = _identifier;
        }

        virtual string toString() {
            return identifier;
        }
    private:
        string identifier;
};
#endif

