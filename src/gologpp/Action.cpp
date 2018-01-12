#include "Action.h"
#include <string>

namespace gologpp {

using namespace std;


Action::Action(const string &name, const vector<string> &args,
	       unique_ptr<Expression> &&precondition, unique_ptr<EffectAxiom> &&effect)
: NameWithArity(name, static_cast<arity_t>(args.size()))
, Scope(args, global_scope())
, Expression(global_scope())
, precondition_(std::move(precondition))
, effect_(std::move(effect))
{}


Action::Action(Action &&other)
: NameWithArity(std::move(other))
, Scope(std::move(other))
, Expression(std::move(other))
, precondition_(std::move(other.precondition_))
, effect_(std::move(other.effect_))
{}


const EffectAxiom &Action::effect() const
{ return *effect_; }

const Expression &Action::precondition() const
{ return *precondition_; }


Transition::Transition(const shared_ptr<Action> &action, vector<unique_ptr<Value>> &&binding)
: action_(action)
, binding_(std::move(binding))
{}


const Action &Transition::action() const
{ return *action_; }


} /* namespace gologpp */
