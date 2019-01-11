#include "constant.h"

#include <boost/functional/hash.hpp>

namespace gologpp {

AbstractConstant::~AbstractConstant()
{}

size_t AbstractConstant::hash() const
{ return boost::hash_value(representation_); }

bool AbstractConstant::operator != (const AbstractConstant &other) const
{ return !(*this == other); }

const AbstractConstant::LiteralVariant &AbstractConstant::variant() const
{ return representation_; }



template<> template<>
Constant<SymbolicExpression>::Constant(const string &repr)
: AbstractConstant(repr)
{}

template<> template<>
Constant<SymbolicExpression>::Constant(string repr)
: AbstractConstant(repr)
{}

template<> template<>
Constant<StringExpression>::Constant(const string &repr)
: AbstractConstant(repr)
{}

template<> template<>
Constant<StringExpression>::Constant(string repr)
: AbstractConstant(repr)
{}

template<> template<>
Constant<BooleanExpression>::Constant(bool repr)
: AbstractConstant(repr)
{}

template<> template<>
Constant<NumericExpression>::Constant(int repr)
: AbstractConstant(repr)
{}

template<> template<>
Constant<NumericExpression>::Constant(long repr)
: AbstractConstant(repr)
{}

template<> template<>
Constant<NumericExpression>::Constant(double repr)
: AbstractConstant(repr)
{}


vector<unique_ptr<AbstractConstant>> copy(const vector<unique_ptr<AbstractConstant>> &v)
{
	vector<unique_ptr<AbstractConstant>> rv;
	for (const unique_ptr<AbstractConstant> &c : v)
		rv.emplace_back(c->copy());
	return rv;
}


} // namespace gologpp
