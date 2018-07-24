#include "scope.h"
#include "atoms.h"
#include "procedural.h"
#include "action.h"
#include "fluent.h"
#include "effect_axiom.h"

namespace gologpp {

Scope Scope::global_scope_;


Scope::Scope()
: parent_scope_(*this)
, globals_(new unordered_map<Identifier, shared_ptr<Global>>())
{}


Scope::Scope(AbstractLanguageElement *owner, const vector<shared_ptr<AbstractVariable>> &variables, Scope &parent_scope)
: parent_scope_(parent_scope)
, owner_(owner)
, globals_(parent_scope.globals_)
{
	for (const shared_ptr<AbstractVariable> &v : variables)
		variables_.emplace(v->name(), v);
}


Scope::Scope(AbstractLanguageElement *owner, Scope &parent_scope)
: Scope(owner, {}, parent_scope)
{}


Scope::Scope(Scope &&other)
: parent_scope_(other.parent_scope_)
, variables_(std::move(other.variables_))
, globals_(std::move(other.globals_))
{}


shared_ptr<AbstractVariable> Scope::variable(const string &name) const
{
	auto it = variables_.find(name);
	shared_ptr<AbstractVariable> rv;
	if (it != variables_.end())
		rv = it->second;
	return rv;
}


vector<shared_ptr<AbstractVariable>> Scope::variables(const vector<string> &names) const
{
	vector<shared_ptr<AbstractVariable>> rv;
	for (const string &name : names)
		rv.push_back(variable(name));
	return rv;
}


void Scope::implement(Implementor &implementor)
{
	if (!impl_) {
		impl_ = implementor.make_impl(*this);
		for (auto &entry : variables_)
			entry.second->implement(implementor);
	}
}


void Scope::set_owner(AbstractLanguageElement *owner)
{ owner_ = owner; }

const AbstractLanguageElement *Scope::owner() const
{ return owner_; }

const Scope::VariablesMap &Scope::var_map() const
{ return variables_; }


void Scope::implement_globals(Implementor &implementor, AExecutionContext &ctx)
{
	// Two loops since we want everything implemented before we attempt to compile anything.
	// It's all connected, you know...
	for (GlobalsMap::value_type &entry : *globals_)
		std::dynamic_pointer_cast<AbstractLanguageElement>(entry.second)
			->implement(implementor);
	for (GlobalsMap::value_type &entry : *globals_)
		entry.second->compile(ctx);
}



void Scope::register_global(Global *g)
{ (*globals_)[static_cast<Identifier>(*g)] = shared_ptr<Global>(g); }


void Scope::clear()
{
	variables_.clear();
	if (this == &global_scope())
		globals_->clear();
}

Scope &global_scope()
{ return Scope::global_scope(); }


} // namespace gologpp
