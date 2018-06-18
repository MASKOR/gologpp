#include "expressions.h"


namespace gologpp {


Expression::Expression(Scope &parent_scope)
: parent_scope_(parent_scope)
{}


Scope &Expression::parent_scope()
{ return parent_scope_; }


const Scope &Expression::parent_scope() const
{ return parent_scope_; }


ExpressionTypeTag BooleanExpression::expression_type_tag()
{ return BOOLEAN_EXPRESSION; }


ExpressionTypeTag NumericExpression::expression_type_tag()
{ return VALUE_EXPRESSION; }


ExpressionTypeTag Statement::expression_type_tag()
{ return STATEMENT; }


Global::Global(const string &name, const vector<shared_ptr<AbstractVariable>> &args)
: Identifier (name, static_cast<arity_t>(args.size()))
, args_(args)
{}


const vector<shared_ptr<AbstractVariable>> &Global::args() const
{ return args_; }

vector<shared_ptr<AbstractVariable>> &Global::args()
{ return args_; }

shared_ptr<AbstractVariable> Global::argument(arity_t idx) const
{ return args_[idx]; }



}
