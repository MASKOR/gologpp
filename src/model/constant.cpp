#include "constant.h"

#include <boost/functional/hash.hpp>
#include <boost/fusion/include/at_c.hpp>

namespace gologpp {

AbstractConstant::~AbstractConstant()
{}

bool AbstractConstant::operator != (const AbstractConstant &other) const
{ return !(*this == other); }



SimpleConstant::SimpleConstant(const LiteralVariant &l)
: representation_(l)
{}

SimpleConstant::SimpleConstant(LiteralVariant &&l)
: representation_(std::move(l))
{}

size_t SimpleConstant::hash() const
{ return boost::hash_value(variant()); }



template<class ExprT> template<class ReprT>
Constant<ExprT>::Constant(ReprT repr)
: SimpleConstant(repr)
{}

template
Constant<SymbolicExpression>::Constant(const string &);

template
Constant<SymbolicExpression>::Constant(string);

template
Constant<StringExpression>::Constant(const string &);

template
Constant<StringExpression>::Constant(string);

template
Constant<BooleanExpression>::Constant(bool);

template
Constant<NumericExpression>::Constant(int);

template
Constant<NumericExpression>::Constant(long);

template
Constant<NumericExpression>::Constant(double);


Constant<CompoundExpression>::Constant(const string &type_name, const vector<fusion_wtf_vector<string, AbstractConstant *>> &definition)
{
	set_type_by_name(type_name);
	for (const boost::fusion::vector<string, AbstractConstant *> &v : definition) {
		const string &field_name = boost::fusion::at_c<0>(v);
		const Type &field_type = type().field_type(field_name);
		AbstractConstant *field_value = boost::fusion::at_c<1>(v);
		if (field_type == field_value->type())
			representation_[field_name].reset(field_value);
		else
			throw ExpressionTypeMismatch("Cannot assign " + field_value->str()
				+ " to a field of type " + field_type.name()
			);
	}
	// TODO: check if all fields have been assigned!
}


Constant<CompoundExpression>::Constant(const Constant<CompoundExpression> &c)
{
	if (semantics_)
		throw Bug("Copying a Constant after Semantics have been assigned is forbidden");
	for (const auto &v : c.representation_)
		representation_[v.first].reset(v.second->copy());
	set_type_by_name(c.type());
}


Constant<CompoundExpression>::Constant(Constant<CompoundExpression> &&c)
{
	representation_ = std::move(c.representation_);
	semantics_ = std::move(c.semantics_);
	type_ = std::move(c.type_);
}


Constant<CompoundExpression> &Constant<CompoundExpression>::operator = (const Constant<CompoundExpression> &c)
{
	representation_.clear();
	for (const auto &v : c.representation_)
		representation_[v.first].reset(v.second->copy());
	set_type_by_name(c.type());
	return *this;
}

size_t Constant<CompoundExpression>::hash() const
{
	size_t rv = 0;
	for (const auto &v : representation_)
		boost::hash_combine(rv, v.second->hash());
	return rv;
}

const Constant<CompoundExpression>::Value &Constant<CompoundExpression>::value() const
{ return representation_; }

Type::Tag Constant<CompoundExpression>::dynamic_type_tag() const
{ return type().dynamic_tag(); }

Constant<CompoundExpression> *Constant<CompoundExpression>::copy() const
{ return new Constant<CompoundExpression>(*this); }


bool Constant<CompoundExpression>::operator == (const AbstractConstant &c) const
{
	if (type() != c.type())
		return false;
	const Constant<CompoundExpression> &cc = dynamic_cast<const Constant<CompoundExpression> &>(c);
	for (const auto &v : representation_) {
		auto it = cc.representation_.find(v.first);
		if (it == cc.representation_.end() || *v.second != *it->second)
			return false;
	}
	return true;
}


string Constant<CompoundExpression>::to_string(const string &pfx) const
{
	string rv = pfx + "{" linesep;
	for (const auto &pair : representation_)
		rv += pfx + pfx + pair.first + " = " + pair.second->to_string("") + ", " linesep;
	return rv + pfx + "}" linesep;
}



vector<unique_ptr<AbstractConstant>> copy(const vector<unique_ptr<AbstractConstant>> &v)
{
	vector<unique_ptr<AbstractConstant>> rv;
	for (const unique_ptr<AbstractConstant> &c : v)
		rv.emplace_back(c->copy());
	return rv;
}





} // namespace gologpp
