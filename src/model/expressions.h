#ifndef GOLOGPP_EXPRESSIONS_H_
#define GOLOGPP_EXPRESSIONS_H_

#include "language.h"
#include "utilities.h"
#include "gologpp.h"

#include <memory>
#include <algorithm>

namespace gologpp {


enum ExpressionTypeTag {
	BOOLEAN,
	NUMERIC,
	SYMBOLIC,
	STRING,
	VOID,
};



string to_string(ExpressionTypeTag t);



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

	virtual ExpressionTypeTag dynamic_type_tag() const = 0;

protected:
	AbstractLanguageElement *parent_;
};


class BooleanExpression : public Expression {
protected:
	using Expression::Expression;

public:
	typedef BooleanExpression expression_t;
	virtual ExpressionTypeTag dynamic_type_tag() const override;
	static ExpressionTypeTag static_type_tag();
};


class NumericExpression : public Expression {
protected:
	using Expression::Expression;

public:
	typedef NumericExpression expression_t;
	virtual ExpressionTypeTag dynamic_type_tag() const override;
	static ExpressionTypeTag static_type_tag();
};


class SymbolicExpression : public Expression {
protected:
	using Expression::Expression;

public:
	typedef SymbolicExpression expression_t;
	virtual ExpressionTypeTag dynamic_type_tag() const override;
	static ExpressionTypeTag static_type_tag();
};


class StringExpression : public Expression {
protected:
	using Expression::Expression;

public:
	typedef StringExpression expression_t;
	virtual ExpressionTypeTag dynamic_type_tag() const override;
	static ExpressionTypeTag static_type_tag();
};


class VoidExpression : public Expression {
protected:
	using Expression::Expression;

public:
	typedef VoidExpression expression_t;
	virtual ExpressionTypeTag dynamic_type_tag() const override;
	static ExpressionTypeTag static_type_tag();
};


}

#endif // GOLOGPP_EXPRESSIONS_H_
