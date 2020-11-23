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

#include "value.h"
#include "semantics.h"

#include <boost/functional/hash.hpp>
#include <boost/fusion/include/at_c.hpp>

namespace gologpp {


unique_ptr<Value>::unique_ptr(const unique_ptr<Value> &c)
: std::unique_ptr<Value>(c->copy())
{}

gologpp::unique_ptr<Value>::unique_ptr(const Value &v)
: std::unique_ptr<Value>(v.copy())
{}

unique_ptr<Value> &unique_ptr<Value>::operator = (const unique_ptr<Value> &c)
{
	std::unique_ptr<Value>::operator = (std::make_unique<Value>(*c));
	return *this;
}

bool unique_ptr<Value>::operator == (const unique_ptr<Value> &other) const
{ return **this == *other; }

bool unique_ptr<Value>::operator != (const unique_ptr<Value> &other) const
{ return **this != *other; }


struct Value::to_string_visitor {
	string pfx;
	bool quote = true;

	string operator() (const string &s) const
	{ return pfx + (quote ? "\"" : "") + s + (quote ? "\"" : ""); }

	string operator() (bool b) const
	{ return pfx + (b ? "true" : "false"); }

	string operator() (const CompoundType::Representation &c) const
	{
		string rv = pfx + "{" linesep;
		for (const auto &pair : c)
			rv += pfx + indent + pair.first + " = " + pair.second->to_string("") + ", " linesep;
		return rv + pfx + "}" linesep;
	}

	string operator() (const ListType::Representation &c) const
	{
		string rv = pfx + "[";
		for (const auto &elem : c)
			rv += elem->str() + ", ";
		if (c.size() > 0)
			rv = rv.substr(0, rv.length() - 2);
		return rv + "]";
	}

	string operator() (void *p) const
	{
		if (!p)
			return "null";

		string rv;
		std::stringstream s(rv);
		s << pfx << std::hex << p;
		return rv;
	}

	template<class T>
	string operator() (const T &o) const
	{ return pfx + std::to_string(o); }
};


struct Value::hash_visitor {
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

	size_t operator () (const ListType::Representation &l) const
	{
		size_t rv = 0;
		for (const auto &p : l)
			boost::hash_combine(rv, p->hash());
		return rv;
	}
};


struct Value::attach_semantics_visitor {
	SemanticsFactory &f;

	template<class T>
	void operator () (const T&) const
	{}

	void operator () (CompoundType::Representation &v) const
	{
		for (auto &pair : v)
			pair.second->attach_semantics(f);
	}

	void operator () (ListType::Representation &vec) const
	{
		for (auto &p_val : vec)
			p_val->attach_semantics(f);
	}
};


Value::~Value()
{}

Value::Value(Representation &&l)
: representation_(std::move(l))
{}


template<>
Value::Value(const Type &t, const char *repr)
: representation_(string(repr))
{ set_type(t);  }


Value::Value(const Type &t, const vector<fusion_wtf_vector<string, Value *>> &compound_values)
{
	set_type(t);

	if (!type().is_compound())
		throw TypeError("Attempt to construct compound value, but type name \""
			+ t.name() + "\" does not refer to a compound type");

	const CompoundType &this_type = dynamic_cast<const CompoundType &>(type());
	CompoundType::Representation tmp_value;
	for (const boost::fusion::vector<string, Value *> &v : compound_values) {
		const string &field_name = boost::fusion::at_c<0>(v);
		const Type &field_type = this_type.field_type(field_name);
		Value *field_value = boost::fusion::at_c<1>(v);
		if (field_type >= *field_value)
			tmp_value[field_name].reset(field_value);
		else
			throw ExpressionTypeMismatch("Cannot assign " + field_value->str()
				+ " to a field of type " + field_type.name()
			);
	}

	representation_ = std::move(tmp_value);

	// TODO: check if all fields have been assigned!
}


Value::Value()
: representation_(nullptr)
{}


Value::Value(const Type &t, const boost::optional<vector<Value *>> &list_values)
{
	set_type(t);

	if (!type().is<ListType>())
		throw TypeError("Attempt to construct list value, but type name \""
			+ t.name() + "\" does not refer to a list type");

	ListType::Representation list_repr;
	for (Value *v : list_values.get_value_or({}))
		list_repr.emplace_back(v);
	representation_ = std::move(list_repr);
}


Value::Value(Value &&c)
{
	representation_ = std::move(c.representation_);
	set_type(c.type());
	semantics_ = std::move(c.semantics_);

	if (c.semantics_)
		semantics_.reset(c.general_semantics<Value>().copy(*this));
}


Value::Value(const Value &c)
{
	if (c.semantics_)
		semantics_.reset(c.general_semantics<Value>().copy(*this));

	this->representation_ = c.representation_;
	set_type(c.type());
}


Value &Value::operator = (const Value &c)
{
	if (c.semantics_)
		semantics_.reset(c.general_semantics<Value>().copy(*this));

	this->representation_ = c.representation_;
	set_type(c.type());

	return *this;
}


Value &Value::operator = (Value &&c)
{
	representation_ = std::move(c.representation_);
	set_type(c.type());

	if (c.semantics_)
		semantics_.reset(c.general_semantics<Value>().copy(*this));

	return *this;
}


size_t Value::hash() const
{ return boost::apply_visitor(hash_visitor(), representation_); }

string Value::to_string(const string &pfx) const
{ return boost::apply_visitor(to_string_visitor { pfx, !(get_type<SymbolType>() >= *this) }, representation_); }

Value *Value::copy() const
{ return new Value(*this); }


bool Value::operator == (const Expression &e) const
{
	try {
		const Value &c = dynamic_cast<const Value &>(e);
		return this->type() >= c.type() && this->type() <= c.type()
			&& representation() == c.representation();
	} catch (std::bad_cast &) {
		return false;
	}
}

bool Value::operator != (const Expression &other) const
{ return !(*this == other); }


bool Value::operator <= (const Type &t) const
{
	try {
		if (type().is<ListType>()) {
			const ListType &lt = dynamic_cast<const ListType &>(t);
			for (const auto &elem : boost::get<const ListType::Representation>(representation_))
				if (!(*elem <= lt.element_type()))
					return false;
			return true;
		}
	} catch (std::bad_cast &) {
		return false;
	}
	return t >= *this;
}


void Value::attach_semantics(SemanticsFactory &f)
{
	if (!semantics_) {
		boost::apply_visitor(attach_semantics_visitor { f }, representation_);
		set_semantics(f.make_semantics(*this));
	}
}


string Value::string_representation() const
{ return boost::apply_visitor(to_string_visitor { "", false }, representation_); }


Value Value::undefined()
{ return Value(); }



GeneralSemantics<Value>::GeneralSemantics(const Value &elem, AExecutionController &context)
: expression_(&elem)
, context_(context)
{}

const Expression &GeneralSemantics<Value>::expression() const
{ return *expression_; }

AExecutionController &GeneralSemantics<Value>::context() const
{ return context_; }

Value GeneralSemantics<Value>::evaluate(const BindingChain &, const History &)
{ return *expression_; }

const ModelElement &GeneralSemantics<Value>::model_element() const
{ return *expression_; }

const Value &GeneralSemantics<Value>::element() const
{ return *expression_; }

void GeneralSemantics<Value>::update_element(const Value *elem)
{ expression_ = elem; }



} // namespace gologpp
