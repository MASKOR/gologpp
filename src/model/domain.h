#ifndef GOLOGPP_DOMAIN_H_
#define GOLOGPP_DOMAIN_H_

#include "gologpp.h"
#include "language.h"
#include "scope.h"
#include "expressions.h"
#include "constant.h"
#include "error.h"

#include <unordered_set>

namespace gologpp {

/**
 * @class AbstractDomain
 * A domain is not a @ref Global since it's compile-time constant in golog++.
 * We also don't need explicit reference representation, i.e. there is no Reference<Domain<...>>.
 * Since domains don't have arguments and anonymous domains can always appear inline,
 * the syntactic separation between named global definition and inline specification is not needed.
 */
class AbstractDomain
: public virtual AbstractLanguageElement
, public Name
, public NoScopeOwner
{
public:
	AbstractDomain(const string &name, bool implicit);
	virtual ~AbstractDomain() override = default;

	/**
	 * @return whether this is an implicit domain.
	 * By default, every @ref Variable has an empty implicit domain, which means its domain is undefined.
	 * For @ref Fluent arguments, the domain can be implicitly defined through the initially: statement.
	 */
	bool is_implicit()
	{ return implicit_; }

	virtual bool is_defined() const = 0;

	void add_subject(AbstractLanguageElement &subject)
	{ subjects_.insert(&subject); }

	std::unordered_set<AbstractLanguageElement *> &subjects()
	{ return subjects_; }

	const std::unordered_set<AbstractLanguageElement *> &subjects() const
	{ return subjects_; }

protected:
	std::unordered_set<AbstractLanguageElement *> subjects_;
	bool implicit_;
};



template<class ExprT>
class Domain
: public AbstractDomain
, public LanguageElement<Domain<ExprT>>
, public std::enable_shared_from_this<Domain<ExprT>>
{
public:
	typedef std::unordered_set<unique_ptr<Constant<ExprT>>> ElementSet;
	typedef typename ElementSet::iterator ElementIterator;

	Domain(const string &name, const vector<Constant<ExprT> *> &elements = {}, bool implicit = false)
	: AbstractDomain(name, implicit)
	{ add_elements(elements); }

	Domain()
	: AbstractDomain("", true)
	{}

	Domain(const string &name, const Domain<ExprT> &other)
	: AbstractDomain(name, false)
	{ add_elements(other); }

	Domain(const Domain<ExprT> &other)
	: AbstractDomain(other.name(), other.implicit_)
	{ add_elements(other); }

	virtual ~Domain() override = default;

	Domain<ExprT> &operator = (const Domain<ExprT> &other)
	{
		this->name_ = other.name_;
		this->subjects_ = other.subjects_;
		this->implicit_ = other.implicit_;
		add_elements(other);
		return *this;
	}

	void attach_semantics(SemanticsFactory &f) override
	{
		for (const unique_ptr<Constant<ExprT>> &c : elements_)
			c->attach_semantics(f);
		set_implementation(f.make_semantics(*this));
	}


	virtual string to_string(const string &) const override
	{
		return gologpp::to_string(ExprT::static_type_tag()) + "domain " + this->name() + " = { "
			+ concat_list(elements_, ", ") + "};";
	}


	ElementSet &elements()
	{ return elements_; }

	const ElementSet &elements() const
	{ return elements_; }

	void add_element(const Constant<ExprT> &c)
	{ elements_.emplace(new Constant<ExprT>(c)); }

	virtual Scope &parent_scope() override
	{ return global_scope(); }

	virtual const Scope &parent_scope() const override
	{ return global_scope(); }

	virtual bool is_defined() const override
	{ return elements_.size() > 0; }

	void add_elements(const vector<Constant<ExprT> *> &elements)
	{
		for (Constant<ExprT> *c : elements)
			elements_.emplace(c);
	}


	void add_elements(const Domain<ExprT> &other)
	{
		for (const unique_ptr<Constant<ExprT>> &e : other.elements())
			elements_.insert(std::make_unique<Constant<ExprT>>(*e));
	}


	template<class DefinitionT>
	void define(const DefinitionT def)
	{
		if (is_defined())
			throw RedefinitionError(this->name());
		add_elements(def);
	}


	void remove(const Domain<ExprT> &other)
	{
		for (const unique_ptr<Constant<ExprT>> &e : other.elements())
			elements_.erase(e);
	}

protected:
	ElementSet elements_;
};


enum DomainOperator {
	UNION, DIFFERENCE, INTERSECTION
};


template<class ExprT>
Domain<ExprT> domain_union(const Domain<ExprT> &lhs, const Domain<ExprT> &rhs)
{
	Domain<ExprT> rv { "" };
	rv.add_elements(lhs);
	rv.add_elements(rhs);
	return std::move(rv);
}


template<class ExprT>
Domain<ExprT> domain_difference(const Domain<ExprT> &lhs, const Domain<ExprT> &rhs)
{
	Domain<ExprT> rv { "" };
	rv.add_elements(lhs);
	rv.remove(rhs);
	return std::move(rv);
}


template<class ExprT>
Domain<ExprT> domain_intersection(const Domain<ExprT> &lhs, const Domain<ExprT> &rhs)
{
	Domain<ExprT> rv { "" };
	rv.add_elements(lhs);
	rv.remove(domain_difference(lhs, rhs));
	rv.remove(domain_difference(rhs, lhs));
	return std::move(rv);
}


template<class ExprT>
Domain<ExprT> domain_operation(const Domain<ExprT> &lhs, DomainOperator op, const Domain<ExprT> &rhs)
{
	switch(op) {
	case UNION:
		return std::move(domain_union(lhs, rhs));
		break;
	case DIFFERENCE:
		return std::move(domain_difference(lhs, rhs));
		break;
	case INTERSECTION:
		return std::move(domain_intersection(lhs, rhs));
		break;
	}
	throw Bug("Unhandled domain type");
}


} // namespace gologpp

#endif // GOLOGPP_DOMAIN_H_
