#ifndef GOLOGPP_SCOPE_H_
#define GOLOGPP_SCOPE_H_

#include <vector>
#include <unordered_map>
#include <boost/variant.hpp>
#include <boost/variant/polymorphic_get.hpp>
#include <cassert>

#include "utilities.h"
#include "language.h"
#include "expressions.h"

#include "gologpp.h"

namespace gologpp {


Scope &global_scope();


class Scope : public LanguageElement<Scope> {
private:
	Scope();

	template<class K, class V>
	using unordered_map = std::unordered_map<K, V>;

public:
	using VariablesMap = unordered_map<string, shared_ptr<AbstractVariable>>;
	using GlobalsMap = unordered_map<Identifier, shared_ptr<Global>>;

	Scope(ScopeOwner *owner, const vector<shared_ptr<AbstractVariable>> &lookup_vars = {}, Scope &parent_scope = global_scope());
	Scope(ScopeOwner *owner, Scope &parent_scope);

	Scope(Scope &&);

	Scope(const Scope &) = delete;
	Scope &operator = (const Scope &) = delete;

	~Scope();

	template<class ExpressionT>
	shared_ptr<Variable<ExpressionT>> get_var(const string &name)
	{
		shared_ptr<Variable<ExpressionT>> rv;
		rv = std::dynamic_pointer_cast<Variable<ExpressionT>>(lookup_var(name));
		if (!rv)
			rv.reset(new Variable<ExpressionT>(name, *this));
		if (!has_var(name))
			variables_.emplace(name, rv);
		AbstractVariable *in_map = variables_.find(name)->second.get();
		assert(in_map == rv.get());
		return rv;
	}

	shared_ptr<AbstractVariable> lookup_var(const string &name) const;
	bool has_var(const string &name) const;

	vector<shared_ptr<AbstractVariable>> lookup_vars(const vector<string> &names);

	void implement(Implementor &implementor);

	static Scope &global_scope()
	{ return global_scope_; }

	Scope &parent_scope();
	const Scope &parent_scope() const;

	void set_owner(ScopeOwner *owner);
	const ScopeOwner *owner() const;

	template<class GologT = Global>
	shared_ptr<GologT> lookup_global(const Identifier &id)
	{ return std::dynamic_pointer_cast<GologT>((*globals_)[id]); }

	void register_global(Global *g);
	const VariablesMap &var_map() const;
	void implement_globals(Implementor &implementor, AExecutionContext &ctx);
	void clear();


	template<class GologT>
	GologT *declare_global(Scope *own_scope, const string &name, const vector<shared_ptr<AbstractVariable>> &args)
	{
		Identifier id { name, static_cast<arity_t>(args.size()) };
		shared_ptr<GologT> obj = lookup_global<GologT>(id);
		if (!obj) {
			obj.reset(new GologT(own_scope, name, args));
			(*globals_)[id] = obj;
		}
		// else
		// TODO: Warn on overwrite?

		return obj.get();
	}


	template<class GologT, class... DefinitionTs>
	GologT *define_global(
		Scope *own_scope, const string &name,
		const vector<shared_ptr<AbstractVariable>> &args,
		DefinitionTs... definition_args
	)
	{
		GologT *obj = declare_global<GologT>(own_scope, name, args);
		obj->define(definition_args...);
		return obj;
	}


private:
	static Scope global_scope_;
	Scope &parent_scope_;
	ScopeOwner *owner_;
	VariablesMap variables_;

	shared_ptr<GlobalsMap> globals_;
};



class ScopeOwner {
public:
	ScopeOwner(Scope *owned_scope);

	virtual ~ScopeOwner() = default;

	const Scope &scope() const;
	Scope &scope();
	const Scope &parent_scope() const;
	Scope &parent_scope();

protected:
	unique_ptr<Scope> scope_;
};



} // namespace gologpp







#endif
