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
#include <boost/numeric/conversion/cast.hpp>
#include <unordered_map>


namespace gologpp {


template<>
class unique_ptr<Value> : public std::unique_ptr<Value> {
public:
	using std::unique_ptr<Value>::unique_ptr;

	unique_ptr()
	: std::unique_ptr<Value>()
	{}

	unique_ptr(const unique_ptr<Value> &c);
	unique_ptr(const Value &v);

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
	explicit Value(const Type &type, ReprT repr) : representation_(repr) {
		set_type(type);
	}

	Value(const Type &type, const vector<fusion_wtf_vector<string, Value *>> &compound_values);
	Value(const Type &type, const boost::optional<vector<Value *>> &list_values);
	Value(Value &&c);
	Value(const Value &c);
	explicit Value();

	Value &operator = (Value &&);
	Value &operator = (const Value &c);

	virtual ~Value() override;

	virtual size_t hash() const;

	explicit operator int () const
	{ return boost::get<int>(representation()); }

	explicit operator unsigned int () const
	{ return boost::get<unsigned int>(representation()); }

	explicit operator long () const
	{ return boost::get<long>(representation()); }

	explicit operator unsigned long () const
	{ return boost::get<unsigned long>(representation()); }

	explicit operator double () const
	{ return boost::get<double>(representation()); }

	explicit operator string () const
	{
		try {
			return boost::get<string>(representation());
		} catch (boost::bad_get &) {
			return string_representation();
		}
	}

	explicit operator bool () const
	{ return boost::get<bool>(representation()); }

	explicit operator const CompoundType::Representation &() const
	{ return boost::get<CompoundType::Representation>(representation()); }

	explicit operator const ListType::Representation &() const
	{ return boost::get<ListType::Representation>(representation()); }

	explicit operator void *() const
	{ return boost::get<void *>(representation()); }

	Representation &representation()
	{ return representation_; }

	const Representation &representation() const
	{ return representation_; }

	/// @return A string representation for software consumption.
	///         Only difference to str() is that strings remain unquoted.
	string string_representation() const;

	template<typename NumericT>
	NumericT numeric_convert() const;

	virtual string to_string(const string &) const override;

	virtual bool operator == (const Expression &c) const override;
	virtual bool operator != (const Expression &c) const override;

	bool operator >= (const Value &) const;
	bool operator <= (const Value &) const;
	bool operator > (const Value &) const;
	bool operator < (const Value &) const;

	bool operator <= (const Type &) const override;

	Value operator ! () const;

	Value operator + (const Value &) const;
	Value operator - (const Value &) const;
	Value operator * (const Value &) const;
	Value operator / (const Value &) const;
	Value operator % (const Value &) const;
	Value pow(const Value &) const;

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


template<typename NumericT>
NumericT Value::numeric_convert() const
{
	using namespace boost::typeindex;
	if (!this->type().is<NumberType>())
		throw TypeError(*this, get_type<NumberType>());

	if (this->representation().type() == type_id<unsigned int>())
		return boost::numeric_cast<NumericT>(static_cast<unsigned int>(*this));
	else if (this->representation().type() == type_id<int>())
		return boost::numeric_cast<NumericT>(static_cast<int>(*this));
	else if (this->representation().type() == type_id<unsigned long>())
		return boost::numeric_cast<NumericT>(static_cast<unsigned long>(*this));
	else if (this->representation().type() == type_id<long>())
		return boost::numeric_cast<NumericT>(static_cast<long>(*this));
	else if (this->representation().type() == type_id<double>())
		return boost::numeric_cast<NumericT>(static_cast<double>(*this));
	else
		throw std::runtime_error("Unknown numeric Value type");
}


template<>
Value::Value(const Type &type, const char *repr);



template<>
class GeneralSemantics<Value>
: public virtual GeneralSemantics<Expression> {
public:
	GeneralSemantics(const Value &elem, AExecutionController &context);

	virtual ~GeneralSemantics<Value>() = default;

	virtual AExecutionController &context() const override;

	virtual Value evaluate(const BindingChain &, const History &) override;
	virtual GeneralSemantics<Value> *copy(const Value &target_element) const = 0;

	virtual const Expression &expression() const override;
	virtual const ModelElement &model_element() const override;
	const Value &element() const;
	void update_element(const Value *);

private:
	const Value *expression_;
	AExecutionController &context_;
};


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

template<>
struct hash<gologpp::Value *> {
	size_t operator () (const gologpp::Value *o) const
	{ return o->hash(); }
};

template<>
struct equal_to<gologpp::Value *> {
	bool operator () (const gologpp::Value *lhs, const gologpp::Value *rhs) const
	{ return *lhs == *rhs; }
};

} // namespace std

#endif //GOLOGPP_CONSTANT_H_
