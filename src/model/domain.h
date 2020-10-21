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
 * A Domain's type is always the @a VoidType since it doesn't represent a value by itself.
 */
class Domain
: public virtual AbstractLanguageElement
, public Type
, public NoScopeOwner
, public LanguageElement<Domain>
{
public:
	typedef std::unordered_set<unique_ptr<Value>> ElementSet;
	typedef typename ElementSet::iterator ElementIterator;

	Domain(const string &name, const Type &elem_type, const vector<Value *> &elements = {}, bool implicit = false);
	Domain(const string &name, shared_ptr<const Type> elem_type, const vector<Value *> &elements = {}, bool implicit = false);
	Domain(const Type &elem_type);
	Domain(const string &name, const Domain &other);
	Domain(const Domain &other);
	Domain();

	virtual ~Domain() override = default;

	Domain &operator = (const Domain &other);

	virtual bool operator <= (const Type &other) const override;
	virtual bool operator >= (const Type &other) const override;
	virtual bool operator >= (const AbstractLanguageElement &e) const override;


	/**
	 * @return whether this is an implicit domain.
	 * By default, every @ref Variable has an empty implicit domain, which means its domain is undefined.
	 * For @ref Fluent arguments, the domain can be implicitly defined through the initially: statement.
	 */
	bool is_implicit(); // TODO: Throw out this whole implicit domain bullshit
	void add_subject(AbstractLanguageElement &subject);

	std::unordered_set<AbstractLanguageElement *> &subjects();
	const std::unordered_set<AbstractLanguageElement *> &subjects() const;

	ElementSet &elements();
	const ElementSet &elements() const;

	const Type &element_type() const;

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
	static string next_anon_name();

	ElementSet elements_;
	shared_ptr<const Type> element_type_;
	std::unordered_set<AbstractLanguageElement *> subjects_;
	bool implicit_;
	static size_t id_count_;
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
