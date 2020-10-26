/*************************************************************************
 * This file is part of golog++.
 *
 * golog++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * golog++ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with golog++.  If not, see <https://www.gnu.org/licenses/>.
**************************************************************************/

#include "scope.h"
#include "procedural.h"
#include "action.h"
#include "fluent.h"
#include "effect_axiom.h"
#include "domain.h"
#include "error.h"
#include "value.h"
#include "platform/constraint.h"

#include <execution/controller.h>


namespace gologpp {

Scope Scope::global_scope_;


Scope &NoScopeOwner::scope()
{ return parent_scope(); }

const Scope &NoScopeOwner::scope() const
{ return parent_scope(); }


#define GOLOGPP_REGISTER_SIMPLE_TYPE(_r, _data, T) \
	register_type_raw(new T());

void Scope::init_types()
{
	types_->clear();
	BOOST_PP_SEQ_FOR_EACH(GOLOGPP_REGISTER_SIMPLE_TYPE, (), GOLOGPP_PREDEFINED_TYPES)
}


Scope::Scope()
: parent_scope_(*this)
, owner_(nullptr)
, globals_(new GlobalsMap())
, domains_(new DomainsMap())
, types_(new TypesMap())
, constraints_(new vector<unique_ptr<platform::Constraint>>())
{
	init_types();
}


Scope::Scope(AbstractLanguageElement &owner, const vector<shared_ptr<Variable>> &variables)
: parent_scope_(owner.scope())
, owner_(&owner)
, globals_(owner.scope().globals_)
, domains_(owner.scope().domains_)
, types_(owner.scope().types_)
, constraints_(owner.scope().constraints_)
{
	for (const shared_ptr<Variable> &v : variables)
		identifiers_.emplace(v->name(), v);
}


Scope::Scope(Scope &parent_scope)
: parent_scope_(parent_scope)
, owner_(nullptr)
, globals_(parent_scope.globals_)
, domains_(parent_scope.domains_)
, types_(parent_scope.types_)
, constraints_(parent_scope.constraints_)
{}

Scope::~Scope()
{}

bool Scope::has_var(const string &name) const
{ return lookup_identifier<Variable>(name).get(); }


shared_ptr<Variable> Scope::get_var(VarDefinitionMode var_def_mode, const Type &type, const string &name)
{
	shared_ptr<Variable> rv = lookup_identifier<Variable>(name);

	if (rv && !(rv->type() <= type))
		return nullptr;

	if (!rv && var_def_mode != VarDefinitionMode::FORCE && &parent_scope() != this)
		rv = std::dynamic_pointer_cast<Variable>(
			parent_scope().get_var(VarDefinitionMode::DENY, type, name)
		);

	if (!rv && var_def_mode != VarDefinitionMode::DENY) {
		rv.reset(new Variable(type, name));
		identifiers_[name] = rv;
	}

	return rv;
}


shared_ptr<Variable> Scope::lookup_var(const string &name) const
{ return lookup_identifier<Variable>(name); }


vector<shared_ptr<Variable>> Scope::lookup_vars(const vector<string> &names)
{
	vector<shared_ptr<Variable>> rv;
	for (const string &name : names)
		rv.push_back(lookup_var(name));
	return rv;
}


shared_ptr<Domain> Scope::lookup_domain(const string &name, const Type &type)
{
	auto it = domains_->find(name);
	if (it != domains_->end() && type <= it->second->element_type())
		return it->second;
	else
		return nullptr;
}


vector<shared_ptr<Variable>> Scope::vars() const
{ return local_identifiers<Variable>(); }


void Scope::attach_semantics(SemanticsFactory &implementor)
{
	if (!semantics_) {
		semantics_ = implementor.make_semantics(*this);
		for (auto &entry : identifiers_)
			entry.second->attach_semantics(implementor);
	}
}

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

const Scope::IdentifierMap &Scope::identifier_map() const
{ return identifiers_; }


void Scope::implement_globals(SemanticsFactory &implementor, AExecutionController &ctx)
{
	ctx.precompile();

	// Two loops since we want everything implemented before we attempt to compile anything.
	// It's all connected, you know...
	for (GlobalsMap::value_type &entry : *globals_)
		entry.second->attach_semantics(implementor);
	for (GlobalsMap::value_type &entry : *globals_)
		entry.second->compile(ctx);

	for (TypesMap::value_type &entry : *types_) {
		// TODO: Only Domains have semantics currently. Should be extended to all types.
		shared_ptr<Domain> d { std::dynamic_pointer_cast<Domain>(entry.second) };
		if (d)
			d->attach_semantics(implementor);

		// Also no compilation for types so far because it wasn't needed.
	}

	for (unique_ptr<platform::Constraint> &c : constraints())
		c->attach_semantics(implementor);


	ctx.postcompile();
}


void Scope::clear()
{
	identifiers_.clear();
	if (this == &global_scope()) {
		globals_->clear();
		domains_->clear();
		init_types();
	}
}

Scope &global_scope()
{ return Scope::global_scope(); }

bool Scope::exists_global(const string &name) const
{ return globals_->find(name) != globals_->end(); }


TypeList Scope::param_types(const string &global_name)
{
	TypeList rv;
	shared_ptr<Global> g = lookup_global<Global>(global_name);
	if (!g)
		throw InvalidIdentifier(global_name);
	for (auto &param : g->params())
		rv.emplace_back(param->type());
	return rv;
}


bool Scope::exists_domain(const string &name) const
{ return domains_->find(name) != domains_->end(); }

bool Scope::exists_type(const string &name) const
{ return types_->find(name) != types_->end(); }

const Type *Scope::lookup_type_raw(const string &name)
{ return lookup_type(name).get(); }

shared_ptr<const ListType> Scope::lookup_list_type(const Type &elem_type) const
{ return lookup_type<ListType>("list[" + elem_type.name() + "]"); }

string Scope::to_string(const string &) const
{ return "[" + concat_list(vars(), ", ") + "]"; }


void Scope::register_global(Global *g)
{
	if (exists_global(g->name()))
		throw RedefinitionError(g->name(), g->arity());
	(*globals_)[*g].reset(g);
	global_scope().identifiers_[*g] = (*globals_)[*g];
}

void Scope::register_constraint(platform::Constraint *c)
{ constraints_->emplace_back(c); }


void Scope::register_domain_raw(Domain *d)
{ register_domain(shared_ptr<Domain>(d)); }

void Scope::register_domain(const shared_ptr<Domain> &d)
{
	if (exists_domain(d->name()))
		throw RedefinitionError(d->name());
	(*domains_)[d->name()] = d;

	register_type(d);
}

void Scope::declare_domain(const string &name, const Type &elem_type)
{
	shared_ptr<Domain> d = lookup_domain(name, elem_type);
	if (!d)
		register_domain(std::make_shared<Domain>(name, elem_type));
	else if (!(*d <= elem_type))
		throw TypeError("Conflicting declaration of domain " + name + ": "
			+ d->type().name() + " vs " + elem_type.name()
		);
}


void Scope::register_type_raw(Type *t)
{ register_type(shared_ptr<Type>(t)); }


void Scope::register_type(shared_ptr<Type> t)
{
	if (exists_type(t->name()))
		throw RedefinitionError(t->name());
	(*types_)[t->name()] = t;
}


Value *Scope::get_symbol(const string &name)
{
	for (const DomainsMap::value_type &entry : *domains_) {
		try {
			const Domain &domain = dynamic_cast<const Domain &>(*entry.second);
			if (
				domain.elements().find(
					unique_ptr<Value>(new Value(gologpp::type<SymbolType>(), name))
				) != domain.elements().end()
			)
				return new Value(gologpp::type<SymbolType>(), name);

		} catch (std::bad_cast &)
		{}
	}
	return nullptr;
}


void Scope::define_domain(const string &name, const Type &t, const Domain &input)
{
	shared_ptr<Domain> d = lookup_domain(name, t);
	if (d) {
		d->set_type(t);
		d->define(input);
	}
	else
		register_domain(std::make_shared<Domain>(name, input));
}


vector<shared_ptr<Global>> Scope::globals() const
{
	vector<shared_ptr<Global>> rv;
	for (const GlobalsMap::value_type &entry : *globals_)
		rv.push_back(entry.second);
	return rv;
}

vector<unique_ptr<platform::Constraint> > &Scope::constraints()
{ return *constraints_; }



ScopeOwner::ScopeOwner(Scope *owned_scope)
: scope_(owned_scope)
{ scope_->set_owner(this); }

const Scope &ScopeOwner::scope() const
{ return *scope_; }

Scope &ScopeOwner::scope()
{ return *scope_; }



} // namespace gologpp
