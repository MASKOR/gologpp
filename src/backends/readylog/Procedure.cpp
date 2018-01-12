#include <functional>
#include <cstdlib>

#include "Procedure.h"
#include "Formula.h"

#include "user_error.h"

namespace gologpp {


Statement::~Statement()
{}


Block::Block(const vector<shared_ptr<Statement>> &elements, InScope &parent_expr)
: Statement(parent_expr)
, elements_(elements)
{}


Choose::Choose(const vector<shared_ptr<Block>> &alternatives, InScope &parent_expr)
: Statement(parent_expr)
, alternatives_(alternatives)
{}


Conditional::Conditional(const shared_ptr<Expression> &condition,
                         const shared_ptr<Block> &block_true,
                         const shared_ptr<Block> &block_false,
                         InScope &parent_expr)
    : Statement(parent_expr)
    , condition_(condition)
    , block_true_(block_true)
    , block_false_(block_false)
{}


Assignment::Assignment(const shared_ptr<Fluent> &fluent, const shared_ptr<Expression> &expression, InScope &parent_expr)
: Statement(parent_expr)
, fluent_(fluent), expression_(expression)
{}


Pick::Pick(const shared_ptr<Variable> &variable, const shared_ptr<Block> &block, InScope &parent_expr)
: Statement(parent_expr)
, variable_(variable), block_(block)
{}


Call::Call(const shared_ptr<Action> &action, const vector<shared_ptr<Expression>> &args, InScope &parent_expr)
: Statement(parent_expr)
, action_(action)
, args_(args)
{}


Search::Search(unique_ptr<Block> &&block, InScope &parent_expr)
: Statement(parent_expr)
, block_(std::move(block))
{}


Test::Test(const shared_ptr<Expression> &expression, InScope &parent_expr)
: Statement(parent_expr)
, expression_(expression)
{}


While::While(const shared_ptr<Expression> &expression, unique_ptr<Block> &&block, InScope &parent_expr)
: Statement(parent_expr)
, expression_(expression), block_(std::move(block))
{}


Procedure::Procedure(const string &name, const vector<string> &arg_names, unique_ptr<Block> &&block)
: Statement(global_scope())
, NameWithArity(name, static_cast<arity_t>(arg_names.size()))
, Scope(arg_names, global_scope())
, block_(std::move(block))
{}



}
