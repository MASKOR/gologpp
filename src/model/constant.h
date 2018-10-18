#ifndef GOLOGPP_CONSTANT_H_
#define GOLOGPP_CONSTANT_H_

#include "gologpp.h"
#include "language.h"
#include "expressions.h"
#include "scope.h"
#include "error.h"

#include <boost/variant.hpp>


namespace gologpp {



class AbstractConstant : public virtual AbstractLanguageElement {
private:
	struct to_string_visitor;
public:
	typedef boost::variant<int, double, string, bool> LiteralVariant;

	template<class T>
	AbstractConstant(const T &representation)
	: representation_(representation)
	{}

	virtual ~AbstractConstant() override;

	virtual ExpressionTypeTag dynamic_type_tag() const = 0;

	template<class T>
	const T &representation() const
	{ return boost::get<T>(representation_); }

	const LiteralVariant &variant() const;

	size_t hash() const;

	template<class ExpressionT>
	operator Constant<ExpressionT> *() {
		return dynamic_cast<Constant<ExpressionT> *>(this);
	}

	template<class ExpressionT>
	operator ExpressionT *() {
		return static_cast<ExpressionT *>(dynamic_cast<Constant<ExpressionT> *>(this));
	}

	virtual string to_string(const string &) const override
	{ return boost::apply_visitor(to_string_visitor(), representation_); }

	virtual bool operator == (const AbstractConstant &) const = 0;

	virtual AbstractConstant *copy() const = 0;

protected:
	const LiteralVariant representation_;

	AbstractConstant(const LiteralVariant &v)
	: representation_(v)
	{}

private:
	struct to_string_visitor {
		string operator() (const string &s) const
		{ return s; }

		template<class T>
		string operator() (const T &o) const
		{ return std::to_string(o); }
	};
};



template<class ExpressionT>
class Constant
: public ExpressionT
, public AbstractConstant
, public NoScopeOwner
, public LanguageElement<Constant<ExpressionT>>
{
public:
	template<class T>
	Constant(T repr);

	Constant(Constant<ExpressionT> &&) = default;

	Constant(const Constant<ExpressionT> &c)
	: AbstractConstant(c.variant())
	{
		if (semantics_)
			throw Bug("Copying a Constant after Semantics have been assigned is forbidden");
	}

	Constant<ExpressionT> &operator = (Constant<ExpressionT> &&) = default;

	Constant<ExpressionT> &operator = (const Constant<ExpressionT> &c)
	{
		if (semantics_)
			throw Bug("Copying a Constant after Semantics have been assigned is forbidden");
		representation_ = c.representation_;
		return *this;
	}

	virtual ~Constant() override = default;

	virtual ExpressionTypeTag dynamic_type_tag() const override
	{ return ExpressionT::dynamic_type_tag(); }

	virtual bool operator == (const AbstractConstant &c) const override
	{
		if (dynamic_type_tag() != c.dynamic_type_tag())
			return false;
		else
			return variant() == c.variant();
	}

	virtual Constant<ExpressionT> *copy() const override
	{ return new Constant(*this); }

	DEFINE_IMPLEMENT
};


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
