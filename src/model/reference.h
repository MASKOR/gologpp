#ifndef GOLOGPP_REFERENCE_H_
#define GOLOGPP_REFERENCE_H_

#include "language.h"
#include "expressions.h"
#include "utilities.h"
#include "gologpp.h"

#include <memory>
#include <vector>

namespace gologpp {


template<class GologT>
class Reference : public virtual AbstractLanguageElement, public LanguageElement<Reference<GologT>>, public GologT::expression_t {
public:
	Reference(const shared_ptr<GologT> &target, vector<unique_ptr<Expression>> &&args, Scope &parent_scope)
	: GologT::expression_t(parent_scope)
	, target_id_(dynamic_cast<Global &>(*target))
	, target_(target)
	, args_(std::move(args))
	{}

	Reference(Reference<GologT> &&other)
	: GologT::expression_t(other.parent_scope())
	, target_id_(std::move(other.target_id_))
	, target_(std::move(other.target_))
	, args_(std::move(other.args_))
	{}

	Reference(const Identifier target_id, Scope &parent_scope, vector<unique_ptr<Expression>> &&args = {})
	: GologT::expression_t(parent_scope)
	, target_id_(target_id)
	, args_(std::move(args))
	{}

	Reference(const string target_name, Scope &parent_scope, const vector<Expression *> &args)
	: GologT::expression_t(parent_scope)
	, target_id_(target_name, arity_t(args.size()))
	, args_(args.begin(), args.end())
	{
	}

	virtual ~Reference() = default;

	GologT &operator * () const
	{ return *target_; }

	GologT *operator -> () const
	{ return target_.get(); }

	GologT &target() const
	{ return *target_; }

	const vector<unique_ptr<Expression>> &args() const
	{ return args_; }

	virtual void implement(Implementor &implementor) override
	{ implement_<GologT>(implementor); }

	template<class GologImplT>
	void implement_(Implementor &implementor)
	{
		if (impl_)
			return;

		impl_ = implementor.make_impl(*this);
		for (unique_ptr<Expression> &expr : args_)
			expr->implement(implementor);
	}

private:
	Identifier target_id_;
	shared_ptr<GologT> target_;
	vector<unique_ptr<Expression>> args_;
};


template<>
template<>
void Reference<BooleanExpression>::implement_<BooleanExpression>(Implementor &);


template<>
template<>
void Reference<NumericExpression>::implement_<NumericExpression>(Implementor &);


} // namespace gologpp


#endif // GOLOGPP_REFERENCE_H_
