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
	virtual Type::Tag dynamic_type_tag() const;

protected:
	AbstractLanguageElement *parent_;
};



template<class TypeT>
class TypedExpression : public Expression {
public:
	using expression_t = TypedExpression<TypeT>;
	using type_t = TypeT;

	virtual const TypeT &type() const override
	{ return dynamic_cast<const TypeT &>(*type_); }

	static Type::Tag static_type_tag()
	{ return TypeT::tag(); }

protected:
	using Expression::Expression;

	TypedExpression()
	: Expression()
	{ set_type_by_name(TypeT::static_name()); }
};



template<>
TypedExpression<CompoundType>::TypedExpression();



void ensure_type_equality(const Expression &e1, const Expression &e2);


}

#endif // GOLOGPP_EXPRESSIONS_H_
