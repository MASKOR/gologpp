#ifndef GOLOGPP_SCOPE_H_
#define GOLOGPP_SCOPE_H_

#include <vector>
#include <unordered_map>
#include <boost/optional.hpp>
#include <cassert>

#include "utilities.h"
#include "language.h"
#include "expressions.h"
#include "user_error.h"

#include "gologpp.h"

namespace gologpp {


Scope &global_scope();



Expression *ref_to_global(
	const string &name,
	const boost::optional<vector<Expression *>> &args
);



class NoScopeOwner : public virtual AbstractLanguageElement {
public:
	virtual const Scope &scope() const override;
	virtual Scope &scope() override;
};



class ScopeOwner : public virtual AbstractLanguageElement {
public:
	ScopeOwner(Scope *owned_scope);

	virtual ~ScopeOwner() override = default;

	virtual const Scope &scope() const override;
	virtual Scope &scope() override;

protected:
	unique_ptr<Scope> scope_;
};



class Scope : public LanguageElement<Scope> {
private:
	Scope();

	template<class K, class V>
	using unordered_map = std::unordered_map<K, V>;

public:
	using VariablesMap = unordered_map<string, shared_ptr<AbstractVariable>>;
	using GlobalsMap = unordered_map<Identifier, shared_ptr<Global>>;
	using DomainsMap = unordered_map<Name, shared_ptr<AbstractDomain>>;

	Scope(AbstractLanguageElement &owner, const vector<shared_ptr<AbstractVariable>> &lookup_vars = {});
	Scope(Scope &parent_scope);

	Scope(Scope &&);
	Scope(const Scope &) = delete;
	Scope &operator = (const Scope &) = delete;

	~Scope() override;

	template<class ExpressionT>
	shared_ptr<Variable<ExpressionT>> get_var(const string &name)
	{
		shared_ptr<Variable<ExpressionT>> rv;
		rv = std::dynamic_pointer_cast<Variable<ExpressionT>>(lookup_var(name));
		if (!rv)
			rv.reset(new Variable<ExpressionT>(name));
		if (!has_var(name))
			variables_.emplace(name, rv); // Cache to avoid future scope walk

		return rv;
	}

	template<class ExpressionT>
	shared_ptr<Variable<ExpressionT>> get_local_var(const string &name)
	{
		if (!has_var(name))
			variables_.emplace(name, new Variable<ExpressionT>(name));
		return std::dynamic_pointer_cast<Variable<ExpressionT>>(variables_[name]);
	}

	shared_ptr<AbstractVariable> lookup_var(const string &name) const;
	bool has_var(const string &name) const;

	vector<shared_ptr<AbstractVariable>> lookup_vars(const vector<string> &names);
	vector<shared_ptr<AbstractVariable>> vars() const;

	void attach_semantics(SemanticsFactory &implementor) override;

	static Scope &global_scope()
	{ return global_scope_; }

	virtual Scope &scope() override;
	const Scope &scope() const override;

	virtual Scope &parent_scope() override;
	const Scope &parent_scope() const override;

	void set_owner(AbstractLanguageElement *owner);
	const AbstractLanguageElement *owner() const;
	AbstractLanguageElement *owner();

	template<class GologT = Global>
	shared_ptr<GologT> lookup_global(const string &name, arity_t arity)
	{
		if (exists_global(name, arity))
			return std::dynamic_pointer_cast<GologT>(
				(*globals_) [{ name, arity }]
			);
		else
			return shared_ptr<GologT>();
	}


	const VariablesMap &var_map() const;
	void implement_globals(SemanticsFactory &implementor, AExecutionContext &ctx);
	void clear();

	bool exists_global(const string &name, arity_t arity) const;


	template<class GologT>
	GologT *declare_global(
		Scope *own_scope,
		const string &name,
		const boost::optional<vector<shared_ptr<AbstractVariable>>> &args
	) {
		GologT *rv = nullptr;
		arity_t arity = static_cast<arity_t>(args.get_value_or({}).size());
		if (exists_global(name, arity)) {
			rv = lookup_global<GologT>(name, arity).get();

			// Here be dragons:
			// own_scope is constructed by the parser, and it may or may not
			// be the one that is already in use by the existing Global.
			// If it's not, we have to delete it since it won't be used.
			if (&(rv->scope()) != own_scope)
				delete own_scope;
		}
		else
			(*globals_) [ { name, arity } ]
				.reset(new GologT(own_scope, name, args.get_value_or({})));

		return lookup_global<GologT>(name, arity).get();
	}


	template<class GologT, class... DefinitionTs>
	GologT *define_global(
		Scope *own_scope, const string &name,
		const boost::optional<vector<shared_ptr<AbstractVariable>>> &args,
		DefinitionTs... definition_args
	) {
		GologT *rv = nullptr;
		arity_t arity = static_cast<arity_t>(args.get_value_or({}).size());
		if (exists_global(name, arity)) {
			rv = lookup_global<GologT>(name, arity).get();

			// Here be dragons:
			// own_scope is constructed by the parser, and it may or may not
			// be the one that is already in use by the existing Global.
			// If it's not, we have to delete it since it won't be used.
			if (&(rv->scope()) != own_scope)
				delete own_scope;

			// TODO: if (rv->defined()): Warn on redefinition
		}
		else
			rv = declare_global<GologT>(own_scope, name, args);
		rv->define(definition_args...);
		return rv;
	}


	bool exists_domain(const string &name) const;

	template<class ExprT>
	shared_ptr<Domain<ExprT>> lookup_domain(const string &name)
	{
		if (exists_domain(name))
			return std::dynamic_pointer_cast<Domain<ExprT>>(
				domains_->find(name)->second
			);
		else
			return shared_ptr<Domain<ExprT>>();
	}


	void register_global(Global *g);
	void register_domain(AbstractDomain *d);

	template<class ExprT>
	void register_domain(shared_ptr<Domain<ExprT>> &d)
	{
		if (exists_domain(d->name()))
			throw RedeclarationError(d->name());
		domains_->emplace(*d, d);
	}

	Constant<SymbolicExpression> *get_symbol(const string &name);

	virtual string to_string(const string &pfx) const override;


private:
	static Scope global_scope_;
	Scope &parent_scope_;
	AbstractLanguageElement *owner_;
	VariablesMap variables_;

	shared_ptr<GlobalsMap> globals_;
	shared_ptr<DomainsMap> domains_;
};



} // namespace gologpp







#endif
