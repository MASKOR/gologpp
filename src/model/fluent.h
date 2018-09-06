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


template<class ExpressionT>
class InitialValue
: public virtual AbstractLanguageElement
, public LanguageElement<InitialValue<ExpressionT>> {
public:
	InitialValue(vector<AbstractConstant *> args, Constant<ExpressionT> *value)
	: args_(args.begin(), args.end())
	, value_(value)
	{}

	const vector<unique_ptr<AbstractConstant>> &args() const
	{ return args_; }

	const Constant<ExpressionT> &value() const
	{ return *value_; }

	vector<unique_ptr<AbstractConstant>> &args()
	{ return args_; }

	Constant<ExpressionT> &value()
	{ return *value_; }


	virtual void implement(Implementor &implementor) override
	{
		if (impl_)
			return;
		value().implement(implementor);
		for (unique_ptr<AbstractConstant> &arg : args())
			arg->implement(implementor);

		impl_ = implementor.make_impl(*this);
	}


	virtual string to_string(const string &pfx) const override
	{ return '(' + concat_list(args(), ", ", pfx) + ") = " + value().to_string(pfx); }

	void set_fluent(Fluent<ExpressionT> *f)
	{ fluent_ = f; }

	Fluent<ExpressionT> *fluent()
	{ return fluent_; }

	const Fluent<ExpressionT> *fluent() const
	{ return fluent_; }

private:
	vector<unique_ptr<AbstractConstant>> args_;
	unique_ptr<Constant<ExpressionT>> value_;
	Fluent<ExpressionT> *fluent_;
};


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
: public AbstractFluent
, public LanguageElement<Fluent<ExpressionT>>
{
public:
	typedef ExpressionT expression_t;
	typedef AbstractFluent abstract_t;

	Fluent(Scope *own_scope, const string &name, const vector<shared_ptr<AbstractVariable>> &args)
	: AbstractFluent(own_scope, name, args)
	{}

	Fluent(Fluent &&) = default;

	virtual ~Fluent() override = default;

	virtual ExpressionTypeTag expression_type_tag() const override
	{ return ExpressionT::static_type_tag(); }

	const vector<unique_ptr<InitialValue<ExpressionT>>> &initially() const
	{ return initial_values_; }


	void define(const vector<InitialValue<ExpressionT> *> &initial_values)
	{
		// TODO: fail if already defined
		for (InitialValue<ExpressionT> *ival : initial_values) {
			for (arity_t arg_idx = 0; arg_idx < arity(); ++arg_idx)
				if (ival->args()[arg_idx]->expression_type_tag() != args()[arg_idx]->expression_type_tag())
					throw ExpressionTypeMismatch(
						dynamic_cast<const Expression &>(*args()[arg_idx]),
						dynamic_cast<const Expression &>(*ival->args()[arg_idx])
					);

			initial_values_.push_back(unique_ptr<InitialValue<ExpressionT>>(ival));
			ival->set_fluent(this);
		}
	}


	virtual void implement(Implementor &implementor) override
	{
		if (impl_)
			return;

		for (unique_ptr<InitialValue<ExpressionT>> &ival : initial_values_)
			ival->implement(implementor);
		scope_->implement(implementor);
		impl_ = implementor.make_impl(*this);
	}


	virtual string to_string(const string &pfx) const override
	{
		return linesep + pfx + gologpp::to_string(expression_type_tag()) + "fluent " + name() + '('
			+ concat_list(args(), ", ", "") + ") {" + linesep
			+ pfx + "initially:" + linesep
			+ pfx + concat_list(initially(), ";" linesep + pfx, pfx) + linesep
			+ pfx + '}';
	}

	virtual Expression *ref(Scope &parent_scope, const vector<Expression *> &args) override
	{ return make_reference<Fluent<ExpressionT>>(parent_scope, args); }

private:
	vector<unique_ptr<InitialValue<ExpressionT>>> initial_values_;
};



} // namespace gologpp



#endif // GOLOGPP_FLUENT_H_
