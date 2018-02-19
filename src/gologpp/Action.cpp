#include "Action.h"
#include <string>

namespace gologpp {

using namespace std;


Action::Action(const string &name, const vector<string> &args,
	       unique_ptr<BooleanExpression> &&precondition, unique_ptr<EffectAxiom> &&effect)
: Identifier(name, static_cast<arity_t>(args.size()))
, scope_(args, Scope::global_scope())
, precondition_(std::move(precondition))
, effect_(std::move(effect))
, args_(args)
{}


const BooleanExpression &Action::precondition() const
{ return *precondition_; }


const EffectAxiom &Action::effect() const
{ return *effect_; }

void Action::set_effect(EffectAxiom &&effect)
{ effect_.reset(new EffectAxiom(std::move(effect))); }


const vector<string> &Action::args() const
{ return args_; }


Scope &Action::scope()
{ return scope_; }

const Scope &Action::scope() const
{ return scope_; }


Transition::Transition(const shared_ptr<Action> &action, vector<unique_ptr<AnyValue>> &&binding)
: action_(action)
, binding_(std::move(binding))
{}


const Action &Transition::action() const
{ return *action_; }


} // namespace gologpp
