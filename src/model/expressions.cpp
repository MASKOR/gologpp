#include "expressions.h"
#include "scope.h"


namespace gologpp {

Type::Tag Expression::dynamic_type_tag() const
{ return type_->dynamic_tag(); }

Expression::Expression()
: parent_(nullptr)
{}

Scope &Expression::parent_scope()
{ return parent_->scope(); }

const Scope &Expression::parent_scope() const
{ return parent_->scope(); }

AbstractLanguageElement *Expression::parent()
{ return parent_; }

const AbstractLanguageElement *Expression::parent() const
{ return parent_; }

void Expression::set_parent(AbstractLanguageElement *parent)
{ parent_ = parent; }



void ensure_type_equality(const Expression &e1, const Expression &e2)
{
	if (e1.type() != e2.type())
		throw ExpressionTypeMismatch(e1, e2);
}



template<>
TypedExpression<CompoundType>::TypedExpression()
: Expression()
{}


}
