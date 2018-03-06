#ifndef GOLOGPP_REFERENCE_H_
#define GOLOGPP_REFERENCE_H_

#include "Language.h"
#include "Scope.h"

#include <memory>
#include <vector>

namespace gologpp {

template<class GologT>
class Reference : public virtual AbstractLanguageElement, public LanguageElement<Reference<GologT>>, public GologT::expression_t {
public:
	Reference(const shared_ptr<GologT> &target, const vector<shared_ptr<Expression>> &args, Scope &parent_scope)
	: GologT::expression_t(parent_scope)
	, target_(target)
	, args_(args)
	{
		arity_t idx = 0;
		for (const shared_ptr<Expression> arg : args_) {
			if (arg->expression_type_tag() != target_->argument(idx++)->expression_type_tag()) {

			}
		}
	}

	Reference(Reference<GologT> &&other)
	: GologT::expression_t(other.parent_scope())
	, target_(std::move(other.target_))
	, args_(std::move(other.args_))
	{}

	virtual ~Reference() = default;

	GologT &operator * () const
	{ return *target_; }

	GologT *operator -> () const
	{ return target_.get(); }

	GologT &target() const
	{ return *target_; }

	const vector<shared_ptr<Expression>> &args() const
	{ return args_; }

	DEFINE_IMPLEMENT

private:
	shared_ptr<GologT> target_;
	vector<shared_ptr<Expression>> args_;
};



} // namespace gologpp


#endif // GOLOGPP_REFERENCE_H_
