#ifndef GOLOGPP_SCOPE_H_
#define GOLOGPP_SCOPE_H_

#include <vector>
#include <unordered_map>
#include <boost/variant.hpp>
#include <boost/variant/polymorphic_get.hpp>

#include "utilities.h"
#include "language.h"
#include "expressions.h"

#include "gologpp.h"

namespace gologpp {


Scope &global_scope();


class Scope : public LanguageElement<Scope> {
private:
	template<class K, class V>
	using unordered_map = std::unordered_map<K, V>;

public:
	using VariablesMap = unordered_map<string, shared_ptr<AbstractVariable>>;
	using GlobalsMap = unordered_map<Identifier, shared_ptr<Global>>;

	Scope(Expression *owner, const vector<shared_ptr<AbstractVariable>> &variables = {}, Scope &parent_scope = global_scope());

	Scope(Scope &&);

	Scope(const Scope &) = delete;
	Scope &operator = (const Scope &) = delete;

	template<class ExpressionT>
	shared_ptr<Variable<ExpressionT>> variable(const string &name)
	{
		auto it = variables_.find(name);
		shared_ptr<Variable<ExpressionT>> rv;
		if (it != variables_.end())
			rv = std::dynamic_pointer_cast<Variable<ExpressionT>>(it->second);
		else {
			rv.reset(new Variable<ExpressionT>(name, *this));
			variables_.emplace(name, rv);
		}
		return rv;
	}

	shared_ptr<AbstractVariable> variable(const string &name) const;

	vector<shared_ptr<AbstractVariable>> variables(const vector<string> &names) const;
	shared_ptr<Scope> parent_scope();

	void implement(Implementor &implementor);

	static Scope &global_scope()
	{ return global_scope_; }

	void set_owner(Expression *owner);
	const Expression *owner() const;

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
	Scope();

	static Scope global_scope_;
	Scope &parent_scope_;
	Expression *owner_;
	VariablesMap variables_;

	shared_ptr<GlobalsMap> globals_;
};


} // namespace gologpp







#endif
