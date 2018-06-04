#ifndef GOLOGPP_EXPRESSIONS_H_
#define GOLOGPP_EXPRESSIONS_H_

#include <memory>
#include "language.h"
#include "gologpp.h"

namespace gologpp {


enum ExpressionTypeTag {
	BOOLEAN_EXPRESSION,
	VALUE_EXPRESSION,
	STATEMENT
};


class Expression : public std::enable_shared_from_this<Expression>, public virtual AbstractLanguageElement {
protected:
	Expression(Scope &parent_scope = global_scope());
	Expression(const Expression &) = delete;
	Expression(Expression &&) = default;
	Expression &operator = (const Expression &) = delete;

public:
	virtual ~Expression() = default;

	Scope &parent_scope();
	const Scope &parent_scope() const;

	static ExpressionTypeTag expression_type_tag();

private:
	Scope &parent_scope_;
};


class BooleanExpression : public virtual Expression {
public:
	typedef BooleanExpression expression_t;

	BooleanExpression(BooleanExpression &&) = default;
	using Expression::Expression;

	static ExpressionTypeTag expression_type_tag();
};


class NumericExpression : public virtual Expression {
public:
	typedef NumericExpression expression_t;

	NumericExpression(NumericExpression &&) = default;
	using Expression::Expression;

	static ExpressionTypeTag expression_type_tag();
};


class Statement : public virtual Expression {
public:
	typedef Statement expression_t;

	Statement(Statement &&) = default;
	using Expression::Expression;

	static ExpressionTypeTag expression_type_tag();
};


typedef BooleanExpression Bool;
typedef NumericExpression Number;


}

#endif // GOLOGPP_EXPRESSIONS_H_
