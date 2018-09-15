#include "expressions.h"
#include "scope.h"


namespace gologpp {


string to_string(ExpressionTypeTag t)
{
	switch (t) {
	case BOOLEAN_EXPRESSION:
		return "?";
	case VALUE_EXPRESSION:
		return "%";
	case STATEMENT:
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


bool Expression::is_ref() const
{ return false; }

AbstractLanguageElement *Expression::parent()
{ return parent_; }

const AbstractLanguageElement *Expression::parent() const
{ return parent_; }

void Expression::set_parent(AbstractLanguageElement *parent)
{ parent_ = parent; }



ExpressionTypeTag BooleanExpression::expression_type_tag() const
{ return BOOLEAN_EXPRESSION; }

ExpressionTypeTag BooleanExpression::static_type_tag()
{ return BOOLEAN_EXPRESSION; }



ExpressionTypeTag NumericExpression::expression_type_tag() const
{ return VALUE_EXPRESSION; }

ExpressionTypeTag NumericExpression::static_type_tag()
{ return VALUE_EXPRESSION; }



ExpressionTypeTag Statement::expression_type_tag() const
{ return STATEMENT; }

ExpressionTypeTag Statement::static_type_tag()
{ return STATEMENT; }



}
