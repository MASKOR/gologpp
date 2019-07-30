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
};



class Value
: public Expression
, public NoScopeOwner
, public LanguageElement<Value>
{
public:
	using LiteralVariant = boost::variant<int, long, double, string, bool, CompoundType::Representation>;

	template<class ReprT>
	Value(const string &type_name, ReprT repr);

	Value(const string &type_name, const vector<fusion_wtf_vector<string, Value *>> &compound_values);
	Value(Value &&c);
	Value(const Value &c);

	Value &operator = (Value &&);
	Value &operator = (const Value &c);

	virtual ~Value() override;

	virtual size_t hash() const;

	operator int () const
	{ return boost::get<int>(representation()); }

	operator long () const
	{ return boost::get<long>(representation()); }

	operator double () const
	{ return boost::get<double>(representation()); }

	operator string () const
	{ return boost::get<string>(representation()); }

	operator bool () const
	{ return boost::get<bool>(representation()); }

	operator CompoundType::Representation () const
	{ return boost::get<CompoundType::Representation>(representation()); }

	Representation &representation()
	{ return representation_; }

	const Representation &representation() const
	{ return representation_; }

	virtual string to_string(const string &) const override;

	virtual bool operator == (const Value &c) const;

	bool operator != (const Value &) const;
	virtual Value *copy() const;

	virtual void attach_semantics(SemanticsFactory &f) override;

protected:
	Value(Representation &&repr);

private:
	Representation representation_;

	struct to_string_visitor;
	struct hash_visitor;
	struct attach_semantics_visitor;
};



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
