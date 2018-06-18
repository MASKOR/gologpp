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


template<ExpressionTypeTag tag>
struct ExpressionTagToType {
};

template<>
struct ExpressionTagToType<ExpressionTypeTag::BOOLEAN_EXPRESSION> {
	typedef BooleanExpression type;
};

template<>
struct ExpressionTagToType<ExpressionTypeTag::VALUE_EXPRESSION> {
	typedef NumericExpression type;
};

template<>
struct ExpressionTagToType<ExpressionTypeTag::STATEMENT> {
	typedef Statement type;
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


class Global : public Identifier, public virtual Expression {
public:
	Global(const string &name, const vector<shared_ptr<AbstractVariable>> &args);

	vector<shared_ptr<AbstractVariable>> &args();
	const vector<shared_ptr<AbstractVariable>> &args() const;
	shared_ptr<AbstractVariable> argument(arity_t idx) const;

private:
	vector<shared_ptr<AbstractVariable>> args_;
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
