#ifndef GOLOGPP_ATOMS_H_
#define GOLOGPP_ATOMS_H_

#include "gologpp.h"

#include "utilities.h"
#include "expressions.h"
#include "language.h"
#include "scope.h"

#include <memory>
#include <type_traits>

#include <boost/variant.hpp>

namespace gologpp {


Scope &global_scope();



class AbstractVariable : public Identifier, public virtual AbstractLanguageElement {
public:
	AbstractVariable(const string &name);

	virtual ExpressionTypeTag expression_type_tag() const = 0;

	template<class ExpressionT>
	operator Variable<ExpressionT> *() {
		return dynamic_cast<Variable<ExpressionT> *>(this);
	}

	template<class ExpressionT>
	operator ExpressionT *() {
		return static_cast<ExpressionT *>(dynamic_cast<Variable<ExpressionT> *>(this));
	}
};



template<class ExpressionT>
class Variable
: public ExpressionT
, public LanguageElement<Variable<ExpressionT>>
, public AbstractVariable
, public NoScopeOwner
, public std::enable_shared_from_this<Variable<ExpressionT>>
{
protected:
	Variable(const string &name)
	: AbstractVariable(name)
	{}

public:
	Variable(Variable<ExpressionT> &&) = default;
	Variable(const Variable<ExpressionT> &) = delete;
	Variable<ExpressionT> &operator = (Variable<ExpressionT> &&) = default;
	Variable<ExpressionT> &operator = (const Variable<ExpressionT> &) = delete;

	friend Scope;

	virtual ExpressionTypeTag expression_type_tag() const override
	{ return ExpressionT::expression_type_tag(); }

	Reference<Variable<ExpressionT>> *ref()
	{
		return new Reference<Variable<ExpressionT>>(
			this->shared_from_this()
		);
	}


	virtual ~Variable() override = default;
	DEFINE_IMPLEMENT

	virtual string to_string(const string &) const override
	{ return gologpp::to_string(expression_type_tag()) + name(); }
};



class AbstractConstant : public virtual AbstractLanguageElement {
public:
	AbstractConstant(const string &representation);

	virtual ExpressionTypeTag expression_type_tag() const = 0;

	const string &representation() const;
	virtual AbstractConstant *copy() const = 0;

	template<class ExpressionT>
	operator Constant<ExpressionT> *() {
		return dynamic_cast<Constant<ExpressionT> *>(this);
	}

	template<class ExpressionT>
	operator ExpressionT *() {
		return static_cast<ExpressionT *>(dynamic_cast<Constant<ExpressionT> *>(this));
	}

protected:
	const string representation_;
};



template<class ExpressionT>
class Constant
: public ExpressionT
, public AbstractConstant
, public NoScopeOwner
, public LanguageElement<Constant<ExpressionT>>
{
public:
	template<class T>
	Constant(T n)
	: AbstractConstant(std::to_string(n))
	{}

	Constant(const string &repr)
	: AbstractConstant(repr)
	{}


	template<class... Ts>
	Constant(const boost::variant<Ts...> v)
	: AbstractConstant(
		boost::apply_visitor(
			[] (auto val) { return std::to_string(val); },
			v
		)
	)
	{}


	Constant(Constant<ExpressionT> &&) = default;
	Constant(const Constant<ExpressionT> &) = delete;
	Constant<ExpressionT> &operator = (Constant<ExpressionT> &&) = default;
	Constant<ExpressionT> &operator = (const Constant<ExpressionT> &) = delete;

	virtual ~Constant() override = default;

	virtual ExpressionTypeTag expression_type_tag() const override
	{ return ExpressionT::expression_type_tag(); }

	DEFINE_IMPLEMENT

	virtual string to_string(const string &) const override
	{ return representation(); }

	virtual Constant<ExpressionT> *copy() const override
	{ return new Constant<ExpressionT>(representation()); }
};


template<>
template<>
Constant<BooleanExpression>::Constant(bool);

template<>
template<>
Constant<NumericExpression>::Constant(const string &);


} // namespace gologpp



namespace std {

template<>
struct hash<shared_ptr<gologpp::AbstractVariable>> {
	size_t operator () (const shared_ptr<gologpp::AbstractVariable> &o) const
	{ return o->hash(); }
};

} // namespace std

#endif
