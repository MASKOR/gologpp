#ifndef GOLOGPP_EXPRESSIONS_H_
#define GOLOGPP_EXPRESSIONS_H_

#include "language.h"
#include "utilities.h"
#include "gologpp.h"
#include "types.h"

#include <memory>
#include <algorithm>

namespace gologpp {



class Expression : public virtual AbstractLanguageElement {
protected:
	Expression();
	Expression(const Expression &) = delete;
	Expression(Expression &&) = default;
	Expression &operator = (const Expression &) = delete;

public:
	virtual ~Expression() override = default;

	virtual Scope &parent_scope() override;
	virtual const Scope &parent_scope() const override;
	AbstractLanguageElement *parent();
	const AbstractLanguageElement *parent() const;
	void set_parent(AbstractLanguageElement *parent);

	template<class TypeT>
	TypedExpression<TypeT> *downcast()
	{
		ensure_type<TypeT>();
		return dynamic_cast<TypedExpression<TypeT> *>(this);
	}

protected:
	AbstractLanguageElement *parent_;
};



template<class TypeT>
class TypedExpression : public Expression {
protected:
	TypedExpression()
	{ set_type_by_name(TypeT::static_name()); }

public:
	virtual const TypeT &type() const override
	{
		return dynamic_cast<const TypeT &>(
			AbstractLanguageElement::type()
		);
	}
};

template<>
class TypedExpression<CompoundType> : public Expression {
protected:
	TypedExpression(const string &type_name)
	{ set_type_by_name(type_name); }

public:
	virtual const CompoundType &type() const override
	{
		return dynamic_cast<const CompoundType &>(
			AbstractLanguageElement::type()
		);
	}
};


template<class T>
class unique_ptr<TypedExpression<T>> : public std::unique_ptr<TypedExpression<T>> {
public:
	using std::unique_ptr<TypedExpression<T>>::unique_ptr;

	unique_ptr(Expression *e)
	: std::unique_ptr<TypedExpression<T>>(e->downcast<T>())
	{}

	unique_ptr<TypedExpression<T>> &operator = (Expression *e)
	{ return *this = e->downcast<T>(); }
};



}

#endif // GOLOGPP_EXPRESSIONS_H_
