#ifndef GOLOGPP_REFERENCE_H_
#define GOLOGPP_REFERENCE_H_

#include "atoms.h"
#include "Formula.h"
#include "utilities.h"

#include <memory>
#include <vector>

namespace gologpp {
namespace generic {

template<class GologT>
class Reference : public LanguageElement<Reference<GologT>>, public BooleanExpression {
public:
	Reference(const shared_ptr<GologT> &target, const vector<shared_ptr<Expression>> &args, Scope &parent_scope)
	: BooleanExpression(parent_scope)
	, target_(target)
	, args_(args)
	{}

	Reference(const shared_ptr<GologT> &target, const vector<string> &args, Scope &parent_scope)
	: BooleanExpression(parent_scope)
	, target_(target)
	, args_(parent_scope.variables(args))
	{}

	Reference(Reference<GologT> &&other)
	: BooleanExpression(other.parent_scope())
	, target_(std::move(other.target_))
	, args_(std::move(other.args_))
	{}

	virtual ~Reference() = default;

	GologT &operator * () const
	{ return *target_; }

	GologT *operator -> () const
	{ return target_.get(); }

	DEFINE_IMPLEMENT

private:
	shared_ptr<GologT> target_;
	vector<shared_ptr<Expression>> args_;
};



} // namespace generic
} // namespace gologpp


#endif // GOLOGPP_REFERENCE_H_
