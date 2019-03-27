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


template<class PodT>
struct pod_type {
};

template<>
struct pod_type<int> {
	typedef Constant<NumericExpression> constant_type;
};

template<>
struct pod_type<long> {
	typedef Constant<NumericExpression> constant_type;
};

template<>
struct pod_type<double> {
	typedef Constant<NumericExpression> constant_type;
};

template<>
struct pod_type<string> {
	typedef Constant<StringExpression> constant_type;
};

template<>
struct pod_type<bool> {
	typedef Constant<BooleanExpression> constant_type;
};



class AbstractConstant : public virtual Expression {
public:
	virtual ~AbstractConstant() override;

	virtual size_t hash() const = 0;

	template<class ExpressionT>
	operator Constant<ExpressionT> *() {
		return dynamic_cast<Constant<ExpressionT> *>(this);
	}

	template<class ExpressionT>
	operator ExpressionT *() {
		return static_cast<ExpressionT *>(dynamic_cast<Constant<ExpressionT> *>(this));
	}

	template<class PodT>
	operator PodT () {
		return dynamic_cast<typename pod_type<PodT>::constant_type *>(this)
			->template representation<PodT>();
	}


	virtual bool operator == (const AbstractConstant &) const = 0;
	bool operator != (const AbstractConstant &) const;

	virtual AbstractConstant *copy() const = 0;
};



class SimpleConstant : public AbstractConstant {
public:
	using LiteralVariant = boost::variant<int, long, double, string, bool>;

	virtual size_t hash() const override;

	template<class T>
	const T &representation() const
	{ return boost::get<T>(variant()); }

	LiteralVariant &variant()
	{ return representation_; }

	const LiteralVariant &variant() const
	{ return representation_; }

	virtual string to_string(const string &) const override
	{ return boost::apply_visitor(to_string_visitor(), representation_); }

protected:
	SimpleConstant(const LiteralVariant &repr);
	SimpleConstant(LiteralVariant &&repr);

private:
	LiteralVariant representation_;

	struct to_string_visitor {
		string operator() (const string &s) const
		{ return s; }

		string operator() (bool b) const
		{ return b ? "true" : "false"; }

		template<class T>
		string operator() (const T &o) const
		{ return std::to_string(o); }
	};
};



template<class ExpressionT>
class Constant
: public ExpressionT
, public SimpleConstant
, public NoScopeOwner
, public LanguageElement<Constant<ExpressionT>>
{
private:
	struct to_string_visitor;
public:
	template<class ReprT>
	Constant(ReprT repr);

	Constant(Constant<ExpressionT> &&c)
	: SimpleConstant(std::move(c.representation_))
	{
		semantics_ = std::move(c.semantics_);
		type_ = std::move(c.type_);
	}

	Constant(const Constant<ExpressionT> &c)
	: SimpleConstant(c.variant())
	{
		if (semantics_)
			throw Bug("Copying a Constant after Semantics have been assigned is forbidden");
	}

	Constant<ExpressionT> &operator = (Constant<ExpressionT> &&) = default;

	Constant<ExpressionT> &operator = (const Constant<ExpressionT> &c)
	{
		if (semantics_)
			throw Bug("Copying a Constant after Semantics have been assigned is forbidden");
		variant() = c.variant();
		return *this;
	}

	virtual ~Constant() override = default;

	virtual ExpressionTypeTag dynamic_type_tag() const override
	{ return this->type().dynamic_tag(); }

	virtual bool operator == (const AbstractConstant &c) const override
	{
		if (this->type() != c.type())
			return false;
		else
			return variant() == dynamic_cast<const Constant<ExpressionT> &>(c).variant();
	}

	virtual Constant<ExpressionT> *copy() const override
	{ return new Constant(*this); }

	DEFINE_IMPLEMENT

};


template<class T1, class T2>
#ifdef BOOST_FUSION_HAS_VARIADIC_VECTOR
using fusion_wtf_vector = boost::fusion::vector<T1, T2>;
#else
using fusion_wtf_vector = boost::fusion::vector2<T1, T2>;
#endif


template<>
class Constant<CompoundExpression>
: public CompoundExpression
, public AbstractConstant
, public NoScopeOwner
, public LanguageElement<Constant<CompoundExpression>>
{
public:
	using Value = std::unordered_map<string, unique_ptr<AbstractConstant>>;

	Constant(const string &type_name, const vector<fusion_wtf_vector<string, AbstractConstant *>> &repr);
	Constant(Constant<CompoundExpression> &&);
	Constant(const Constant<CompoundExpression> &);

	virtual ~Constant() override = default;

	Constant<CompoundExpression> &operator = (Constant<CompoundExpression> &&);
	Constant<CompoundExpression> &operator = (const Constant<CompoundExpression> &c);

	virtual size_t hash() const override;
	const Value &value() const;

	virtual ExpressionTypeTag dynamic_type_tag() const override;
	virtual bool operator == (const AbstractConstant &c) const override;

	virtual Constant<CompoundExpression> *copy() const override;

	virtual string to_string(const string &pfx) const override;


	DEFINE_IMPLEMENT

private:
	Value representation_;
};


vector<unique_ptr<AbstractConstant>> copy(const vector<unique_ptr<AbstractConstant>> &v);



} // namespace gologpp



namespace std {

template<class ExprT>
struct hash<unique_ptr<gologpp::Constant<ExprT>>> {
	size_t operator () (const unique_ptr<gologpp::Constant<ExprT>> &o) const
	{ return o->hash(); }
};

template<class ExprT>
struct equal_to<unique_ptr<gologpp::Constant<ExprT>>> {
	bool operator () (const unique_ptr<gologpp::Constant<ExprT>> &lhs, const unique_ptr<gologpp::Constant<ExprT>> &rhs) const
	{ return *lhs == *rhs; }
};

} // namespace std

#endif //GOLOGPP_CONSTANT_H_
