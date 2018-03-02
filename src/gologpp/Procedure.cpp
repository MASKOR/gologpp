#include <functional>
#include <cstdlib>

#include "Procedure.h"
#include "Formula.h"

#include "user_error.h"

namespace gologpp {


Statement::~Statement()
{}


Block::Block(vector<unique_ptr<Statement>> &&elements, Scope &parent_scope)
: Statement(parent_scope)
, elements_(std::move(elements))
{}


void Block::implement(Implementor &implementor)
{
	for (auto &stmt : elements_)
		stmt->implement(implementor);
}


Choose::Choose(vector<unique_ptr<Block>> &&alternatives, Scope &parent_scope)
: Statement(parent_scope)
, alternatives_(std::move(alternatives))
{}


void Choose::implement(Implementor &implementor)
{
	for (auto &stmt : alternatives_)
		stmt->implement(implementor);
}


Conditional::Conditional(unique_ptr<BooleanExpression> &&condition,
                         unique_ptr<Block> &&block_true,
                         unique_ptr<Block> &&block_false,
                         Scope &parent_scope)
    : Statement(parent_scope)
    , condition_(std::move(condition))
    , block_true_(std::move(block_true))
    , block_false_(std::move(block_false))
{}


Assignment::Assignment(Reference<Fluent> &&fluent, unique_ptr<Expression> &&expression, Scope &parent_scope)
: Statement(parent_scope)
, fluent_(std::move(fluent))
, expression_(std::move(expression))
{}


Pick::Pick(const shared_ptr<Variable> &variable, unique_ptr<Block> &&block, Scope &parent_scope)
: Statement(parent_scope)
, variable_(std::move(variable)), block_(std::move(block))
{}


Search::Search(unique_ptr<Block> &&block, Scope &parent_scope)
: Statement(parent_scope)
, block_(std::move(block))
{}


Test::Test(unique_ptr<BooleanExpression> &&expression, Scope &parent_scope)
: Statement(parent_scope)
, expression_(std::move(expression))
{}


While::While(unique_ptr<BooleanExpression> &&expression, unique_ptr<Block> &&block, Scope &parent_scope)
: Statement(parent_scope)
, expression_(std::move(expression)), block_(std::move(block))
{}


Procedure::Procedure(const string &name, const vector<string> &arg_names, unique_ptr<Block> &&block)
: Statement(Scope::global_scope())
, Identifier(name, static_cast<arity_t>(arg_names.size()))
, scope_(arg_names, Scope::global_scope())
, block_(std::move(block))
, args_(arg_names)
{}

const Block &Procedure::block() const
{ return *block_; }

const Scope &Procedure::scope() const
{ return scope_; }

const vector<string> &Procedure::args() const
{ return args_; }


} // namespace gologpp
