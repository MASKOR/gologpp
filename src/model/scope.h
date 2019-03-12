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
#include "error.h"

#include "gologpp.h"

namespace gologpp {


Scope &global_scope();



Expression *ref_to_global(
	const string &name,
	const boost::optional<vector<Expression *>> &args
);



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
class Scope : public LanguageElement<Scope> {
private:
	Scope();

	template<class K, class V>
	using unordered_map = std::unordered_map<K, V>;

public:
	using VariablesMap = unordered_map<string, shared_ptr<AbstractVariable>>;
	using GlobalsMap = unordered_map<Identifier, shared_ptr<Global>>;
	using DomainsMap = unordered_map<Name, shared_ptr<AbstractDomain>>;
	using TypesMap = unordered_map<Name, shared_ptr<Type>>;

	explicit Scope(AbstractLanguageElement &owner, const vector<shared_ptr<AbstractVariable>> &lookup_vars = {});
	Scope(Scope &parent_scope);

	Scope(Scope &&);
	Scope(const Scope &) = delete;
	Scope &operator = (const Scope &) = delete;

	~Scope() override;

	template<class ExpressionT, bool only_local = false, bool allow_def = true>
	shared_ptr<Variable<ExpressionT>> get_var(const string &name)
	{
		shared_ptr<Variable<ExpressionT>> rv;

		auto it = variables_.find(name);
		if (it != variables_.end())
			rv = std::dynamic_pointer_cast<Variable<ExpressionT>>(it->second);

		if (!rv && !only_local && &parent_scope() != this)
			rv = std::dynamic_pointer_cast<Variable<ExpressionT>>(
				parent_scope().get_var<ExpressionT, false, false>(name)
			);

		if (!rv && allow_def) {
			rv.reset(new Variable<ExpressionT>(name));
			variables_[name] = rv;
		}

		return rv;
	}

	template<class ExpressionT>
	shared_ptr<Variable<ExpressionT>> get_local_var(const string &name)
	{ return get_var<ExpressionT, true, true>(name); }

	template<class ExpressionT>
	shared_ptr<Variable<ExpressionT>> lookup_local_var(const string &name) const
	{ return get_var<ExpressionT, true, false>(name); }

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


	bool exists_type(const string &name) const;

	template<class TypeT = Type>
	shared_ptr<TypeT> lookup_type(const string &name)
	{
		if (exists_type(name))
			return std::dynamic_pointer_cast<TypeT>(
				types_->find(name)->second
			);
		else
			return shared_ptr<TypeT>();
	}

	void register_type(Type *t);

	void register_global(Global *g);
	void register_domain(AbstractDomain *d);

	template<class ExprT>
	void register_domain(const shared_ptr<Domain<ExprT>> &d)
	{ domains_->emplace(*d, d); }

	template<class ExprT>
	void declare_domain(const string &name)
	{ domains_->emplace(name, std::make_shared<Domain<ExprT>>(name)); }


	template<class ExprT>
	void define_domain(const string &name, const Domain<ExprT> input)
	{
		shared_ptr<Domain<ExprT>> d = lookup_domain<ExprT>(name);
		if (d)
			d->define(input);
		else
			register_domain<ExprT>(std::make_shared<Domain<ExprT>>(name, input));
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
	shared_ptr<TypesMap> types_;
};



template<class T>
T &type()
{
	if (!global_scope().exists_type(T::static_name()))
		global_scope().register_type(new T());
	return *global_scope().lookup_type<T>(T::static_name());
}



} // namespace gologpp







#endif
