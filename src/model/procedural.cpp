#include <functional>
#include <cstdlib>

#include "procedural.h"
#include "formula.h"

#include "user_error.h"

namespace gologpp {


Block::Block(const vector<Statement *> &elements, Scope &parent_scope)
: Statement(parent_scope)
, elements_(elements.begin(), elements.end())
{}

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



Choose::Choose(const vector<Statement *> &alternatives, Scope &parent_scope)
: Statement(parent_scope)
, alternatives_(alternatives.begin(), alternatives.end())
{}

const vector<unique_ptr<Statement>> &Choose::alternatives() const
{ return alternatives_; }

void Choose::implement(Implementor &implementor)
{
	if (!impl_) {
		impl_ = implementor.make_impl(*this);
		for (unique_ptr<Statement> &stmt : alternatives_)
			stmt->implement(implementor);
	}
}



Conditional::Conditional(unique_ptr<BooleanExpression> &&condition,
                         unique_ptr<Statement> &&block_true,
                         unique_ptr<Statement> &&block_false,
                         Scope &parent_scope)
    : Statement(parent_scope)
    , condition_(std::move(condition))
    , block_true_(std::move(block_true))
    , block_false_(std::move(block_false))
{}

const BooleanExpression &Conditional::condition() const
{ return *condition_; }

const Statement &Conditional::block_false() const
{ return *block_false_; }

const Statement &Conditional::block_true() const
{ return *block_true_; }



Pick::Pick(const shared_ptr<AbstractVariable> &variable, Statement *statement, Scope &parent_scope)
: Statement(parent_scope)
, variable_(std::move(variable))
, statement_(statement)
, scope_(this, {variable}, parent_scope)
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



AbstractFunction::AbstractFunction(const string &name, const vector<shared_ptr<AbstractVariable>> &args, Block &&block)
: Global(name, args)
, scope_(this, {}, Scope::global_scope())
, block_(std::move(block))
{}

AbstractFunction::~AbstractFunction()
{}

const Scope &AbstractFunction::scope() const
{ return scope_; }

const Block &AbstractFunction::block() const
{ return block_; }


} // namespace gologpp
