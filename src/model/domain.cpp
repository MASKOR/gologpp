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

#include "domain.h"
#include "semantics.h"

namespace gologpp {


size_t Domain::id_count_ = 0;


string Domain::next_anon_name()
{ return "~unnamed_domain_" + std::to_string(id_count_++) + "~"; }


Domain::Domain(const string &name, const Type &elem_type, const vector<Value *> &elements, bool implicit)
: Domain(name, elem_type.shared_from_this(), elements, implicit)
{}

Domain::Domain(const string &name, shared_ptr<const Type> elem_type, const vector<Value *> &elements, bool implicit)
: Type(name)
, element_type_(elem_type)
, implicit_(implicit)
{ add_elements(elements); }

Domain::Domain(const Type &elem_type)
: Domain(next_anon_name(), elem_type, {}, true)
{}

Domain::Domain(const string &name, const Domain &other)
: Domain(name, other.element_type_, {}, false)
{ add_elements(other); }

Domain::Domain(const Domain &other)
: Domain(other.name(), other.element_type_, {}, other.implicit_)
{ add_elements(other); }

Domain::Domain()
: Type(next_anon_name())
, element_type_(get_type<UndefinedType>().shared_from_this())
, implicit_(false)
{}


Domain &Domain::operator = (const Domain &other)
{
	ensure_type(other.type());
	this->name_ = other.name_;
	this->subjects_ = other.subjects_;
	this->implicit_ = other.implicit_;
	this->element_type_ = other.element_type_;
	add_elements(other);
	return *this;
}

bool Domain::operator <= (const Type &other) const
{
	if (Type::operator <= (other) || element_type() <= other)
		return true;
	try {
		const Domain &d = dynamic_cast<const Domain &>(other);

		// d without *this = {} iff d subset this
		return domain_difference(*this, d).elements().empty();
	} catch (std::bad_cast &) {
		return false;
	}
}

bool Domain::operator >= (const Type &other) const
{
	if (Type::operator >= (other))
		return true;
	try {
		const Domain &d = dynamic_cast<const Domain &>(other);

		// d without *this = {} iff d subset this
		return domain_difference(d, *this).elements().empty();
	} catch (std::bad_cast &) {
		return false;
	}
}

bool Domain::operator >= (const AbstractLanguageElement &e) const
{
	if (*this >= e.type())
		return true;
	try {
		unique_ptr<Value> v(new Value(dynamic_cast<const Value &>(e)));
		return elements().find(v) != elements().end();
	} catch (std::bad_cast &) {
		return false;
	}
}

/**
 * @return whether this is an implicit domain.
 * By default, every @ref Variable has an empty implicit domain, which means its domain is undefined.
 * For @ref Fluent arguments, the domain can be implicitly defined through the initially: statement.
 */
bool Domain::is_implicit()
{ return implicit_; }

void Domain::add_subject(AbstractLanguageElement &subject)
{ subjects_.insert(&subject); }

std::unordered_set<AbstractLanguageElement *> &Domain::subjects()
{ return subjects_; }

const std::unordered_set<AbstractLanguageElement *> &Domain::subjects() const
{ return subjects_; }


void Domain::attach_semantics(SemanticsFactory &f)
{
	for (const unique_ptr<Value> &c : elements_)
		c->attach_semantics(f);
	set_semantics(f.make_semantics(*this));
}


string Domain::to_string(const string &) const
{
	return type().name() + " domain " + this->name() + " = { "
	+ concat_list(elements_, ", ") + "}";
}


Domain::ElementSet &Domain::elements()
{ return elements_; }

const Domain::ElementSet &Domain::elements() const
{ return elements_; }

const Type &Domain::element_type() const
{ return *element_type_; }

void Domain::add_element(const Value &c)
{ elements_.emplace(new Value(c)); }

Scope &Domain::parent_scope()
{ return global_scope(); }

const Scope &Domain::parent_scope() const
{ return global_scope(); }

bool Domain::is_defined() const
{ return elements_.size() > 0; }

void Domain::add_elements(const vector<Value *> &elements)
{
	if (elements.size())
		ensure_type(elements[0]->type());
	for (Value *c : elements) {
		if (!(element_type() >= *c))
			throw TypeError(*c, element_type());
		elements_.emplace(c);
	}
}


void Domain::add_elements(const Domain &other)
{
	ensure_type(other.type());
	for (const unique_ptr<Value> &e : other.elements()) {
		if (!(element_type() >= *e))
			throw TypeError(*e, element_type());
		elements_.emplace(new Value(*e));
	}
}


void Domain::remove(const Domain &other)
{
	ensure_type(other.type());
	for (const unique_ptr<Value> &e : other.elements())
		elements_.erase(e);
}


Domain domain_union(const Domain &lhs, const Domain &rhs)
{
	Domain rv { lhs };
	rv.add_elements(rhs);
	return rv;
}


Domain domain_difference(const Domain &lhs, const Domain &rhs)
{
	Domain rv { lhs };
	rv.remove(rhs);
	return rv;
}


Domain domain_intersection(const Domain &lhs, const Domain &rhs)
{
	Domain rv { lhs };
	rv.remove(domain_difference(lhs, rhs));
	rv.remove(domain_difference(rhs, lhs));
	return rv;
}


Domain domain_operation(const Domain &lhs, DomainOperator op, const Domain &rhs)
{
	switch(op) {
	case UNION:
		return std::move(domain_union(lhs, rhs));
	case DIFFERENCE:
		return std::move(domain_difference(lhs, rhs));
	case INTERSECTION:
		return std::move(domain_intersection(lhs, rhs));
	}
	throw Bug("Unhandled domain type");
}


}
