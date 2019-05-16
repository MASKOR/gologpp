#include "scope.h"
#include "procedural.h"
#include "action.h"
#include "fluent.h"
#include "effect_axiom.h"
#include "domain.h"
#include "user_error.h"
#include "constant.h"
#include "execution.h"


namespace gologpp {

Scope Scope::global_scope_;


Expression *ref_to_global(
	const string &name,
	const boost::optional<vector<Expression *>> &args
) {
	return global_scope().lookup_global(
		name,
		arity_t(args ? args->size() : 0)
	)->ref(args.get_value_or({}));
}



Scope &NoScopeOwner::scope()
{ return parent_scope(); }

const Scope &NoScopeOwner::scope() const
{ return parent_scope(); }



Scope::Scope()
: parent_scope_(*this)
, owner_(nullptr)
, globals_(new GlobalsMap())
, domains_(new DomainsMap())
, types_(new TypesMap())
{
#define GOLOGPP_REGISTER_SIMPLE_TYPE(_r, _data, T) \
	register_type(new T());

	BOOST_PP_SEQ_FOR_EACH(GOLOGPP_REGISTER_SIMPLE_TYPE, (), GOLOGPP_SIMPLE_TYPES)
}


Scope::Scope(AbstractLanguageElement &owner, const vector<shared_ptr<Variable>> &variables)
: parent_scope_(owner.scope())
, owner_(&owner)
, globals_(owner.scope().globals_)
, domains_(owner.scope().domains_)
, types_(owner.scope().types_)
{
	for (const shared_ptr<Variable> &v : variables)
		variables_.emplace(v->name(), v);
}


Scope::Scope(Scope &parent_scope)
: parent_scope_(parent_scope)
, owner_(nullptr)
, globals_(parent_scope.globals_)
, domains_(parent_scope.domains_)
, types_(parent_scope.types_)
{}

Scope::~Scope()
{}

bool Scope::has_var(const string &name) const
{ return variables_.find(name) != variables_.end(); }


shared_ptr<Variable> Scope::get_var(VarDefinitionMode var_def_mode, const string &type_name, const string &name)
{
	shared_ptr<Variable> rv;

	auto it = variables_.find(name);
	if (it != variables_.end())
		rv = std::dynamic_pointer_cast<Variable>(it->second);

	if (!rv && var_def_mode != VarDefinitionMode::FORCE && &parent_scope() != this)
		rv = std::dynamic_pointer_cast<Variable>(
			parent_scope().get_var(VarDefinitionMode::DENY, type_name, name)
		);

	if (!rv && var_def_mode != VarDefinitionMode::DENY) {
		rv.reset(new Variable(type_name, name));
		variables_[name] = rv;
	}

	return rv;
}


shared_ptr<Variable> Scope::lookup_var(const string &name) const
{
	auto it = variables_.find(name);
	shared_ptr<Variable> rv;
	if (it != variables_.end())
		rv = it->second;
	else if (&parent_scope() != this) {
		// This is not the root scope, so search upwards recursively
		rv = parent_scope().lookup_var(name);
	}
	return rv;
}


vector<shared_ptr<Variable>> Scope::lookup_vars(const vector<string> &names)
{
	vector<shared_ptr<Variable>> rv;
	for (const string &name : names)
		rv.push_back(lookup_var(name));
	return rv;
}


shared_ptr<Domain> Scope::lookup_domain(const string &name)
{
	if (exists_domain(name))
		return std::dynamic_pointer_cast<Domain>(
			domains_->find(name)->second
		);
	else
		return shared_ptr<Domain>();
}


vector<shared_ptr<Variable>> Scope::vars() const
{
	vector<shared_ptr<Variable>> rv;
	for (auto &entry : variables_)
		rv.push_back(entry.second);
	return rv;
}


void Scope::attach_semantics(SemanticsFactory &implementor)
{
	if (!semantics_) {
		semantics_ = implementor.make_semantics(*this);
		for (auto &entry : variables_)
			entry.second->attach_semantics(implementor);
	}
}

Scope &Scope::scope()
{ return *this; }

const Scope &Scope::scope() const
{ return *this; }

Scope &Scope::parent_scope()
{ return parent_scope_; }

const Scope &Scope::parent_scope() const
{ return parent_scope_; }

void Scope::set_owner(AbstractLanguageElement *owner)
{
	if (owner) {
		owner_ = owner;
		globals_ = owner->scope().globals_;
	}
}

const AbstractLanguageElement *Scope::owner() const
{ return owner_; }

AbstractLanguageElement *Scope::owner()
{ return owner_; }

const Scope::VariablesMap &Scope::var_map() const
{ return variables_; }


void Scope::implement_globals(SemanticsFactory &implementor, AExecutionContext &ctx)
{
	ctx.precompile();

	// Two loops since we want everything implemented before we attempt to compile anything.
	// It's all connected, you know...
	for (GlobalsMap::value_type &entry : *globals_)
		std::dynamic_pointer_cast<AbstractLanguageElement>(entry.second)
			->attach_semantics(implementor);
	for (GlobalsMap::value_type &entry : *globals_)
		entry.second->compile(ctx);

	ctx.postcompile();
}


void Scope::clear()
{
	variables_.clear();
	if (this == &global_scope()) {
		globals_->clear();
		domains_->clear();
	}
}

Scope &global_scope()
{ return Scope::global_scope(); }

bool Scope::exists_global(const string &name, arity_t arity) const
{ return globals_->find( { name, arity } ) != globals_->end(); }

bool Scope::exists_domain(const string &name) const
{ return domains_->find(name) != domains_->end(); }

bool Scope::exists_type(const string &name) const
{ return types_->find(name) != types_->end(); }

string Scope::to_string(const string &) const
{ return "[" + concat_list(vars(), ", ") + "]"; }


void Scope::register_global(Global *g)
{
	if (exists_global(g->name(), g->arity()))
		throw RedefinitionError(g->name(), g->arity());
	(*globals_)[*g].reset(g);
}


void Scope::register_domain(AbstractDomain *d)
{
	if (exists_domain(d->name()))
		throw RedefinitionError(d->name());
	(*domains_)[*d].reset(d);
}

void Scope::register_domain(const shared_ptr<Domain> &d)
{ domains_->emplace(*d, d); }

void Scope::declare_domain(const string &name)
{ domains_->emplace(name, std::make_shared<Domain>(name)); }


void Scope::register_type(Type *t)
{
	if (exists_type(t->name()))
		throw RedefinitionError(t->name());
	(*types_)[*t].reset(t);
}

Constant *Scope::get_symbol(const string &name)
{
	for (const DomainsMap::value_type &entry : *domains_) {
		try {
			const Domain &domain = dynamic_cast<const Domain &>(*entry.second);
			if (domain.elements().find(unique_ptr<Constant>(new Constant(Symbol::static_name(), name))) != domain.elements().end())
				return new Constant(Symbol::static_name(), name);
		} catch (std::bad_cast &)
		{}
	}
	return nullptr;
}

void Scope::define_domain(const string &name, const Domain &input)
{
	shared_ptr<Domain> d = lookup_domain(name);
	if (d)
		d->define(input);
	else
		register_domain(std::make_shared<Domain>(name, input));
}


ScopeOwner::ScopeOwner(Scope *owned_scope)
: scope_(owned_scope)
{ scope_->set_owner(this); }

const Scope &ScopeOwner::scope() const
{ return *scope_; }

Scope &ScopeOwner::scope()
{ return *scope_; }



} // namespace gologpp
