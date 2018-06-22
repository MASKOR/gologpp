#ifndef GOLOGPP_EXPRESSIONS_H_
#define GOLOGPP_EXPRESSIONS_H_

#include <memory>
#include "language.h"
#include "utilities.h"
#include "gologpp.h"

namespace gologpp {


enum ExpressionTypeTag {
	UNKNOWN,
	BOOLEAN_EXPRESSION,
	VALUE_EXPRESSION,
	STATEMENT,
};


class Expression : public virtual AbstractLanguageElement {
protected:
	Expression(Scope &parent_scope = global_scope());
	Expression(const Expression &) = delete;
	Expression(Expression &&) = default;
	Expression &operator = (const Expression &) = delete;

public:
	virtual ~Expression() = default;

	Scope &parent_scope();
	const Scope &parent_scope() const;

	virtual ExpressionTypeTag expression_type_tag() const = 0;

private:
	Scope &parent_scope_;
};


class Global : public Identifier, public virtual Expression {
protected:
	Global(const string &name, const vector<shared_ptr<AbstractVariable>> &args);

public:
	vector<shared_ptr<AbstractVariable>> &args();
	const vector<shared_ptr<AbstractVariable>> &args() const;
	shared_ptr<AbstractVariable> argument(arity_t idx) const;

private:
	vector<shared_ptr<AbstractVariable>> args_;
};


class BooleanExpression : public virtual Expression {
protected:
	BooleanExpression(BooleanExpression &&) = default;
	using Expression::Expression;

public:
	typedef BooleanExpression expression_t;
	virtual ExpressionTypeTag expression_type_tag() const override;
};


class NumericExpression : public virtual Expression {
protected:
	NumericExpression(NumericExpression &&) = default;
	using Expression::Expression;

public:
	typedef NumericExpression expression_t;
	virtual ExpressionTypeTag expression_type_tag() const override;
};


class Statement : public virtual Expression {
protected:
	Statement(Statement &&) = default;
	using Expression::Expression;

public:
	typedef Statement expression_t;
	virtual ExpressionTypeTag expression_type_tag() const override;
};


typedef BooleanExpression Bool;
typedef NumericExpression Number;


}

#endif // GOLOGPP_EXPRESSIONS_H_
