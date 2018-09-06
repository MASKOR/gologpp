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
, globals_(new GlobalsMap())
{}


Scope::Scope(ScopeOwner *owner, const vector<shared_ptr<AbstractVariable>> &variables, Scope &parent_scope)
: parent_scope_(parent_scope)
, owner_(owner)
, globals_(parent_scope.globals_)
{
	for (const shared_ptr<AbstractVariable> &v : variables)
		variables_.emplace(v->name(), v);
}

Scope::Scope(ScopeOwner *owner, Scope &parent_scope)
: Scope(owner, {}, parent_scope)
{}

Scope::Scope(Scope &&other)
: parent_scope_(other.parent_scope_)
, variables_(std::move(other.variables_))
, globals_(std::move(other.globals_))
{}

Scope::~Scope()
{}

bool Scope::has_var(const string &name) const
{ return variables_.find(name) != variables_.end(); }


shared_ptr<AbstractVariable> Scope::lookup_var(const string &name) const
{
	auto it = variables_.find(name);
	shared_ptr<AbstractVariable> rv;
	if (it != variables_.end())
		rv = it->second;
	else if (&parent_scope_ != this) {
		// This is not the root scope, so search upwards recursively
		rv = parent_scope_.lookup_var(name);
	}
	return rv;
}


vector<shared_ptr<AbstractVariable>> Scope::lookup_vars(const vector<string> &names)
{
	vector<shared_ptr<AbstractVariable>> rv;
	for (const string &name : names)
		rv.push_back(lookup_var(name));
	return rv;
}


vector<shared_ptr<AbstractVariable>> Scope::vars() const
{
	vector<shared_ptr<AbstractVariable>> rv;
	for (auto &entry : variables_)
		rv.push_back(entry.second);
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

Scope &Scope::parent_scope()
{ return parent_scope_; }

const Scope &Scope::parent_scope() const
{ return parent_scope_; }

void Scope::set_owner(ScopeOwner *owner)
{ owner_ = owner; }

const ScopeOwner *Scope::owner() const
{ return owner_; }

const Scope::VariablesMap &Scope::var_map() const
{ return variables_; }


Expression *Scope::ref_to_global(
	const string &name,
	const boost::optional<vector<Expression *>> &args
) {
	return (*globals_)[ { name, static_cast<arity_t>(args.get_value_or({}).size()) } ]
		->ref(*this, args.get_value_or({}));
}


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


void Scope::clear()
{
	variables_.clear();
	if (this == &global_scope())
		globals_->clear();
}

Scope &global_scope()
{ return Scope::global_scope(); }

bool Scope::exists_global(const string &name, arity_t arity) const
{ return globals_->find( { name, arity } ) != globals_->end(); }

string Scope::to_string(const string &) const
{ return "[" + concat_list(vars(), ", ") + "]"; }




ScopeOwner::ScopeOwner(Scope *owned_scope)
: scope_(owned_scope)
{ scope_->set_owner(this); }

const Scope &ScopeOwner::scope() const
{ return *scope_; }

Scope &ScopeOwner::scope()
{ return *scope_; }

const Scope &ScopeOwner::parent_scope() const
{ return scope().parent_scope(); }

Scope &ScopeOwner::parent_scope()
{ return scope().parent_scope(); }



} // namespace gologpp
