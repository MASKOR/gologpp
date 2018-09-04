#ifndef FORMULA_H
#define FORMULA_H

#include "expressions.h"
#include "atoms.h"
#include "scope.h"

#include <vector>
#include <memory>


namespace gologpp {


class Negation : public BooleanExpression, public LanguageElement<Negation> {
public:
	Negation(unique_ptr<BooleanExpression> &&expression, Scope &parent_scope);
	Negation(Negation &&) = default;
	virtual ~Negation() override = default;

	const BooleanExpression &expression() const;

	DEFINE_IMPLEMENT_WITH_MEMBERS(*expression_)

	virtual string to_string(const string &pfx) const override;

protected:
	unique_ptr<BooleanExpression> expression_;
};


enum ComparisonOperator {
	EQ = 1, NEQ, GE, GT, LE, LT
};


string to_string(ComparisonOperator op);


class Comparison : public BooleanExpression, public LanguageElement<Comparison> {
public:
	Comparison(NumericExpression *lhs, ComparisonOperator op, NumericExpression *rhs, Scope &parent_scope);
	ComparisonOperator op() const;
	const NumericExpression &lhs() const;
	const NumericExpression &rhs() const;

	DEFINE_IMPLEMENT_WITH_MEMBERS(*lhs_, *rhs_)

	virtual string to_string(const string &pfx) const override;

protected:
	unique_ptr<NumericExpression> lhs_;
	ComparisonOperator op_;
	unique_ptr<NumericExpression> rhs_;
};



/*--------------------------------------------*\
  Connective formulas, i.e. AND, OR, IMPLIES
\*--------------------------------------------*/

enum BooleanOperator {
	AND = 1, OR, XOR, IMPLIES, IFF
};

string to_string(BooleanOperator op);


class BooleanOperation : public BooleanExpression {
public:
	BooleanOperation(BooleanExpression *lhs, BooleanOperator op, BooleanExpression *rhs, Scope &parent_scope);
	BooleanOperator op() const;
	const BooleanExpression &lhs() const;
	const BooleanExpression &rhs() const;

	DEFINE_IMPLEMENT_WITH_MEMBERS(*lhs_, *rhs_)

	virtual string to_string(const string &pfx) const override;

protected:
	unique_ptr<BooleanExpression> lhs_;
	BooleanOperator op_;
	unique_ptr<BooleanExpression> rhs_;
};



/*--------------------------------------------*\
  Set expressions & Quantifications
\*--------------------------------------------*/


class AbstractVariable;

enum QuantificationOperator {
	EXISTS = 1, FORALL
};

string to_string(QuantificationOperator op);


class Quantification : public BooleanExpression, public ScopeOwner {
public:
	Quantification(
		Scope *own_scope,
		QuantificationOperator op,
		const shared_ptr<AbstractVariable> &variable,
		BooleanExpression *expression
	);

	template<class ExprT>
	Quantification(
		QuantificationOperator op,
		const string &var_name,
		BooleanExpression *expression,
		Scope &parent_scope
	)
	: BooleanExpression(parent_scope)
	, ScopeOwner(new Scope(this, {}, parent_scope))
	, op_(op)
	, variable_(scope().lookup_var(var_name))
	, expression_(expression)
	{}

	QuantificationOperator op() const;
	const AbstractVariable &variable() const;
	const BooleanExpression &expression() const;

	DEFINE_IMPLEMENT_WITH_MEMBERS(*variable_, *expression_)

	virtual string to_string(const string &pfx) const override;

protected:
	QuantificationOperator op_;
	shared_ptr<AbstractVariable> variable_;
	unique_ptr<BooleanExpression> expression_;
};



} // namespace gologpp



#endif // FORMULA_H
