#ifndef GOLOGPP_ATOMS_H_
#define GOLOGPP_ATOMS_H_

#include "utilities.h"
#include "Language.h"
#include <memory>

namespace gologpp {
namespace generic {

class AnyValue;

class Expression : public std::enable_shared_from_this<Expression>, public virtual AbstractLanguageElement {
protected:
	Expression(Scope &parent_scope);
	Expression(Expression &&);

	virtual ~Expression();

public:
	Scope &parent_scope();
	operator Scope &();

private:
	Scope &parent_scope_;
};


class Atom : public Expression {
public:
	using Expression::Expression;
};


class Variable : public Atom, public Name, public LanguageElement<Variable> {
protected:
	Variable(const string &name, Scope &parent_scope);
	friend Scope;
};


class AnyValue : public Atom, public LanguageElement<AnyValue> {
public:
	AnyValue();
};


template<typename DataT>
class Value : public AnyValue {
public:
	Value(const DataT &data)
	: data_(data)
	{}

	DataT data()
	{ return data_; }

protected:
	DataT data_;
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