#ifndef GOLOGPP_ATOMS_H_
#define GOLOGPP_ATOMS_H_

#include "utilities.h"
#include "Translation.h"
#include <memory>

namespace gologpp {

using namespace std;

class Value;


class Expression : public enable_shared_from_this<Expression>, public InScope {
public:
	Expression(InScope &parent_expr);
	Expression(const shared_ptr<Scope> &parent_scope);
	Expression(Expression &&);

	virtual ~Expression();
};


class Atom : public Expression {
public:
	using Expression::Expression;
};


class Variable : public Atom, public Name {
protected:
	Variable(const string &name, const shared_ptr<Scope> &parent_scope);
	friend Scope;
};


class Value : public Atom {
public:
	using Atom::Atom;
};


} // namespace gologpp



namespace std {

template<>
struct hash<shared_ptr<gologpp::Variable>> {
    size_t operator () (const shared_ptr<gologpp::Variable> &o) const
    { return o->hash(); }
};

} // namespace std

#endif