#ifndef GOLOGPP_ATOMS_H_
#define GOLOGPP_ATOMS_H_

#include "gologpp.h"

#include "utilities.h"
#include "Language.h"
#include "expressions.h"

#include <memory>

namespace gologpp {


Scope &global_scope();


class AbstractVariable : public Name, public virtual AbstractLanguageElement {
public:
	AbstractVariable(const string &name, Scope &parent_scope);

	virtual ExpressionTypeTag expression_type_tag() const = 0;

private:
	Scope &parent_scope_;
};


template<class ExpressionT>
class Variable
: public ExpressionT
, public LanguageElement<Variable<ExpressionT>>
, public AbstractVariable
{
protected:
	Variable(const string &name, Scope &parent_scope)
	: ExpressionT(parent_scope)
	, AbstractVariable(name, parent_scope)
	{}

public:
	Variable(Variable<ExpressionT> &&) = default;
	Variable(const Variable<ExpressionT> &) = delete;
	Variable<ExpressionT> &operator = (Variable<ExpressionT> &&) = default;
	Variable<ExpressionT> &operator = (const Variable<ExpressionT> &) = delete;

	friend Scope;

	virtual ExpressionTypeTag expression_type_tag() const override
	{ return ExpressionT::expression_type_tag(); }

	virtual ~Variable() override = default;
	DEFINE_IMPLEMENT
};


template<class ExpressionT>
class Constant
: public ExpressionT
, public LanguageElement<Constant<ExpressionT>>
, public virtual AbstractLanguageElement
{
public:
	Constant(const string &representation)
	: ExpressionT(global_scope())
	, representation_(representation)
	{}

	Constant(Constant<ExpressionT> &&) = default;
	Constant(const Constant<ExpressionT> &) = delete;
	Constant<ExpressionT> &operator = (Constant<ExpressionT> &&) = default;
	Constant<ExpressionT> &operator = (const Constant<ExpressionT> &) = delete;

	virtual ~Constant() override = default;

	const string &representation() const
	{ return representation_; }

	virtual ExpressionTypeTag expression_type_tag() const
	{ return ExpressionT::expression_type_tag(); }

	DEFINE_IMPLEMENT
private:
	const string &representation_;
};


} // namespace gologpp



namespace std {

template<>
struct hash<shared_ptr<gologpp::AbstractVariable>> {
    size_t operator () (const shared_ptr<gologpp::AbstractVariable> &o) const
    { return o->hash(); }
};

} // namespace std

#endif
