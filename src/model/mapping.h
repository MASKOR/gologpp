#include "utilities.h"
#include "expressions.h"



namespace gologpp{

class ActionMapping : public Identifier {
public:
	ActionMapping(const string &name, const vector<Expression *> &args);

	const vector<unique_ptr<Expression>> &args() const;
private:
	const vector<unique_ptr<Expression>> args_;
};

}
