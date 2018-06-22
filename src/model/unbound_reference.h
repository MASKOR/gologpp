#ifndef GOLOGPP_UNBOUND_REFERENCE_H_
#define GOLOGPP_UNBOUND_REFERENCE_H_

#include <vector>

#include "expressions.h"
#include "utilities.h"
#include "scope.h"
#include "gologpp.h"
#include "reference.h"
#include "error.h"

namespace gologpp {


template<class TargetT>
class UnboundReference : public TargetT::expression_t, public AbstractReference {
public:
	UnboundReference(const string &target_name, Scope &parent_scope, const vector<Expression *> &args)
	: TargetT::expression_t(parent_scope)
	, AbstractReference({target_name, static_cast<arity_t>(args.size())})
	, args_(args.begin(), args.end())
	{}

	template<class GologT = TargetT>
	Reference<GologT> bind() {
		shared_ptr<Global> target = global_scope().lookup_global(target_id());

		vector<unique_ptr<Expression>> bound_args;

		for (arity_t idx = 0; idx < target_id().arity(); ++idx) {

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

		return Reference<GologT>(std::dynamic_pointer_cast<GologT>(target), this->parent_scope(), std::move(bound_args));
	}

	virtual void implement(Implementor &) override
	{ throw Bug("Cannot implement an UnboundReference"); }

private:
	vector<unique_ptr<Expression>> args_;
};


}

#endif // GOLOGPP_UNBOUND_REFERENCE_H_
