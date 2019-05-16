#include "constant.h"

#include <boost/functional/hash.hpp>
#include <boost/fusion/include/at_c.hpp>

namespace gologpp {


struct Constant::copy_visitor {
	template<class T, class U>
	void operator () (const T &, U &) const
	{ throw Bug("Mismatched boost::variant copy"); }

	template<class T>
	void operator () (const T &src, T &tgt) const
	{ tgt = src; }

	void operator () (const CompoundType::Representation &src, CompoundType::Representation &tgt) const
	{
		tgt.clear();
		for (const CompoundType::Representation::value_type &pair : src)
			tgt[pair.first].reset(pair.second->copy());
	}
};


struct Constant::to_string_visitor {
	string pfx;

	string operator() (const string &s) const
	{ return pfx + s; }

	string operator() (bool b) const
	{ return pfx + (b ? "true" : "false"); }

	string operator() (const CompoundType::Representation &c) const
	{
		string rv = pfx + "{" linesep;
		for (const auto &pair : c)
			rv += pfx + pfx + pair.first + " = " + pair.second->to_string("") + ", " linesep;
		return rv + pfx + "}" linesep;
	}

	template<class T>
	string operator() (const T &o) const
	{ return pfx + std::to_string(o); }
};


struct Constant::hash_visitor {
	template<class T>
	size_t operator () (const T &v) const
	{ return boost::hash_value(v); }

	size_t operator () (const CompoundType::Representation &v) const
	{
		size_t rv = 0;
		for (const auto &pair : v)
			boost::hash_combine(rv, pair.second->hash());

		return rv;
	}
};


struct Constant::attach_semantics_visitor {
	SemanticsFactory &f;

	template<class T>
	void operator () (const T&) const
	{}

	void operator () (CompoundType::Representation &v) const
	{
		for (auto &pair : v)
			pair.second->attach_semantics(f);
	}
};


Constant::~Constant()
{}

bool Constant::operator != (const Constant &other) const
{ return !(*this == other); }


Constant::Constant(LiteralVariant &&l)
: representation_(std::move(l))
{}


template<class ReprT>
Constant::Constant(const string &type_name, ReprT repr)
: representation_(repr)
{
	set_type_by_name(type_name);
}

template
Constant::Constant(const string &type_name, const string &);

template
Constant::Constant(const string &type_name, string);

template
Constant::Constant(const string &type_name, bool);

template
Constant::Constant(const string &type_name, int);

template
Constant::Constant(const string &type_name, long);

template
Constant::Constant(const string &type_name, double);


Constant::Constant(const string &type_name, const vector<fusion_wtf_vector<string, Constant *>> &definition)
{
	set_type_by_name(type_name);
	const CompoundType &this_type = dynamic_cast<const CompoundType &>(type());
	CompoundType::Representation tmp_value;
	for (const boost::fusion::vector<string, Constant *> &v : definition) {
		const string &field_name = boost::fusion::at_c<0>(v);
		const Type &field_type = this_type.field_type(field_name);
		Constant *field_value = boost::fusion::at_c<1>(v);
		if (field_type == field_value->type())
			tmp_value[field_name].reset(field_value);
		else
			throw ExpressionTypeMismatch("Cannot assign " + field_value->str()
				+ " to a field of type " + field_type.name()
			);
	}

	representation_ = std::move(tmp_value);

	// TODO: check if all fields have been assigned!
}


Constant::Constant(Constant &&c)
{
	ensure_type_equality(*this, c);

	semantics_ = std::move(c.semantics_);
	type_ = std::move(c.type_);
	representation_ = std::move(c.representation_);
}


Constant::Constant(const Constant &c)
{
	ensure_type_equality(*this, c);

	if (semantics_)
		throw Bug("Copying a Constant after Semantics have been assigned is forbidden");

	boost::apply_visitor(Constant::copy_visitor(), c.representation_, this->representation_);
	set_type_by_name(c.type());
}



Constant &Constant::operator = (const Constant &c)
{
	ensure_type_equality(*this, c);
	if (semantics_)
		throw Bug("Copying a Constant after Semantics have been assigned is forbidden");

	boost::apply_visitor(copy_visitor(), c.representation_ , this->representation_);
	set_type_by_name(c.type());

	return *this;
}


Constant &Constant::operator = (Constant &&c)
{
	representation_ = std::move(c.representation_);
	type_ = std::move(c.type_);
	semantics_ = std::move(c.semantics_);

	return *this;
}


size_t Constant::hash() const
{ return boost::apply_visitor(hash_visitor(), representation_); }

string Constant::to_string(const string &pfx) const
{ return boost::apply_visitor(to_string_visitor { pfx }, representation_); }

Constant *Constant::copy() const
{ return new Constant(*this); }


bool Constant::operator == (const Constant &c) const
{
	return this->type() == c.type()
		&& variant() == c.variant();
}


void Constant::attach_semantics(SemanticsFactory &f)
{
	if (!semantics_) {
		boost::apply_visitor(attach_semantics_visitor { f }, representation_);
		set_implementation(f.make_semantics(*this));
	}
}


vector<unique_ptr<Constant>> copy(const vector<unique_ptr<Constant>> &v)
{
	vector<unique_ptr<Constant>> rv;
	for (const unique_ptr<Constant> &c : v)
		rv.emplace_back(c->copy());
	return rv;
}





} // namespace gologpp
