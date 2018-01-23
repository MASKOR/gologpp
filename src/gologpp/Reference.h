#ifndef GOLOGPP_REFERENCE_H_
#define GOLOGPP_REFERENCE_H_

#include "atoms.h"
#include "Formula.h"

#include <memory>
#include <vector>

namespace gologpp {
namespace generic {

using namespace std;

template<class GologT>
class Reference : public virtual LanguageElement<Reference<GologT>>{
public:
	Reference(const shared_ptr<GologT> &target, const vector<shared_ptr<Expression>> &args, const shared_ptr<Scope> &parent_scope)
	: target_(target)
	, args_(args)
	, scope_(parent_scope)
	{}

	Reference(Reference<GologT> &&other)
	: target_(std::move(other.target_))
	, args_(std::move(other.args_))
	, scope_(std::move(other.scope_))
	{}

	virtual ~Reference()
	{}

	GologT &operator * ()
	{ return *target_; }

	const GologT &operator * () const
	{ return *target_; }

private:
	shared_ptr<GologT> target_;
	vector<shared_ptr<Expression>> args_;
	shared_ptr<Scope> scope_;
};


class Fluent;


class FluentReference : public Reference<Fluent>, public BooleanExpression {
public:
	using Reference<Fluent>::Reference;
	FluentReference(const shared_ptr<Fluent> &target, const vector<shared_ptr<Expression>> &args, const shared_ptr<Scope> &parent_scope)
	: Reference<Fluent>(target, args, parent_scope)
	, BooleanExpression(parent_scope)
	{}
};


} // namespace generic
} // namespace gologpp


#endif // GOLOGPP_REFERENCE_H_