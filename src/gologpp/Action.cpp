#include "Action.h"
#include <string>

namespace gologpp {

using namespace std;


Action::Action(const string &name, arity_t arity, unique_ptr<PrecondAxiom> &&precondition, unique_ptr<EffectAxiom> &&effect)
: NameWithArity<gologpp::Action>(name, arity)
, precondition_(std::move(precondition))
, effect_(std::move(effect))
{
}


const EffectAxiom &Action::effect() const
{ return *effect_; }

const PrecondAxiom &Action::precondition() const
{ return *precondition_; }


Transition::Transition(const shared_ptr<Action> &action, vector<unique_ptr<Value>> &&binding)
: action_(action)
, binding_(std::move(binding))
{}


const Action &Transition::action() const
{ return *action_; }


} /* namespace gologpp */
