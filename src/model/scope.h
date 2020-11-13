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

#ifndef GOLOGPP_SCOPE_H_
#define GOLOGPP_SCOPE_H_

#include <vector>
#include <unordered_map>
#include <boost/optional.hpp>
#include <cassert>
#include <list>

#include "utilities.h"
#include "language.h"
#include "expressions.h"
#include "error.h"

#include "gologpp.h"

namespace gologpp {


Scope &global_scope();



/**
 * @brief The NoScopeOwner class: A language element that does not open a new scope,
 * i.e. its scope and the scope of its children is its parent scope.
 */
class NoScopeOwner : public virtual AbstractLanguageElement {
public:
	virtual const Scope &scope() const override;
	virtual Scope &scope() override;
};



/**
 * @brief The ScopeOwner class: A language element that opens a new scope.
 * I.e. its scope is the parent scope of its children, which different from its parent scope.
 */
class ScopeOwner : public virtual AbstractLanguageElement {
public:
	ScopeOwner(Scope *owned_scope);

	virtual ~ScopeOwner() override = default;

	virtual const Scope &scope() const override;
	virtual Scope &scope() override;

protected:
	unique_ptr<Scope> scope_;
};



/**
 * @brief The Scope class: A container and definition space for language elements.
 * The global (root) scope is its own parent.
 */
class Scope : public ModelElement {
private:
	Scope();

	template<class K, class V>
	using unordered_map = std::unordered_map<K, V>;

	void init_types();

public:
	using IdentifierMap = unordered_map<string, shared_ptr<Identifier>>;
	using GlobalsMap = unordered_map<Name, shared_ptr<Global>>;
	using DomainsMap = unordered_map<Name, shared_ptr<Domain>>;
	using TypesMap = unordered_map<Name, shared_ptr<Type>>;

	using SignifierT = ModelElement;

	explicit Scope(AbstractLanguageElement &owner, const vector<shared_ptr<Variable>> &lookup_vars = {});
	Scope(Scope &parent_scope);

	~Scope() override;

	shared_ptr<Variable> get_var(VarDefinitionMode var_def_mode, const Type &type, const string &name);
	shared_ptr<Variable> lookup_var(const string &name) const;
	bool has_var(const string &name) const;
	vector<shared_ptr<Variable>> lookup_vars(const vector<string> &names);
	vector<shared_ptr<Variable>> vars() const;

	void attach_semantics(SemanticsFactory &implementor) override;

	static Scope &global_scope()
	{ return global_scope_; }

	Scope &parent_scope();
	const Scope &parent_scope() const;

	void set_owner(AbstractLanguageElement *owner);
	const AbstractLanguageElement *owner() const;
	AbstractLanguageElement *owner();

	template<class GologT = Global>
	shared_ptr<GologT> lookup_global(const string &name) const
	{
		if (exists_global(name))
			return std::dynamic_pointer_cast<GologT>(
				globals_->at(name)
			);
		else
			return shared_ptr<GologT>();
	}


	template<class GologT = Identifier>
	shared_ptr<GologT> lookup_identifier(const string &name) const
	{
		auto it = identifiers_.find(name);
		shared_ptr<GologT> rv;
		if (it != identifiers_.end())
			rv = std::dynamic_pointer_cast<GologT>(it->second);
		else if (&parent_scope() != this) {
			// This is not the root scope, so search upwards recursively
			rv = parent_scope().lookup_identifier<GologT>(name);
		}
		return rv;
	};


	template<class GologT = Identifier>
	shared_ptr<GologT> lookup_local_identifier(const string &name) const
	{
		auto it = identifiers_.find(name);
		if (it != identifiers_.end())
			return std::dynamic_pointer_cast<GologT>(it->second);
		else
			return nullptr;
	};


	template<class GologT = Identifier>
	void register_identifier(GologT *i)
	{
		if (lookup_identifier<GologT>(i->name()))
			throw RedefinitionError("Identifier \"" + i->name() + "\" already defined");

		identifiers_[i->name()].reset(i);
	}


