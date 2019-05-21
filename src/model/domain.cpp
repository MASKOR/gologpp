#include "domain.h"

namespace gologpp {


Domain::Domain(const string &name, const string &type_name, const vector<Constant *> &elements, bool implicit)
: Name(name)
, implicit_(implicit)
{
	set_type_by_name(type_name);
	add_elements(elements);
}

Domain::Domain(const string &type_name)
: Domain(type_name, "", {}, true)
{}

Domain::Domain(const string &name, const Domain &other)
: Domain(name, other.type().name(), {}, false)
{ add_elements(other); }

Domain::Domain(const Domain &other)
: Domain(other.type().name(), other.name(), {}, other.implicit_)
{
	ensure_type_equality(*this, other);
	add_elements(other);
}

Domain::Domain()
: Name("")
, implicit_(false)
{}


Domain &Domain::operator = (const Domain &other)
{
	ensure_type_equality(*this, other);
	this->name_ = other.name_;
	this->subjects_ = other.subjects_;
	this->implicit_ = other.implicit_;
	add_elements(other);
	return *this;
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
	for (const unique_ptr<Constant> &c : elements_)
		c->attach_semantics(f);
	set_implementation(f.make_semantics(*this));
}


string Domain::to_string(const string &) const
{
	return gologpp::to_string(type().dynamic_tag()) + "domain " + this->name() + " = { "
	+ concat_list(elements_, ", ") + "};";
}


Domain::ElementSet &Domain::elements()
{ return elements_; }

const Domain::ElementSet &Domain::elements() const
{ return elements_; }

void Domain::add_element(const Constant &c)
{ elements_.emplace(new Constant(c)); }

Scope &Domain::parent_scope()
{ return global_scope(); }

const Scope &Domain::parent_scope() const
{ return global_scope(); }

bool Domain::is_defined() const
{ return elements_.size() > 0; }

void Domain::add_elements(const vector<Constant *> &elements)
{
	for (Constant *c : elements) {
		ensure_type_equality(*this, *c);
		elements_.emplace(c);
	}
}


void Domain::add_elements(const Domain &other)
{
	for (const unique_ptr<Constant> &e : other.elements()) {
		ensure_type_equality(*this, *e);
		elements_.emplace(e->copy());
	}
}


void Domain::remove(const Domain &other)
{
	ensure_type_equality(*this, other);
	for (const unique_ptr<Constant> &e : other.elements())
		elements_.erase(e);
}



Domain domain_union(const Domain &lhs, const Domain &rhs)
{
	ensure_type_equality(lhs, rhs);
	Domain rv { "" };
	rv.add_elements(lhs);
	rv.add_elements(rhs);
	return rv;
}


Domain domain_difference(const Domain &lhs, const Domain &rhs)
{
	ensure_type_equality(lhs, rhs);
	Domain rv { "" };
	rv.add_elements(lhs);
	rv.remove(rhs);
	return rv;
}


Domain domain_intersection(const Domain &lhs, const Domain &rhs)
{
	ensure_type_equality(lhs, rhs);
	Domain rv { "" };
	rv.add_elements(lhs);
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
