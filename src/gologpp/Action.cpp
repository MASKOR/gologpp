#include "Action.h"
#include "EffectAxiom.h"
#include <string>

namespace gologpp {

using namespace std;


Action::Action(const string &name, const vector<string> &args,
	       unique_ptr<BooleanExpression> &&precondition, unique_ptr<AbstractEffectAxiom> &&effect)
: Statement(Scope::global_scope())
, Identifier(name, static_cast<arity_t>(args.size()))
, scope_(this, {}, Scope::global_scope())
, precondition_(std::move(precondition))
, effect_(std::move(effect))
, args_(args)
{}


const BooleanExpression &Action::precondition() const
{ return *precondition_; }


const AbstractEffectAxiom &Action::effect() const
{ return *effect_; }


const vector<string> &Action::arg_names() const
{ return args_; }


shared_ptr<AbstractVariable> Action::argument(arity_t idx) const
{ return scope_.variable(args_[static_cast<size_t>(idx)]); }


Scope &Action::scope()
{ return scope_; }

const Scope &Action::scope() const
{ return scope_; }

void Action::implement(Implementor &implementor)
{
	impl_ = implementor.make_impl(*this);
	scope_.implement(implementor);
	precondition_->implement(implementor);
	effect_->implement(implementor);
}

} // namespace gologpp
