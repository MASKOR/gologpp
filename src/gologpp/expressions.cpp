#include "expressions.h"


namespace gologpp {


Expression::Expression(Scope &parent_scope)
: parent_scope_(parent_scope)
{}


Scope &Expression::parent_scope()
{ return parent_scope_; }


const Scope &Expression::parent_scope() const
{ return parent_scope_; }


ExpressionTypeTag BooleanExpression::expression_type_tag() const
{ return BOOLEAN_EXPRESSION; }


ExpressionTypeTag ValueExpression::expression_type_tag() const
{ return VALUE_EXPRESSION; }


ExpressionTypeTag Statement::expression_type_tag() const
{ return STATEMENT; }


}
