#include "Action.h"
#include <string>

namespace gologpp {
namespace generic {

using namespace std;


Action::Action(const string &name, const vector<string> &args,
	       unique_ptr<BooleanExpression> &&precondition, unique_ptr<EffectAxiom> &&effect)
: Identifier(name, static_cast<arity_t>(args.size()))
, scope_(args, Scope::global_scope())
, precondition_(std::move(precondition))
, effect_(std::move(effect))
, args_(args)
{}


Action::Action(Action &&other)
: Identifier(std::move(other))
, scope_(std::move(other.scope_))
, precondition_(std::move(other.precondition_))
, effect_(std::move(other.effect_))
, args_(std::move(other.args_))
{}


const BooleanExpression &Action::precondition() const
{ return *precondition_; }


const EffectAxiom &Action::effect() const
{ return *effect_; }

void Action::set_effect(unique_ptr<EffectAxiom> &&effect)
{ effect_ = std::move(effect); }


vector<shared_ptr<Expression>> Action::args() const
{ return scope_.variables(args_); }


Scope &Action::scope()
{ return scope_; }


Transition::Transition(const shared_ptr<Action> &action, vector<unique_ptr<AnyValue>> &&binding)
: action_(action)
, binding_(std::move(binding))
{}


const Action &Transition::action() const
{ return *action_; }


} // namespace generic
} // namespace gologpp
