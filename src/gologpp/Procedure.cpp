#include <functional>
#include <cstdlib>

#include "Procedure.h"
#include "Formula.h"

#include "user_error.h"

namespace gologpp {


Statement::~Statement()
{}


Block::Block(const vector<shared_ptr<Statement>> &elements, Scope &parent_scope)
: Statement(parent_scope)
, elements_(elements)
{}


void Block::implement(Implementor &implementor)
{
	for (auto &stmt : elements_)
		stmt->implement(implementor);
}


Choose::Choose(const vector<shared_ptr<Block>> &alternatives, Scope &parent_scope)
: Statement(parent_scope)
, alternatives_(alternatives)
{}


void Choose::implement(Implementor &implementor)
{
	for (auto &stmt : alternatives_)
		stmt->implement(implementor);
}


Conditional::Conditional(const shared_ptr<BooleanExpression> &condition,
                         const shared_ptr<Block> &block_true,
                         const shared_ptr<Block> &block_false,
                         Scope &parent_scope)
    : Statement(parent_scope)
    , condition_(condition)
    , block_true_(block_true)
    , block_false_(block_false)
{}


Assignment::Assignment(Reference<Fluent> &&fluent, const shared_ptr<Expression> &expression, Scope &parent_scope)
: Statement(parent_scope)
, fluent_(std::move(fluent))
, expression_(expression)
{}


Pick::Pick(const shared_ptr<Variable> &variable, const shared_ptr<Block> &block, Scope &parent_scope)
: Statement(parent_scope)
, variable_(variable), block_(block)
{}


Search::Search(unique_ptr<Block> &&block, Scope &parent_scope)
: Statement(parent_scope)
, block_(std::move(block))
{}


Test::Test(const shared_ptr<BooleanExpression> &expression, Scope &parent_scope)
: Statement(parent_scope)
, expression_(expression)
{}


While::While(const shared_ptr<BooleanExpression> &expression, unique_ptr<Block> &&block, Scope &parent_scope)
: Statement(parent_scope)
, expression_(expression), block_(std::move(block))
{}


Procedure::Procedure(const string &name, const vector<string> &arg_names, unique_ptr<Block> &&block)
: Statement(Scope::global_scope())
, Identifier(name, static_cast<arity_t>(arg_names.size()))
, scope_(arg_names, Scope::global_scope())
, block_(std::move(block))
{}



} // namespace gologpp
