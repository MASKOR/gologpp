#ifndef GOLOGPP_STRING_CONCATENATION_H_
#define GOLOGPP_STRING_CONCATENATION_H_

#include "expressions.h"
#include "language.h"
#include "scope.h"


namespace gologpp {


class ToString
: public Expression
, public NoScopeOwner
, public LanguageElement<ToString, StringType> {
public:
	ToString(Expression *expr);

	const Expression &expression() const;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*expr_)

	virtual string to_string(const string &pfx) const override;

private:
	unique_ptr<Expression> expr_;
};



class StringConcatenation : public Expression, public NoScopeOwner, public LanguageElement<StringConcatenation, StringType> {
public:
	StringConcatenation(Expression *lhs, Expression *rhs);

	const Expression &rhs() const;
	const Expression &lhs() const;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*lhs_, *rhs_)

	virtual string to_string(const string &pfx) const override;

private:
	SafeExprOwner<StringType> lhs_, rhs_;
};



}

#endif /// GOLOGPP_STRING_CONCATENATION_H_
