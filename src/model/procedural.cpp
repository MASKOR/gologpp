#include <functional>
#include <cstdlib>

#include "procedural.h"
#include "formula.h"
#include "execution.h"
#include "reference.h"

#include "user_error.h"

namespace gologpp {


Block::Block(Scope *own_scope, const vector<Statement *> &elements)
: Statement(own_scope->parent_scope())
, ScopeOwner(own_scope)
, elements_(elements.begin(), elements.end())
{}

void Block::implement(Implementor &implementor)
{
	if (!impl_) {
		impl_ = implementor.make_impl(*this);
		scope().implement(implementor);
		for (auto &stmt : elements_)
			stmt->implement(implementor);
	}
}

const vector<unique_ptr<Statement>> &Block::elements() const
{ return elements_; }


string Block::to_string(const string &pfx) const
{
	if (elements().size() == 0)
		return "{}";
	else if (elements().size() == 1)
		return elements()[0]->to_string(pfx);
	else {
		return pfx + "{" + linesep
			+ concat_list(elements(), ";" linesep + pfx + indent, pfx + indent) + ";" linesep
			+ pfx + "}" + linesep;
	}
}



Choose::Choose(Scope *own_scope, const vector<Statement *> &alternatives)
: Statement(own_scope->parent_scope())
, ScopeOwner(own_scope)
, alternatives_(alternatives.begin(), alternatives.end())
{}

const vector<unique_ptr<Statement>> &Choose::alternatives() const
{ return alternatives_; }


void Choose::implement(Implementor &implementor)
{
	if (!impl_) {
		impl_ = implementor.make_impl(*this);
		scope().implement(implementor);
		for (unique_ptr<Statement> &stmt : alternatives_)
			stmt->implement(implementor);
	}
}


string Choose::to_string(const string &pfx) const
{
	return linesep + pfx + "choose {" + linesep
		+ concat_list(alternatives(), ";" linesep + pfx + indent, pfx + indent) + ";" linesep
		+ pfx + "}" linesep;
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


string Conditional::to_string(const string &pfx) const
{
	return linesep + pfx + "if (" + condition().to_string("") + ") " + block_true().to_string(pfx)
		+ (block_false_ ? pfx + linesep + "else" + block_false().to_string(pfx) : "");
}



Pick::Pick(Scope *own_scope, const shared_ptr<AbstractVariable> &variable, Statement *statement)
: Statement(own_scope->parent_scope())
, ScopeOwner(own_scope)
, variable_(std::move(variable))
, statement_(statement)
{}

Pick::Pick(const string &var_name, Statement *statement, Scope &parent_scope)
: Statement(parent_scope)
, ScopeOwner(new Scope(this, {}, parent_scope))
, variable_(scope().lookup_var(var_name))
, statement_(statement)
{}

const AbstractVariable &Pick::variable() const
{ return *variable_; }

const Statement &Pick::statement() const
{ return *statement_; }

string Pick::to_string(const string &pfx) const
{ return linesep + pfx + "pick (" + variable().to_string("") + "): " + statement().to_string(pfx); }



Search::Search(Statement *statement, Scope &parent_scope)
: Statement(parent_scope)
, statement_(statement)
{}

const Statement &Search::statement() const
{ return *statement_; }

string Search::to_string(const string &pfx) const
{ return linesep + pfx + "search " + statement().to_string(pfx); }



Solve::Solve(
	NumericExpression *horizon,
	Reference<NumericFunction> *reward,
	Statement *statement,
	Scope &parent_scope
)
: Search(statement, parent_scope)
, horizon_(horizon)
, reward_(reward)
{}

const NumericExpression &Solve::horizon() const
{ return *horizon_; }

const Reference<NumericFunction> &Solve::reward() const
{ return *reward_; }


void Solve::implement(Implementor &implementor)
{
	if (impl_)
		return;
	horizon_->implement(implementor);
	reward_->implement(implementor);
	statement_->implement(implementor);
	impl_ = implementor.make_impl(*this);
}


string Solve::to_string(const string &pfx) const
{
	return linesep + pfx + "solve ("
		+ horizon().to_string(pfx) + ", "
		+ reward().to_string(pfx) + ")"
		+ statement().to_string(pfx + indent);
}



Test::Test(BooleanExpression *expression, Scope &parent_scope)
: Statement(parent_scope)
, expression_(expression)
{}

const BooleanExpression &Test::expression() const
{ return *expression_; }

string Test::to_string(const string &pfx) const
{ return linesep + pfx + "test (" + expression().to_string("") + ')'; }



While::While(BooleanExpression *expression, Statement *statement, Scope &parent_scope)
: Statement(parent_scope)
, expression_(expression)
, statement_(statement)
{}

const BooleanExpression &While::expression() const
{ return *expression_; }

const Statement &While::statement() const
{ return *statement_; }

string While::to_string(const string &pfx) const
{ return linesep + pfx + "while (" + expression().to_string("") + ") " + statement().to_string(pfx); }





AbstractFunction::AbstractFunction(Scope *own_scope, const string &name, const vector<shared_ptr<AbstractVariable>> &args, Statement *definition)
: Global(name, args)
, ScopeOwner(own_scope)
, definition_(definition)
{}

AbstractFunction::~AbstractFunction()
{}

const Statement &AbstractFunction::definition() const
{ return *definition_; }

void AbstractFunction::define(boost::optional<Statement *> definition)
{ definition_.reset(definition.value()); }

void AbstractFunction::compile(AExecutionContext &ctx)
{ ctx.compile(*this); }



} // namespace gologpp
