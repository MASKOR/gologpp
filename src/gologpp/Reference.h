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
class Reference : public virtual LanguageElement<Reference<GologT>> {
public:
	Reference(const shared_ptr<GologT> &target, const vector<shared_ptr<Expression>> &args, Scope &parent_scope)
	: LanguageElement<Reference<GologT>>(*this)
	, target_(target)
	, args_(args)
	, scope_(parent_scope)
	{}

	Reference(Reference<GologT> &&other)
	: LanguageElement<Reference<GologT>>(*this)
	, target_(std::move(other.target_))
	, args_(std::move(other.args_))
	, scope_(other.scope_)
	{}

	virtual ~Reference() = default;

	GologT &operator * ()
	{ return *target_; }

	const GologT &operator * () const
	{ return *target_; }

	//virtual void init_impl(unique_ptr<Implementation<Reference<GologT>>> &impl) override;

private:
	shared_ptr<GologT> target_;
	vector<shared_ptr<Expression>> args_;
	Scope &scope_;
};


class Fluent;


template<>
class Reference<Fluent> : public BooleanExpression, public LanguageElement<Reference<Fluent>> {
public:
	Reference(const shared_ptr<Fluent> &target, const vector<shared_ptr<Expression>> &args, Scope &parent_scope)
	: BooleanExpression(parent_scope)
	, LanguageElement(*this)
	, target_(target)
	, args_(args)
	, scope_(parent_scope)
	{}

	Reference(Reference<Fluent> &&other)
	: BooleanExpression(other.parent_scope())
	, LanguageElement<Reference<Fluent>>(*this)
	, target_(std::move(other.target_))
	, args_(std::move(other.args_))
	, scope_(other.scope_)
	{}

	virtual ~Reference() = default;

	Fluent &operator * ()
	{ return *target_; }

	const Fluent &operator * () const
	{ return *target_; }

	//virtual void init_impl(unique_ptr<Implementation<FluentReference>> &impl) override;

private:
	shared_ptr<Fluent> target_;
	vector<shared_ptr<Expression>> args_;
	Scope &scope_;
};


} // namespace generic
} // namespace gologpp


#endif // GOLOGPP_REFERENCE_H_