#include "Action.h"
#include "EffectAxiom.h"
#include <string>

namespace gologpp {

using namespace std;


Action::Action(const string &name, const vector<string> &args,
	       unique_ptr<BooleanExpression> &&precondition, unique_ptr<AbstractEffectAxiom> &&effect)
: Identifier(name, static_cast<arity_t>(args.size()))
, scope_(args, Scope::global_scope())
, precondition_(std::move(precondition))
, effect_(std::move(effect))
, args_(args)
{}


const BooleanExpression &Action::precondition() const
{ return *precondition_; }


const AbstractEffectAxiom &Action::effect() const
{ return *effect_; }


const vector<string> &Action::args() const
{ return args_; }


Scope &Action::scope()
{ return scope_; }

const Scope &Action::scope() const
{ return scope_; }


} // namespace gologpp
