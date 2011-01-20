#include "expression.h"

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
