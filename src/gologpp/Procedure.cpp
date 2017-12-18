#include <functional>
#include <cstdlib>

#include "Procedure.h"
#include "Formula.h"

#include "user_error.h"

namespace gologpp {


Statement::~Statement()
{}


Block::Block(const vector<shared_ptr<Statement>> &elements)
: elements_(elements)
{}


Choose::Choose(const vector<shared_ptr<Block>> &alternatives)
: alternatives_(alternatives)
{}


Conditional::Conditional(const shared_ptr<Expression> &condition,
                         const shared_ptr<Block> &block_true,
                         const shared_ptr<Block> &block_false)
    : condition_(condition),
      block_true_(block_true),
      block_false_(block_false)
{}


Assignment::Assignment(const shared_ptr<Fluent> &fluent, const shared_ptr<Expression> &expression)
: fluent_(fluent), expression_(expression)
{}


Pick::Pick(const shared_ptr<Variable> &variable, const shared_ptr<Block> &block)
: variable_(variable), block_(block)
{}


Call::Call(const shared_ptr<Action> &action, const vector<shared_ptr<Expression>> &args)
: action_(action)
, args_(args)
{}


Search::Search(unique_ptr<Block> &&block)
: block_(std::move(block))
{}


Test::Test(const shared_ptr<Expression> &expression)
: expression_(expression)
{}


While::While(const shared_ptr<Expression> &expression, unique_ptr<Block> &&block)
: expression_(expression), block_(std::move(block))
{}


Procedure::Procedure(const string &name, arity_t arity, unique_ptr<Block> &&block)
: NameWithArity<Procedure>(name, arity)
, block_(std::move(block))
{}



}
