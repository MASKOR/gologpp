#include "domain.h"

namespace gologpp {

AbstractDomain::AbstractDomain(const string &type_name, const string &name, bool implicit)
: Name(name)
, implicit_(implicit)
{ set_type_by_name(type_name); }



Domain domain_union(const Domain &lhs, const Domain &rhs)
{
	ensure_type_equality(lhs, rhs);
	Domain rv { "" };
	rv.add_elements(lhs);
	rv.add_elements(rhs);
	return std::move(rv);
}


Domain domain_difference(const Domain &lhs, const Domain &rhs)
{
	ensure_type_equality(lhs, rhs);
	Domain rv { "" };
	rv.add_elements(lhs);
	rv.remove(rhs);
	return std::move(rv);
}


Domain domain_intersection(const Domain &lhs, const Domain &rhs)
{
	ensure_type_equality(lhs, rhs);
	Domain rv { "" };
	rv.add_elements(lhs);
	rv.remove(domain_difference(lhs, rhs));
	rv.remove(domain_difference(rhs, lhs));
	return std::move(rv);
}


Domain domain_operation(const Domain &lhs, DomainOperator op, const Domain &rhs)
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


}
