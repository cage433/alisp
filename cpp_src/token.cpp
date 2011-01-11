#include "token.h"
#include <iostream>

int main(){
    Token *h = new IntegerToken(3);
    cout << h->toString() << "\n";
    return 0;

}
