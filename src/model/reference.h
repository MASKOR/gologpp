#ifndef GOLOGPP_REFERENCE_H_
#define GOLOGPP_REFERENCE_H_

#include "language.h"
#include "expressions.h"
#include "atoms.h"
#include "utilities.h"
#include "gologpp.h"
#include "scope.h"
#include "error.h"

#include <memory>
#include <vector>
#include <boost/optional.hpp>

namespace gologpp {



template<class TargetT>
class Reference
: public TargetT::expression_t
, public virtual AbstractLanguageElement
, public LanguageElement<Reference<TargetT>>
{
public:
	Reference(const shared_ptr<TargetT> &target, Scope &parent_scope, vector<unique_ptr<Expression>> &&args = {})
	: TargetT::expression_t(parent_scope)
	, target_(target)
	, args_(std::move(args))
	{}

	Reference(Reference<TargetT> &&other)
	: TargetT::expression_t(other.parent_scope())
	, target_(std::move(other.target_))
	, args_(std::move(other.args_))
	{}

	Reference(const string &target_name, Scope &parent_scope, const vector<Expression *> &args)
	: TargetT::expression_t(parent_scope)
	, target_(parent_scope.lookup_global<TargetT>( { target_name, static_cast<arity_t>(args.size()) } ))
	, args_(args.begin(), args.end())
	{}

	Reference(const string &target_name, Scope &parent_scope, const boost::optional<vector<Expression *>> &args)
	: Reference(target_name, parent_scope, args.get_value_or({}))
	{}


	virtual ~Reference() override = default;

	TargetT &operator * () const
	{ return target(); }

	TargetT *operator -> () const
	{ return &target(); }

	const vector<unique_ptr<Expression>> &args() const
	{ return args_; }

	const string &name() const
	{ return std::dynamic_pointer_cast<Identifier>(target_.lock())->name(); }

	arity_t arity() const
	{ return std::dynamic_pointer_cast<Identifier>(target_.lock())->arity(); }

	virtual bool bound()
	{ return !target_.expired(); }

	shared_ptr<TargetT> target()
	{ return target_.lock(); }

	shared_ptr<const TargetT> target() const
	{ return std::dynamic_pointer_cast<const TargetT>(target_.lock()); }

	virtual bool is_ref() const override
	{ return true; }


	virtual void implement(Implementor &implementor) override
	{
		if (impl_)
			return;

		impl_ = implementor.make_impl(*this);
		for (unique_ptr<Expression> &expr : args_)
			expr->implement(implementor);
	}

	virtual string to_string(const string &pfx) const override
	{ return pfx + name() + '(' + concat_list(args(), ", ", "") + ')'; }


private:
	weak_ptr<TargetT> target_;
	vector<unique_ptr<Expression>> args_;
};


} // namespace gologpp


#endif // GOLOGPP_REFERENCE_H_
