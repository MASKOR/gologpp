#ifndef GOLOGPP_DOMAIN_H_
#define GOLOGPP_DOMAIN_H_

#include "gologpp.h"
#include "language.h"
#include "scope.h"
#include "expressions.h"
#include "value.h"
#include "error.h"

#include <unordered_set>

namespace gologpp {


/**
 * @class Domain
 * A domain is not a @ref Global since it's compile-time constant in golog++.
 * We also don't need explicit reference representation, i.e. there is no Reference<Domain<...>>.
 * Since domains don't have arguments and anonymous domains can always appear inline,
 * the syntactic separation between named global definition and inline specification is not needed.
 */
class Domain
: public virtual AbstractLanguageElement
, public Name
, public NoScopeOwner
, public LanguageElement<Domain>
, public std::enable_shared_from_this<Domain>
{
public:
	typedef std::unordered_set<unique_ptr<Value>> ElementSet;
	typedef typename ElementSet::iterator ElementIterator;

	Domain(const string &name, const string &type_name, const vector<Value *> &elements = {}, bool implicit = false);
	Domain(const string &type_name);
	Domain(const string &name, const Domain &other);
	Domain(const Domain &other);
	Domain();

	virtual ~Domain() override = default;

	Domain &operator = (const Domain &other);


	/**
	 * @return whether this is an implicit domain.
	 * By default, every @ref Variable has an empty implicit domain, which means its domain is undefined.
	 * For @ref Fluent arguments, the domain can be implicitly defined through the initially: statement.
	 */
	bool is_implicit();
	void add_subject(AbstractLanguageElement &subject);

	std::unordered_set<AbstractLanguageElement *> &subjects();
	const std::unordered_set<AbstractLanguageElement *> &subjects() const;

	ElementSet &elements();
	const ElementSet &elements() const;

	void add_element(const Value &c);

	virtual Scope &parent_scope() override;
	virtual const Scope &parent_scope() const override;

	virtual bool is_defined() const;

	void add_elements(const vector<Value *> &elements);
	void add_elements(const Domain &other);

	void remove(const Domain &other);

	template<class DefinitionT>
	void define(const DefinitionT &def);

	void attach_semantics(SemanticsFactory &f) override;

	virtual string to_string(const string &) const override;


private:
	ElementSet elements_;
	std::unordered_set<AbstractLanguageElement *> subjects_;
	bool implicit_;
};


template<class DefinitionT>
void Domain::define(const DefinitionT &def)
{
	if (is_defined())
		throw RedefinitionError(this->name());
	add_elements(def);
}



enum DomainOperator {
	UNION, DIFFERENCE, INTERSECTION
};


Domain domain_union(const Domain &lhs, const Domain &rhs);
Domain domain_difference(const Domain &lhs, const Domain &rhs);
Domain domain_intersection(const Domain &lhs, const Domain &rhs);
Domain domain_operation(const Domain &lhs, DomainOperator op, const Domain &rhs);


} // namespace gologpp

#endif // GOLOGPP_DOMAIN_H_
