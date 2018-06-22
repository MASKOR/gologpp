#include "expressions.h"
#include "scope.h"


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


ExpressionTypeTag NumericExpression::expression_type_tag() const
{ return VALUE_EXPRESSION; }


ExpressionTypeTag Statement::expression_type_tag() const
{ return STATEMENT; }


Global::Global(const string &name, const vector<shared_ptr<AbstractVariable>> &args)
: Identifier (name, static_cast<arity_t>(args.size()))
, args_(args)
{ global_scope().register_global(this); }


const vector<shared_ptr<AbstractVariable>> &Global::args() const
{ return args_; }

vector<shared_ptr<AbstractVariable>> &Global::args()
{ return args_; }

shared_ptr<AbstractVariable> Global::argument(arity_t idx) const
{ return args_[idx]; }



}
