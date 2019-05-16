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
	AbstractDomain(const string &type_name, const string &name, bool implicit);
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



class Domain
: public AbstractDomain
, public LanguageElement<Domain>
, public std::enable_shared_from_this<Domain>
{
public:
	typedef std::unordered_set<unique_ptr<Constant>> ElementSet;
	typedef typename ElementSet::iterator ElementIterator;

	Domain(const string &type_name, const string &name, const vector<Constant *> &elements = {}, bool implicit = false)
	: AbstractDomain(type_name, name, implicit)
	{ add_elements(elements); }

	Domain(const string &type_name)
	: AbstractDomain(type_name, "", true)
	{}

	Domain(const string &type_name, const string &name, const Domain &other)
	: AbstractDomain(type_name, name, false)
	{
		ensure_type_equality(*this, other);
		add_elements(other);
	}

	Domain(const Domain &other)
	: AbstractDomain(other.type().name(), other.name(), other.implicit_)
	{
		ensure_type_equality(*this, other);
		add_elements(other);
	}

	virtual ~Domain() override = default;

	Domain &operator = (const Domain &other)
	{
		ensure_type_equality(*this, other);
		this->name_ = other.name_;
		this->subjects_ = other.subjects_;
		this->implicit_ = other.implicit_;
		add_elements(other);
		return *this;
	}

	void attach_semantics(SemanticsFactory &f) override
	{
		for (const unique_ptr<Constant> &c : elements_)
			c->attach_semantics(f);
		set_implementation(f.make_semantics(*this));
	}


	virtual string to_string(const string &) const override
	{
		return gologpp::to_string(type().dynamic_tag()) + "domain " + this->name() + " = { "
			+ concat_list(elements_, ", ") + "};";
	}


	ElementSet &elements()
	{ return elements_; }

	const ElementSet &elements() const
	{ return elements_; }

	void add_element(const Constant &c)
	{ elements_.emplace(new Constant(c)); }

	virtual Scope &parent_scope() override
	{ return global_scope(); }

	virtual const Scope &parent_scope() const override
	{ return global_scope(); }

	virtual bool is_defined() const override
	{ return elements_.size() > 0; }

	void add_elements(const vector<Constant *> &elements)
	{
		for (Constant *c : elements) {
			ensure_type_equality(*this, *c);
			elements_.emplace(c);
		}
	}


	void add_elements(const Domain &other)
	{
		for (const unique_ptr<Constant> &e : other.elements()) {
			ensure_type_equality(*this, *e);
			elements_.emplace(e->copy());
		}
	}


	template<class DefinitionT>
	void define(const DefinitionT def)
	{
		if (is_defined())
			throw RedefinitionError(this->name());
		add_elements(def);
	}


	void remove(const Domain &other)
	{
		ensure_type_equality(*this, other);
		for (const unique_ptr<Constant> &e : other.elements())
			elements_.erase(e);
	}

protected:
	ElementSet elements_;
};


enum DomainOperator {
	UNION, DIFFERENCE, INTERSECTION
};


Domain domain_union(const Domain &lhs, const Domain &rhs);
Domain domain_difference(const Domain &lhs, const Domain &rhs);
Domain domain_intersection(const Domain &lhs, const Domain &rhs);
Domain domain_operation(const Domain &lhs, DomainOperator op, const Domain &rhs);


} // namespace gologpp

#endif // GOLOGPP_DOMAIN_H_
