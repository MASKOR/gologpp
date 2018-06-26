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

namespace gologpp {


class AbstractReference {
public:
	AbstractReference(const Identifier &target_id)
	: target_id_(target_id)
	{}

	virtual ~AbstractReference();

	virtual void bind() = 0;
	virtual bool bound() = 0;

	const Identifier &target_id() const
	{ return target_id_; }

private:
	const Identifier target_id_;
};


template<class TargetT>
class Reference
: public TargetT::expression_t
, public AbstractReference
, public virtual AbstractLanguageElement
, public LanguageElement<Reference<TargetT>>
{
public:
	Reference(const shared_ptr<TargetT> &target, Scope &parent_scope, vector<unique_ptr<Expression>> &&args = {})
	: TargetT::expression_t(parent_scope)
	, AbstractReference(*target)
	, target_(target)
	, args_(std::move(args))
	{}

	Reference(Reference<TargetT> &&other)
	: TargetT::expression_t(other.parent_scope())
	, AbstractReference(other.target_id())
	, target_(std::move(other.target_))
	, args_(std::move(other.args_))
	{}

	Reference(const string &target_name, Scope &parent_scope, const vector<Expression *> &args)
	: TargetT::expression_t(parent_scope)
	, AbstractReference({target_name, static_cast<arity_t>(args.size())})
	, args_(args.begin(), args.end())
	{}

	virtual ~Reference() override = default;

	TargetT &operator * () const
	{ return target(); }

	TargetT *operator -> () const
	{ return &target(); }

	const vector<unique_ptr<Expression>> &args() const
	{ return args_; }

	const string &name() const
	{ return target_id().name(); }

	arity_t arity() const
	{ return target_id().arity(); }

	virtual bool bound() override
	{ return static_cast<bool>(target_); }


	TargetT &target()
	{
		if (!target_)
			bind();
		return *target_;
	}


	virtual void implement(Implementor &implementor) override
	{
		if (impl_)
			return;

		impl_ = implementor.make_impl(*this);
		for (unique_ptr<Expression> &expr : args_)
			expr->implement(implementor);
	}


	virtual void bind() override {
		for (unique_ptr<Expression> &arg : args_) {
			AbstractReference *uref = dynamic_cast<AbstractReference *>(arg.get());
			if (uref && !uref->bound())
				uref->bind();
		}

		target_ = global_scope().lookup_global<TargetT>(target_id());
	}

private:
	shared_ptr<TargetT> target_;
	vector<unique_ptr<Expression>> args_;
};


} // namespace gologpp


#endif // GOLOGPP_REFERENCE_H_
