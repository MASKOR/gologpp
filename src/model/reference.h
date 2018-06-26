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

	template<class GologT>
	Reference<GologT> bind();

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

	TargetT &target() const
	{
		if (!target_)
			bind();
		return *target_;
	}

	const vector<unique_ptr<Expression>> &args() const
	{ return args_; }

	virtual void implement(Implementor &implementor) override
	{
		if (impl_)
			return;

		impl_ = implementor.make_impl(*this);
		for (unique_ptr<Expression> &expr : args_)
			expr->implement(implementor);
	}

	const string &name() const
	{ return target_id().name(); }

	arity_t arity() const
	{ return target_id().arity(); }


	void bind() {
		shared_ptr<Global> target = global_scope().lookup_global(target_id());

		vector<unique_ptr<Expression>> bound_args;

		for (arity_t idx = 0; idx < arity(); ++idx) {

			AbstractReference *uref = dynamic_cast<AbstractReference *>(args_[idx].get());

			if (uref) {
				switch (target->argument(idx)->expression_type_tag()) {
				case ExpressionTypeTag::BOOLEAN_EXPRESSION:
					bound_args[idx] = unique_ptr<Expression>(
						new Reference<BooleanExpression>(uref->bind<BooleanExpression>())
					);
					break;
				case ExpressionTypeTag::VALUE_EXPRESSION:
					bound_args[idx] = unique_ptr<Expression>(
						new Reference<NumericExpression>(uref->bind<NumericExpression>())
					);
					break;
				case ExpressionTypeTag::STATEMENT:
					throw Bug(target->name() + ", argument #" + std::to_string(idx)
						+ " has expression type STATEMENT. This should have been impossible.");
					break;
				case ExpressionTypeTag::UNKNOWN:
					throw Bug(target->name() + ", argument #" + std::to_string(idx)
						+ " has expression type UNKNOWN. This should have been impossible.");
					break;

				// No default: since we want a compiler warning on unhandled values.
				}
			}
			else {
				std::swap(bound_args[idx], args_[idx]);
			}
		}

		target_ = std::dynamic_pointer_cast<TargetT>(target);
	}

private:
	shared_ptr<TargetT> target_;
	vector<unique_ptr<Expression>> args_;
};

/*
template<>
template<>
void Reference<BooleanExpression>::implement_<BooleanExpression>(Implementor &);


template<>
template<>
void Reference<NumericExpression>::implement_<NumericExpression>(Implementor &);
*/

} // namespace gologpp


#endif // GOLOGPP_REFERENCE_H_
