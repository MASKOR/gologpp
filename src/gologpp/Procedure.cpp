#include <functional>
#include <cstdlib>

#include "Procedure.h"
#include "Formula.h"

#include "user_error.h"

namespace gologpp {
namespace generic {


Statement::~Statement()
{}


Block::Block(const vector<shared_ptr<Statement>> &elements, const shared_ptr<Scope> &parent_scope)
: Statement(parent_scope)
, elements_(elements)
{}


Choose::Choose(const vector<shared_ptr<Block>> &alternatives, const shared_ptr<Scope> &parent_scope)
: Statement(parent_scope)
, alternatives_(alternatives)
{}


Conditional::Conditional(const shared_ptr<BooleanExpression> &condition,
                         const shared_ptr<Block> &block_true,
                         const shared_ptr<Block> &block_false,
                         const shared_ptr<Scope> &parent_scope)
    : Statement(parent_scope)
    , condition_(condition)
    , block_true_(block_true)
    , block_false_(block_false)
{}


Assignment::Assignment(Reference<Fluent> &&fluent, const shared_ptr<Expression> &expression, const shared_ptr<Scope> &parent_scope)
: Statement(parent_scope)
, fluent_(std::move(fluent))
, expression_(expression)
{}


Pick::Pick(const shared_ptr<Variable> &variable, const shared_ptr<Block> &block, const shared_ptr<Scope> &parent_scope)
: Statement(parent_scope)
, variable_(variable), block_(block)
{}


Call::Call(const shared_ptr<Action> &action, const vector<shared_ptr<Expression>> &args, const shared_ptr<Scope> &parent_scope)
: Statement(parent_scope)
, action_(action)
, args_(args)
{}


Search::Search(unique_ptr<Block> &&block, const shared_ptr<Scope> &parent_scope)
: Statement(parent_scope)
, block_(std::move(block))
{}


Test::Test(const shared_ptr<BooleanExpression> &expression, const shared_ptr<Scope> &parent_scope)
: Statement(parent_scope)
, expression_(expression)
{}


While::While(const shared_ptr<BooleanExpression> &expression, unique_ptr<Block> &&block, const shared_ptr<Scope> &parent_scope)
: Statement(parent_scope)
, expression_(expression), block_(std::move(block))
{}


Procedure::Procedure(const string &name, const vector<string> &arg_names, unique_ptr<Block> &&block)
: Statement(Scope::global_scope())
, NameWithArity(name, static_cast<arity_t>(arg_names.size()))
, scope_(arg_names, Scope::global_scope())
, block_(std::move(block))
{}



} // namespace generic
} // namespace gologpp
