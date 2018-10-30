#ifndef GOLOGPP_STRING_CONCATENATION_H_
#define GOLOGPP_STRING_CONCATENATION_H_

#include "expressions.h"
#include "language.h"
#include "scope.h"


namespace gologpp {


class StringConcatenation : public StringExpression, public NoScopeOwner, public LanguageElement<StringConcatenation> {
public:
	StringConcatenation(StringExpression *lhs, StringExpression *rhs);

	const StringExpression &rhs() const;
	const StringExpression &lhs() const;

	DEFINE_IMPLEMENT_WITH_MEMBERS(*lhs_, *rhs_)

	virtual string to_string(const string &pfx) const override;

private:
	unique_ptr<StringExpression> lhs_, rhs_;
};



}

#endif /// GOLOGPP_STRING_CONCATENATION_H_
