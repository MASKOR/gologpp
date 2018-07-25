#include <functional>
#include <cstdlib>

#include "procedural.h"
#include "formula.h"
#include "execution.h"

#include "user_error.h"

namespace gologpp {


Block::Block(Scope *own_scope, const vector<Statement *> &elements)
: Statement(own_scope->parent_scope())
, scope_(own_scope)
, elements_(elements.begin(), elements.end())
{ own_scope->set_owner(this); }

void Block::implement(Implementor &implementor)
{
	if (!impl_) {
		impl_ = implementor.make_impl(*this);
		for (auto &stmt : elements_)
			stmt->implement(implementor);
	}
}

const vector<unique_ptr<Statement>> &Block::elements() const
{ return elements_; }



Choose::Choose(Scope *own_scope, const vector<Statement *> &alternatives)
: Statement(own_scope->parent_scope())
, scope_(own_scope)
, alternatives_(alternatives.begin(), alternatives.end())
{ own_scope->set_owner(this); }

const vector<unique_ptr<Statement>> &Choose::alternatives() const
{ return alternatives_; }

void Choose::implement(Implementor &implementor)
{
	if (!impl_) {
		impl_ = implementor.make_impl(*this);
		scope_->implement(implementor);
		for (unique_ptr<Statement> &stmt : alternatives_)
			stmt->implement(implementor);
	}
}



Conditional::Conditional(
	unique_ptr<BooleanExpression> &&condition,
	unique_ptr<Statement> &&block_true,
	unique_ptr<Statement> &&block_false,
	Scope &parent_scope
)
: Statement(parent_scope)
, condition_(std::move(condition))
, block_true_(std::move(block_true))
, block_false_(std::move(block_false))
{}

Conditional::Conditional(
	BooleanExpression *condition,
	Statement *block_true,
	const boost::optional<Statement *> &block_false,
	Scope &parent_expr
)
: Conditional(
	unique_ptr<BooleanExpression>(condition),
	unique_ptr<Statement>(block_true),
	unique_ptr<Statement>(block_false.value_or(nullptr)),
	parent_expr
)
{}

const BooleanExpression &Conditional::condition() const
{ return *condition_; }

const Statement &Conditional::block_false() const
{ return *block_false_; }

const Statement &Conditional::block_true() const
{ return *block_true_; }



Pick::Pick(Scope *own_scope, const shared_ptr<AbstractVariable> &variable, Statement *statement)
: Statement(own_scope->parent_scope())
, scope_(own_scope)
, variable_(std::move(variable))
, statement_(statement)
{ own_scope->set_owner(this); }

Pick::Pick(const string &var_name, Statement *statement, Scope &parent_scope)
: Statement(parent_scope)
, scope_(new Scope(this, {}, parent_scope))
, variable_(scope_->variable(var_name))
, statement_(statement)
{}

const AbstractVariable &Pick::variable() const
{ return *variable_; }

const Statement &Pick::statement() const
{ return *statement_; }



Search::Search(Statement *statement, Scope &parent_scope)
: Statement(parent_scope)
, statement_(statement)
{}

const Statement &Search::statement() const
{ return *statement_; }



Test::Test(BooleanExpression *expression, Scope &parent_scope)
: Statement(parent_scope)
, expression_(expression)
{}

const BooleanExpression &Test::expression() const
{ return *expression_; }



While::While(BooleanExpression *expression, Statement *statement, Scope &parent_scope)
: Statement(parent_scope)
, expression_(expression)
, statement_(statement)
{}

const BooleanExpression &While::expression() const
{ return *expression_; }

const Statement &While::statement() const
{ return *statement_; }



AbstractFunction::AbstractFunction(Scope *own_scope, const string &name, const vector<shared_ptr<AbstractVariable>> &args, Statement *definition)
: Global(name, args)
, scope_(own_scope)
, definition_(definition)
{ own_scope->set_owner(this); }

AbstractFunction::~AbstractFunction()
{}

const Scope &AbstractFunction::scope() const
{ return *scope_; }

const Statement &AbstractFunction::definition() const
{ return *definition_; }

void AbstractFunction::define(boost::optional<Statement *> definition)
{ definition_.reset(definition.value()); }

void AbstractFunction::compile(AExecutionContext &ctx)
{ ctx.compile(*this); }


} // namespace gologpp
