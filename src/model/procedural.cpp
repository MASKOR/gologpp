#include <functional>
#include <cstdlib>

#include "procedural.h"
#include "formula.h"
#include "execution.h"
#include "reference.h"

#include "user_error.h"

namespace gologpp {


Block::Block(Scope *own_scope, const vector<Statement *> &elements)
: ScopeOwner(own_scope)
{
	for (Statement *stmt : elements) {
		stmt->set_parent(this);
		elements_.emplace_back(stmt);
	}
}

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
: ScopeOwner(own_scope)
{
	for (Statement *stmt : alternatives) {
		stmt->set_parent(this);
		alternatives_.emplace_back(stmt);
	}
}

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
	BooleanExpression *condition,
	Statement *block_true,
	const boost::optional<Statement *> &block_false
)
: condition_(condition)
, block_true_(block_true)
, block_false_(block_false.value_or(nullptr))
{
	condition_->set_parent(this);
	block_true_->set_parent(this);
	block_false_->set_parent(this);
}

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
: ScopeOwner(own_scope)
, variable_(std::move(variable))
, statement_(statement)
{
	dynamic_cast<Expression *>(variable_.get())->set_parent(this);
	statement_->set_parent(this);
}

const AbstractVariable &Pick::variable() const
{ return *variable_; }

const Statement &Pick::statement() const
{ return *statement_; }

string Pick::to_string(const string &pfx) const
{ return linesep + pfx + "pick (" + variable().to_string("") + "): " + statement().to_string(pfx); }



Search::Search(Statement *statement)
: statement_(statement)
{
	statement_->set_parent(this);
}

const Statement &Search::statement() const
{ return *statement_; }

string Search::to_string(const string &pfx) const
{ return linesep + pfx + "search " + statement().to_string(pfx); }



Solve::Solve(
	NumericExpression *horizon,
	Reference<NumericFunction> *reward,
	Statement *statement
)
: Search(statement)
, horizon_(horizon)
, reward_(reward)
{
	horizon_->set_parent(this);
	reward_->set_parent(this);
}

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



Test::Test(BooleanExpression *expression)
: expression_(expression)
{
	expression_->set_parent(this);
}

const BooleanExpression &Test::expression() const
{ return *expression_; }

string Test::to_string(const string &pfx) const
{ return linesep + pfx + "test (" + expression().to_string("") + ')'; }



While::While(BooleanExpression *expression, Statement *statement)
: expression_(expression)
, statement_(statement)
{
	expression_->set_parent(this);
	statement_->set_parent(this);
}

const BooleanExpression &While::expression() const
{ return *expression_; }

const Statement &While::statement() const
{ return *statement_; }

string While::to_string(const string &pfx) const
{ return linesep + pfx + "while (" + expression().to_string("") + ") " + statement().to_string(pfx); }





AbstractFunction::AbstractFunction(Scope *own_scope, const string &name, const vector<shared_ptr<AbstractVariable>> &args)
: Global(name, args)
, ScopeOwner(own_scope)
{}

AbstractFunction::~AbstractFunction()
{}

const Statement &AbstractFunction::definition() const
{ return *definition_; }

void AbstractFunction::define(Statement *definition)
{
	definition_.reset(definition);
	definition_->set_parent(this);
}

void AbstractFunction::compile(AExecutionContext &ctx)
{ ctx.compile(*this); }



} // namespace gologpp
