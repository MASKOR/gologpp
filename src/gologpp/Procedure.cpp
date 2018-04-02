#include <functional>
#include <cstdlib>

#include "Procedure.h"
#include "Formula.h"

#include "user_error.h"

namespace gologpp {


Block::Block(vector<unique_ptr<Statement>> &&elements, Scope &parent_scope)
: Statement(parent_scope)
, elements_(std::move(elements))
{}

void Block::implement(Implementor &implementor)
{
	impl_ = implementor.make_impl(*this);
	for (auto &stmt : elements_)
		stmt->implement(implementor);
}

const vector<unique_ptr<Statement>> &Block::elements() const
{ return elements_; }



Choose::Choose(vector<Block> &&alternatives, Scope &parent_scope)
: Statement(parent_scope)
, alternatives_(std::move(alternatives))
{}

const vector<Block> &Choose::alternatives() const
{ return alternatives_; }

void Choose::implement(Implementor &implementor)
{
	for (Block &block : alternatives_)
		block.implement(implementor);
}



Conditional::Conditional(unique_ptr<BooleanExpression> &&condition,
                         Block &&block_true,
                         Block &&block_false,
                         Scope &parent_scope)
    : Statement(parent_scope)
    , condition_(std::move(condition))
    , block_true_(std::move(block_true))
    , block_false_(std::move(block_false))
{}

const BooleanExpression &Conditional::condition() const
{ return *condition_; }

const Block &Conditional::block_false() const
{ return block_false_; }

const Block &Conditional::block_true() const
{ return block_true_; }



Pick::Pick(const shared_ptr<AbstractVariable> &variable, Block &&block, Scope &parent_scope)
: Statement(parent_scope)
, variable_(std::move(variable))
, block_(std::move(block))
, scope_(this, {variable}, parent_scope)
{}

const AbstractVariable &Pick::variable() const
{ return *variable_; }

const Block &Pick::block() const
{ return block_; }



Search::Search(Block &&block, Scope &parent_scope)
: Statement(parent_scope)
, block_(std::move(block))
{}

const Block &Search::block() const
{ return block_; }



Test::Test(unique_ptr<BooleanExpression> &&expression, Scope &parent_scope)
: Statement(parent_scope)
, expression_(std::move(expression))
{}

const BooleanExpression &Test::expression() const
{ return *expression_; }



While::While(unique_ptr<BooleanExpression> &&expression, Block &&block, Scope &parent_scope)
: Statement(parent_scope)
, expression_(std::move(expression)), block_(std::move(block))
{}

const BooleanExpression &While::expression() const
{ return *expression_; }



AbstractFunction::AbstractFunction(const string &name, const vector<string> &arg_names, Block &&block)
: Expression(Scope::global_scope())
, Identifier(name, static_cast<arity_t>(arg_names.size()))
, scope_(this, {}, Scope::global_scope())
, block_(std::move(block))
, args_(arg_names)
{}

AbstractFunction::~AbstractFunction()
{}

const Scope &AbstractFunction::scope() const
{ return scope_; }

const Block &AbstractFunction::block() const
{ return block_; }

const vector<string> &AbstractFunction::args() const
{ return args_; }

shared_ptr<AbstractVariable> AbstractFunction::argument(arity_t idx) const
{ return scope_.variable(args_[idx]); }



} // namespace gologpp
