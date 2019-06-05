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
class unique_ptr<Constant> : public std::unique_ptr<Constant> {
public:
	using std::unique_ptr<Constant>::unique_ptr;

	unique_ptr()
	: std::unique_ptr<Constant>()
	{}

	unique_ptr(const unique_ptr<Constant> &c);

	unique_ptr<Constant> &operator = (const unique_ptr<Constant> &c);
};



class Constant
: public Expression
, public NoScopeOwner
, public LanguageElement<Constant>
{
public:
	using LiteralVariant = boost::variant<int, long, double, string, bool, CompoundType::Representation>;

	template<class ReprT>
	Constant(const string &type_name, ReprT repr);

	Constant(const string &type_name, const vector<fusion_wtf_vector<string, Constant *>> &definition);
	Constant(Constant &&c);
	Constant(const Constant &c);

	Constant &operator = (Constant &&);
	Constant &operator = (const Constant &c);

	virtual ~Constant() override;

	virtual size_t hash() const;

	template<class T>
	operator T () const
	{ return boost::get<T>(variant()); }

	LiteralVariant &variant()
	{ return representation_; }

	const LiteralVariant &variant() const
	{ return representation_; }

	virtual string to_string(const string &) const override;

	virtual bool operator == (const Constant &c) const;

	bool operator != (const Constant &) const;
	virtual Constant *copy() const;

	virtual void attach_semantics(SemanticsFactory &f) override;

protected:
	Constant(LiteralVariant &&repr);

private:
	LiteralVariant representation_;

	struct to_string_visitor;
	struct hash_visitor;
	struct attach_semantics_visitor;
};



vector<unique_ptr<Constant>> copy(const vector<unique_ptr<Constant>> &v);



} // namespace gologpp



namespace std {

template<>
struct hash<gologpp::unique_ptr<gologpp::Constant>> {
	size_t operator () (const gologpp::unique_ptr<gologpp::Constant> &o) const
	{ return o->hash(); }
};

template<>
struct equal_to<gologpp::unique_ptr<gologpp::Constant>> {
	bool operator () (const gologpp::unique_ptr<gologpp::Constant> &lhs, const gologpp::unique_ptr<gologpp::Constant> &rhs) const
	{ return *lhs == *rhs; }
};

} // namespace std

#endif //GOLOGPP_CONSTANT_H_
