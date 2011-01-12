#ifndef TOKEN_INCLUDE
#define TOKEN_INCLUDE

#include <string>
#include <sstream>
#include <typeinfo>

using namespace std;

class Token {
    public:
        virtual string toString() = 0;
        virtual bool operator== (const Token& tok) = 0;
        bool operator != (const Token& tok) {
            return ! (*this == tok);
        }
};

class IntegerToken : public Token{
        int num;
    public:
        IntegerToken(int _num) : num(_num){}

        virtual string toString() {
            stringstream s;
            s << num;
            return s.str();
        };
        bool operator==(const Token& tok){
            try {
                const IntegerToken& other = dynamic_cast<const IntegerToken&>(tok);
                return num == other.num;
            } catch (std::bad_cast& foo) {
                return false;
            }
        }
};

class DoubleToken : public Token{
        double num;
    public:
        DoubleToken(double _num) :num(_num){}

        virtual string toString() {
            stringstream s;
            s << num;
            return s.str();
        }
        bool operator==(const Token& tok){
            try {
                const DoubleToken& other = dynamic_cast<const DoubleToken&>(tok);
                return num == other.num;
            } catch (std::bad_cast&) {
                return false;
            }
        }
};

class LeftParenToken : public Token{
    public:
        virtual string toString() {
            return string("Left Paren");
        }
        bool operator==(const Token& tok){
            try {
                const LeftParenToken& other = dynamic_cast<const LeftParenToken&>(tok);
                return true;
            } catch (std::bad_cast&) {
                return false;
            }
        }
};

class RightParenToken : public Token{
    public:
        virtual string toString() {
            return string("Right Paren");
        }
        bool operator==(const Token& tok){
            try {
                const RightParenToken& other = dynamic_cast<const RightParenToken&>(tok);
                return true;
            } catch (std::bad_cast&) {
                return false;
            }
        }
    private:
};

class IdentifierToken : public Token{
        string identifier;
    public:
        IdentifierToken(string _identifier) : identifier(_identifier){}

        virtual string toString() {
            return identifier;
        }

        bool operator==(const Token& tok){
            try {
                const IdentifierToken& other = dynamic_cast<const IdentifierToken&>(tok);
                return identifier == other.identifier;
            } catch (std::bad_cast&) {
                return false;
            }
        }
};
#endif


