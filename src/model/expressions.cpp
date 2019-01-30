#include "expressions.h"
#include "scope.h"


namespace gologpp {


string to_string(ExpressionTypeTag t)
{
	switch (t) {
	case BOOLEAN:
		return "?";
	case NUMERIC:
		return "%";
	case SYMBOLIC:
		return "§";
	case STRING:
		return "$";
	case VOID:
		return "";
	}
	return "[Unknown ExpressionTypeTag]";
}


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



ExpressionTypeTag BooleanExpression::dynamic_type_tag() const
{ return BOOLEAN; }

ExpressionTypeTag BooleanExpression::static_type_tag()
{ return BOOLEAN; }



ExpressionTypeTag NumericExpression::dynamic_type_tag() const
{ return NUMERIC; }

ExpressionTypeTag NumericExpression::static_type_tag()
{ return NUMERIC; }



ExpressionTypeTag SymbolicExpression::dynamic_type_tag() const
{ return SYMBOLIC; }

ExpressionTypeTag SymbolicExpression::static_type_tag()
{ return SYMBOLIC; }


ExpressionTypeTag StringExpression::dynamic_type_tag() const
{ return STRING; }

ExpressionTypeTag StringExpression::static_type_tag()
{ return STRING; }


ExpressionTypeTag VoidExpression::dynamic_type_tag() const
{ return VOID; }

ExpressionTypeTag VoidExpression::static_type_tag()
{ return VOID; }


}
