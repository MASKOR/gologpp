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

	const string &type_name() const;

protected:
	AbstractLanguageElement *parent_;
};


template<class TypeT>
struct TypedExpression {
};


template<class T>
class unique_ptr<TypedExpression<T>> : public std::unique_ptr<Expression> {
public:
	using std::unique_ptr<Expression>::unique_ptr;

	unique_ptr(Expression *e)
	: std::unique_ptr<Expression>(e)
	{ e->ensure_type<T>(); }

	unique_ptr<TypedExpression<T>> &operator = (Expression *e)
	{
		e->ensure_type<T>();
		return *this = e;
	}
};



}

#endif // GOLOGPP_EXPRESSIONS_H_
