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

#ifndef GOLOGPP_CONSTANT_H_
#define GOLOGPP_CONSTANT_H_

#include "gologpp.h"
#include "language.h"
#include "expressions.h"
#include "scope.h"
#include "error.h"

#include <boost/variant.hpp>
#include <boost/fusion/include/vector.hpp>
#include <unordered_map>


namespace gologpp {


template<class T1, class T2>
#ifdef BOOST_FUSION_HAS_VARIADIC_VECTOR
using fusion_wtf_vector = boost::fusion::vector<T1, T2>;
#else
using fusion_wtf_vector = boost::fusion::vector2<T1, T2>;
#endif


template<>
class unique_ptr<Value> : public std::unique_ptr<Value> {
public:
	using std::unique_ptr<Value>::unique_ptr;

	unique_ptr()
	: std::unique_ptr<Value>()
	{}

	unique_ptr(const unique_ptr<Value> &c);

	unique_ptr<Value> &operator = (const unique_ptr<Value> &c);

	bool operator == (const unique_ptr<Value> &other) const;
	bool operator != (const unique_ptr<Value> &other) const;
};



class Value
: public Expression
, public NoScopeOwner
, public LanguageElement<Value>
{
public:
	using Representation = boost::variant <
		unsigned int, int, unsigned long, long, double, // NumberType
		string, // StringType, SymbolType
		bool, // BoolType
		CompoundType::Representation,
		ListType::Representation,
		void *
	>;

	// For ReprT = const char * there is an explicit specialization since it would be
	// converted to bool otherwise (see below and in .cpp).
	template<class ReprT>
	explicit Value(const string &type_name, ReprT repr) : representation_(repr) {
		set_type_by_name(type_name);
	}

	Value(const string &type_name, const vector<fusion_wtf_vector<string, Value *>> &compound_values);
	Value(const string &type_name, const boost::optional<vector<Value *>> &list_values);
	Value(Value &&c);
	Value(const Value &c);
	explicit Value();

	Value &operator = (Value &&);
	Value &operator = (const Value &c);

	virtual ~Value() override;

	virtual size_t hash() const;

	/* TODO: This needs to go. We cannot directly do casting with boost::get
	 *       since we might end up calling boost::get for a type that is in the
	 *       signature, but incorrect. Imagine upcasting an unsigned int to long.
	 *       Will either require nested casts or trigger boost::bad_get! */

	operator int () const
	{ return boost::get<int>(representation()); }

	operator unsigned int () const
	{ return boost::get<unsigned int>(representation()); }

	operator long () const
	{ return boost::get<long>(representation()); }

	operator unsigned long () const
	{ return boost::get<unsigned long>(representation()); }

	operator double () const
	{ return boost::get<double>(representation()); }

	operator string () const
	{ return boost::get<string>(representation()); }

	operator bool () const
	{ return boost::get<bool>(representation()); }

	operator const CompoundType::Representation &() const
	{ return boost::get<CompoundType::Representation>(representation()); }

	operator const ListType::Representation &() const
	{ return boost::get<ListType::Representation>(representation()); }

	operator void *() const
	{ return boost::get<void *>(representation()); }

	Representation &representation()
	{ return representation_; }

	const Representation &representation() const
	{ return representation_; }

	virtual string to_string(const string &) const override;

	virtual bool operator == (const Value &c) const;

	bool operator != (const Value &) const;
	virtual Value *copy() const;

	virtual void attach_semantics(SemanticsFactory &f) override;

	static Value undefined();

protected:
	Value(Representation &&repr);

private:
	Representation representation_;

	struct to_string_visitor;
	struct hash_visitor;
	struct attach_semantics_visitor;
};


template<>
Value::Value(const string &type_name, const char *repr);


vector<unique_ptr<Value>> copy(const vector<unique_ptr<Value>> &v);



} // namespace gologpp



namespace std {

template<>
struct hash<gologpp::unique_ptr<gologpp::Value>> {
	size_t operator () (const gologpp::unique_ptr<gologpp::Value> &o) const
	{ return o->hash(); }
};

template<>
struct equal_to<gologpp::unique_ptr<gologpp::Value>> {
	bool operator () (const gologpp::unique_ptr<gologpp::Value> &lhs, const gologpp::unique_ptr<gologpp::Value> &rhs) const
	{ return *lhs == *rhs; }
};

} // namespace std

#endif //GOLOGPP_CONSTANT_H_
