#ifndef GOLOGPP_ATOMS_H_
#define GOLOGPP_ATOMS_H_

#include "utilities.h"
#include "Language.h"
#include "Scope.h"
#include <memory>
#include <boost/variant.hpp>

namespace gologpp {
namespace generic {

class AnyValue;

class Expression : public std::enable_shared_from_this<Expression>, public virtual AbstractLanguageElement {
protected:
	Expression(Scope &parent_scope);
	Expression(Expression &&);
	Expression(const Expression &) = delete;

	Expression &operator = (const Expression &) = delete;

	virtual ~Expression();

public:
	Scope &parent_scope();
	const Scope &parent_scope() const;

private:
	Scope &parent_scope_;
};


class Atom : public Expression {
public:
	using Expression::Expression;
	Atom(Atom &&);
	Atom(const Atom &) = delete;
};


class Variable : public Atom, public Name, public LanguageElement<Variable> {
protected:
	Variable(const string &name, Scope &parent_scope);
public:
	Variable(Variable &&) = default;
	Variable(const Variable &) = delete;
	Variable &operator = (Variable &&) = default;
	Variable &operator = (const Variable &) = delete;

	friend Scope;

	virtual ~Variable() override = default;
	DEFINE_IMPLEMENT
};


class AnyValue : public Atom, public LanguageElement<AnyValue> {
public:
	typedef boost::variant<string, int, long, double, bool> variant_t;

	template<class T>
	AnyValue(T value)
	: Atom(Scope::global_scope())
	, value_(value)
	{}

	const variant_t &value() const;
	DEFINE_IMPLEMENT

protected:
	variant_t value_;
};



} // namespace generic
} // namespace gologpp



namespace std {

template<>
struct hash<shared_ptr<gologpp::generic::Variable>> {
    size_t operator () (const shared_ptr<gologpp::generic::Variable> &o) const
    { return o->hash(); }
};

} // namespace std

#endif
