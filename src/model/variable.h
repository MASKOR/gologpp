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

#ifndef GOLOGPP_VARIABLE_H_
#define GOLOGPP_VARIABLE_H_

#include "gologpp.h"
#include "language.h"
#include "utilities.h"
#include "expressions.h"
#include "scope.h"
#include "domain.h"

namespace gologpp {


class Variable
: public LanguageElement<Variable>
, public Identifier
, public Expression
, public NoScopeOwner
, public std::enable_shared_from_this<Variable>
{
protected:
	Variable(const Type &type, const string &name);

public:
	virtual ~Variable() override = default;

	friend Scope;

	Reference<Variable> *ref();

	Domain &domain();
	const Domain &domain() const;

	/**
	 * Add @param c to this Variable's @class Domain if it is an implicit domain.
	 * If it's an explicit domain, this method does nothing.
	 */
	virtual void add_implicit_domain_element(const Value &c);
	void set_domain(const string &domain_name);
	void set_domain_copy(const Domain &domain);
	virtual void define_implicit_domain(const string &domain_name);

	virtual string to_string(const string &) const override;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*domain_)

private:
	shared_ptr<Domain> domain_;
};


} // namespace gologpp



namespace std {

template<>
struct hash<shared_ptr<gologpp::Variable>> {
	size_t operator () (const shared_ptr<const gologpp::Variable> &o) const
	{ return o->hash(); }
};

template<>
struct equal_to<shared_ptr<gologpp::Variable>> {
	bool operator () (const shared_ptr<const gologpp::Variable> &lhs, const shared_ptr<const gologpp::Variable> &rhs) const
	{ return *lhs == *rhs; }
};

} // namespace std



#endif // GOLOGPP_VARIABLE_H_
