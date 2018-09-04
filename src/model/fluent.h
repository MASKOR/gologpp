#ifndef GOLOGPP_FLUENT_H_
#define GOLOGPP_FLUENT_H_

#include <string>
#include <memory>
#include <algorithm>

#include <boost/optional.hpp>

#include "gologpp.h"
#include "atoms.h"
#include "utilities.h"
#include "reference.h"
#include "scope.h"
#include "arithmetic.h"
#include "error.h"

namespace gologpp {


class AbstractFluent
: public Global
, public ScopeOwner
, public virtual AbstractLanguageElement
{
public:
	typedef Expression expression_t;

	AbstractFluent(Scope *own_scope, const string &name, const vector<shared_ptr<AbstractVariable>> &args);
	AbstractFluent(AbstractFluent &&) = default;

	virtual ~AbstractFluent() override = default;
	virtual ExpressionTypeTag expression_type_tag() const = 0;

	virtual void compile(AExecutionContext &ctx) override;
};


template<class ExpressionT>
class Fluent
: public ExpressionT
, public AbstractFluent
, public LanguageElement<Fluent<ExpressionT>>
{
public:
	typedef ExpressionT expression_t;

	Fluent(Scope *own_scope, const string &name, const vector<shared_ptr<AbstractVariable>> &args, boost::optional<Constant<ExpressionT> *> init)
	: ExpressionT(Scope::global_scope())
	, AbstractFluent(own_scope, name, args)
	, initial_value_(init.get_value_or(nullptr))
	{}

	Fluent(Scope *own_scope, const string &name, const vector<shared_ptr<AbstractVariable>> &args)
	: Fluent(own_scope, name, args, boost::optional<Constant<ExpressionT> *>())
	{}


	const ExpressionT &initially()
	{ return *initial_value_; }

	virtual ExpressionTypeTag expression_type_tag() const override
	{ return ExpressionT::static_type_tag(); }

	void define(boost::optional<Constant<ExpressionT> *> initial_value)
	{ initial_value_.reset(initial_value.value()); }


	Fluent(Fluent &&) = default;

	virtual ~Fluent() override = default;

	DEFINE_IMPLEMENT_WITH_MEMBERS(*scope_, *initial_value_)

	virtual string to_string(const string &pfx) const override
	{
		return linesep + pfx + gologpp::to_string(expression_type_tag()) + "fluent " + name() + '('
			+ concat_list(args(), ", ", "") + ") {" + linesep
			+ pfx + "initially:" + linesep
			+ pfx + concat_list(initially(), ";" linesep + pfx, pfx) + linesep
			+ pfx + '}';
	}

private:
	unique_ptr<Constant<ExpressionT>> initial_value_;
};



} // namespace gologpp



#endif // GOLOGPP_FLUENT_H_
