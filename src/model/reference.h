#ifndef GOLOGPP_REFERENCE_H_
#define GOLOGPP_REFERENCE_H_

#include "language.h"
#include "expressions.h"
#include "utilities.h"
#include "gologpp.h"
#include "scope.h"
#include "error.h"
#include "variable.h"
#include "domain.h"

#include <memory>
#include <vector>
#include <boost/optional.hpp>

namespace gologpp {


class AbstractReference
: public virtual AbstractLanguageElement
{
public:
	AbstractReference();
	virtual ~AbstractReference();

	virtual bool bound() const = 0;
	virtual bool consistent() const = 0;
};



template<class TargetT>
class Reference
: public TargetT::expression_t
, public AbstractReference
, public NoScopeOwner
, public LanguageElement<Reference<TargetT>>
{
public:
	Reference(const shared_ptr<TargetT> &target, const vector<Expression *> &args = {})
	: args_(args.begin(), args.end())
	, target_(target)
	{}

	Reference(const string &target_name, const vector<Expression *> &args)
	: args_(args.begin(), args.end())
	, target_(global_scope().lookup_global<TargetT>(target_name, arity_t(args.size())))
	{}

	Reference(const string &target_name, const boost::optional<vector<Expression *>> &args)
	: Reference(target_name, args.get_value_or({}))
	{}

	Reference(Reference<TargetT> &&other)
	: TargetT::expression_t(other.parent_scope())
	, args_(std::move(other.args_))
	, target_(std::move(other.target_))
	{}


	virtual ~Reference() override = default;

	TargetT &operator * () const
	{ return target(); }

	TargetT *operator -> () const
	{ return &target(); }

	const string &name() const
	{ return target_.lock()->name(); }

	arity_t arity() const
	{ return target_.lock()->arity(); }

	virtual bool bound() const override
	{ return !target_.expired(); }

	const vector<unique_ptr<Expression>> &args() const
	{ return args_; }


	virtual bool consistent() const override
	{
		if (!bound() || args().size() != target()->args().size())
			return false;

		auto it_rarg = args().begin();
		auto it_targ = target()->args().begin();
		for (; it_rarg < args().end() && it_targ < target()->args().end(); ++it_rarg, ++it_targ) {
			if ((*it_rarg)->dynamic_type_tag() != (*it_targ)->dynamic_type_tag())
				return false;

			if ((*it_rarg)->is_ref() && !dynamic_cast<AbstractReference &>(**it_rarg).consistent())
				return false;
		}

		return true;
	}


	shared_ptr<TargetT> target()
	{ return target_.lock(); }

	shared_ptr<const TargetT> target() const
	{ return std::dynamic_pointer_cast<const TargetT>(target_.lock()); }

	virtual bool is_ref() const override
	{ return true; }


	virtual void attach_semantics(SemanticsFactory &implementor) override
	{
		if (semantics_)
			return;

		semantics_ = implementor.make_semantics(*this);
		for (unique_ptr<Expression> &expr : args_)
			expr->attach_semantics(implementor);
	}

	virtual string to_string(const string &pfx) const override
	{ return pfx + name() + '(' + concat_list(args(), ", ", "") + ')'; }


private:
	vector<unique_ptr<Expression>> args_;
	weak_ptr<TargetT> target_;
};



template<class ExprT>
class Reference<Variable<ExprT>>
: public ExprT
, public AbstractReference
, public NoScopeOwner
, public LanguageElement<Reference<Variable<ExprT>>>
{
public:
	Reference(const shared_ptr<Variable<ExprT>> &target)
	: target_(target)
	{}

	Reference(Reference<Variable<ExprT>> &&other)
	: AbstractReference(std::move(other.args_))
	, target_(std::move(other.target_))
	{}

	Reference(const string &var_name)
	: target_(global_scope().get_var<ExprT>(var_name))
	{}

	virtual ~Reference() override = default;

	Variable<ExprT> &operator * () const
	{ return target(); }

	Variable<ExprT> *operator -> () const
	{ return &target(); }

	const string &name() const
	{ return target_.lock()->name(); }

	virtual bool bound() const override
	{ return !target_.expired(); }

	shared_ptr<Variable<ExprT>> target()
	{ return target_.lock(); }

	shared_ptr<const Variable<ExprT>> target() const
	{ return std::dynamic_pointer_cast<const Variable<ExprT>>(target_.lock()); }

	virtual bool is_ref() const override
	{ return true; }


	virtual void attach_semantics(SemanticsFactory &implementor) override
	{
		if (semantics_)
			return;
		semantics_ = implementor.make_semantics(*this);
	}


	virtual bool consistent() const override
	{ return bound(); }


	virtual string to_string(const string &pfx) const override
	{ return target()->to_string(pfx); }


private:
	weak_ptr<Variable<ExprT>> target_;
};



} // namespace gologpp


#endif // GOLOGPP_REFERENCE_H_