	template<class GologT = Identifier>
	vector<shared_ptr<GologT>> local_identifiers() const
	{
		vector<shared_ptr<GologT>> rv;
		for (auto &entry : identifier_map()) {
			shared_ptr<GologT> s = std::dynamic_pointer_cast<GologT>(entry.second);
			if (s)
				rv.push_back(s);
		}
		return rv;
	}


	const IdentifierMap &identifier_map() const;
	void implement_globals(SemanticsFactory &implementor, AExecutionController &ctx);
	void clear();

	bool exists_global(const string &name) const;

	TypeList param_types(const string &global_name);

	template<class GologT>
	GologT *declare_global(
		Scope *own_scope,
		const Type &type,
		const string &name,
		const boost::optional<vector<shared_ptr<Variable>>> &args
	) {
		GologT *rv = nullptr;
		if (exists_global(name)) {
			rv = lookup_global<GologT>(name).get();

			if (!rv) {
				shared_ptr<Global> tmp = lookup_global(name);
				throw RedefinitionError(*tmp, name, typeid(GologT));
			}

			if (rv && !(rv->type() <= type))
				throw TypeError("Cannot redeclare " + rv->str()
					+ " with type " + type.name());
		}
		else
			register_global(new GologT(own_scope, type, name, args.get_value_or({})));

		return lookup_global<GologT>(name).get();
	}


	template<class GologT, class... DefinitionTs>
	GologT *define_global(
		Scope *own_scope,
		const Type &type,
		const string &name,
		const boost::optional<vector<shared_ptr<Variable>>> &args,
		DefinitionTs... definition_args
	) {
		GologT *rv = nullptr;
		if (exists_global(name)) {
			rv = lookup_global<GologT>(name).get();

			if (!rv) {
				shared_ptr<Global> tmp = lookup_global(name);
				throw RedefinitionError(*tmp, name, typeid(GologT));
			}

			if (!(rv->type() <= type))
				throw TypeError("Cannot redefine " + rv->str()
					+ " with type " + type.name());

			// TODO: if (rv->defined()): Warn on redefinition
		}
		else
			rv = declare_global<GologT>(own_scope, type, name, args);

		rv->define(definition_args...);

		return rv;
	}



	bool exists_type(const string &name) const;

	template<class TypeT = Type>
	shared_ptr<const TypeT> lookup_type(const string &name) const
	{
		if (exists_type(name))
			return std::dynamic_pointer_cast<const TypeT>(
				types_->find(name)->second
			);
		else
			return nullptr;
	}

	shared_ptr<const ListType> lookup_list_type(const Type &elem_type) const;

	const Type *lookup_type_raw(const string &name);

	void register_type_raw(Type *t);
	void register_type(shared_ptr<Type> t);
	void register_global(Global *g);
	void register_constraint(platform::Constraint *c);

	bool exists_domain(const string &name) const;
	shared_ptr<Domain> lookup_domain(const string &name, const Type &t);

	void register_domain_raw(Domain *d);
	void register_domain(const shared_ptr<Domain> &d);
	void declare_domain(const string &name, const Type &type);
	void define_domain(const string &name, const Type &type, const Domain &input);

	Value *get_symbol(const string &name);

	virtual string to_string(const string &pfx) const override;

	vector<shared_ptr<Global>> globals() const;

	vector<unique_ptr<platform::Constraint>> &constraints();

	template<class = Scope>
	Semantics<Scope> &semantics() const
	{ return ModelElement::semantics<Scope>(); }

	Scope *get_scope(const string &identifier);


private:
	static Scope global_scope_;
	Scope &parent_scope_;
	AbstractLanguageElement *owner_;
	IdentifierMap identifiers_;

	shared_ptr<GlobalsMap> globals_;
	shared_ptr<DomainsMap> domains_;
	shared_ptr<TypesMap> types_;
	shared_ptr<vector<unique_ptr<platform::Constraint>>> constraints_;
};



template<class T>
const T &type()
{
	if (!global_scope().exists_type(T::static_name()))
		global_scope().register_type_raw(new T());
	return *global_scope().lookup_type<T>(T::static_name());
}


template<class T>
const T &get_type()
{ return type<T>(); }


} // namespace gologpp







#endif
