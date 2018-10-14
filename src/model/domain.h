#ifndef GOLOGPP_DOMAIN_H_
#define GOLOGPP_DOMAIN_H_

#include "gologpp.h"
#include "language.h"
#include "scope.h"
#include "expressions.h"
#include "constant.h"

#include <unordered_set>

namespace gologpp {

/**
 * @class AbstractDomain
 * A domain is not a @class Global since it's compile-time constant in golog++.
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
	 * By default, every @class Variable has an empty implicit domain, which means its domain is undefined.
	 * For @class Fluent arguments, the domain can be implicitly defined through the initially: statement.
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
	Domain(const string &name, vector<Constant<ExprT> *> elements = {}, bool implicit = false)
	: AbstractDomain(name, implicit)
	{
		for (Constant<ExprT> *c : elements) {
			elements_.emplace(std::move(*c));
			delete c;
		}
	}

	Domain()
	: AbstractDomain("", true)
	{}

	virtual ~Domain() override = default;

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

	std::unordered_set<unique_ptr<Constant<ExprT>>> &elements()
	{ return elements_; }

	const std::unordered_set<unique_ptr<Constant<ExprT>>> &elements() const
	{ return elements_; }

	void add_element(const Constant<ExprT> &c)
	{ elements_.emplace(new Constant<ExprT>(c)); }

	virtual Scope &parent_scope() override
	{ return global_scope(); }

	virtual const Scope &parent_scope() const override
	{ return global_scope(); }

	virtual bool is_defined() const override
	{ return elements_.size() > 0; }


protected:
	std::unordered_set<unique_ptr<Constant<ExprT>>> elements_;
};



} // namespace gologpp

#endif // GOLOGPP_DOMAIN_H_
