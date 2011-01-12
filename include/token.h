#ifndef TOKEN_INCLUDE
#define TOKEN_INCLUDE

#include <string>
#include <sstream>
#include <typeinfo>

using namespace std;

enum TokenType {
    IntegerTokenType,
    DoubleTokenType,
    LeftParenTokenType,
    RightParenTokenType,
    IdentifierTokenType
};

class Token {
    public:
        virtual string toString() const = 0;
        virtual bool operator== (const Token& tok) const = 0;
        bool operator != (const Token& tok) const {
            return ! (*this == tok);
        }
        virtual TokenType tokenType() = 0;
};

class IntegerToken : public Token{
        int num;
    public:
        IntegerToken(int _num) : num(_num){}

        virtual string toString() const {
            stringstream s;
            s << num;
            return s.str();
        };
        bool operator==(const Token& tok) const {
            try {
                const IntegerToken& other = dynamic_cast<const IntegerToken&>(tok);
                return num == other.num;
            } catch (std::bad_cast& foo) {
                return false;
            }
        }
        virtual TokenType tokenType(){return IntegerTokenType;}
        int getNum(){return num;}
};

class DoubleToken : public Token{
        double num;
    public:
        DoubleToken(double _num) :num(_num){}

        virtual string toString() const {
            stringstream s;
            s << num;
            return s.str();
        }
        bool operator==(const Token& tok) const{
            try {
                const DoubleToken& other = dynamic_cast<const DoubleToken&>(tok);
                return num == other.num;
            } catch (std::bad_cast&) {
                return false;
            }
        }
        virtual TokenType tokenType(){return DoubleTokenType;}
        int getNum(){return num;}
};

class LeftParenToken : public Token{
    public:
        virtual string toString() const {
            return string("Left Paren");
        }
        bool operator==(const Token& tok) const {
            try {
                const LeftParenToken& other = dynamic_cast<const LeftParenToken&>(tok);
                return true;
            } catch (std::bad_cast&) {
                return false;
            }
        }
        virtual TokenType tokenType(){return LeftParenTokenType;}
};

class RightParenToken : public Token{
    public:
        virtual string toString() const {
            return string("Right Paren");
        }
        bool operator==(const Token& tok) const {
            try {
                const RightParenToken& other = dynamic_cast<const RightParenToken&>(tok);
                return true;
            } catch (std::bad_cast&) {
                return false;
            }
        }
        virtual TokenType tokenType(){return RightParenTokenType;}
};

class IdentifierToken : public Token{
        string identifier;
    public:
        IdentifierToken(string _identifier) : identifier(_identifier){}

        virtual string toString() const {
            return identifier;
        }

        bool operator==(const Token& tok) const{
            try {
                const IdentifierToken& other = dynamic_cast<const IdentifierToken&>(tok);
                return identifier == other.identifier;
            } catch (std::bad_cast&) {
                return false;
            }
        }
        virtual TokenType tokenType(){return IdentifierTokenType;}
        string getIdentifier(){return identifier;}
};
#endif


