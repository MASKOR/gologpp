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
#include <boost/functional/hash.hpp>

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



template<class TargetT, class ExprT >
class ReferenceBase
: public TargetT::expression_t
, public AbstractReference
, public NoScopeOwner
{
public:
	ReferenceBase(const shared_ptr<TargetT> &target, vector<unique_ptr<ExprT>> &&args)
	: args_(std::move(args))
	, target_(target)
	{
		for (unique_ptr<ExprT> &a : args_)
			dynamic_cast<Expression *>(a.get())->set_parent(this);
	}


	ReferenceBase(const shared_ptr<TargetT> &target, const vector<ExprT *> &args)
	: ReferenceBase(target, vector<unique_ptr<ExprT>>(args.begin(), args.end()))
	{}


	ReferenceBase(const string &target_name, const vector<ExprT *> &args)
	: ReferenceBase(
		global_scope().lookup_global<TargetT>(target_name, arity_t(args.size())),
		args
	)
	{}

	ReferenceBase(const string &target_name, const boost::optional<vector<ExprT *>> &args)
	: ReferenceBase(target_name, args.get_value_or({}))
	{}

	ReferenceBase(ReferenceBase<TargetT, ExprT> &&other)
	: args_(std::move(other.args_))
	, target_(std::move(other.target_))
	{}


	virtual ~ReferenceBase() override = default;

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

	const vector<unique_ptr<ExprT>> &args() const
	{ return args_; }

	vector<unique_ptr<ExprT>> &args()
	{ return args_; }


	virtual bool consistent() const override
	{
		if (!bound() || args().size() != target()->args().size())
			return false;

		// Compare target argument types with this reference's argument types
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


	virtual void attach_semantics(SemanticsFactory &implementor) override
	{
		for (unique_ptr<ExprT> &expr : args_)
			expr->attach_semantics(implementor);
	}

	virtual string to_string(const string &pfx) const override
	{ return pfx + name() + '(' + concat_list(args(), ", ", "") + ')'; }


private:
	vector<unique_ptr<ExprT>> args_;
	weak_ptr<TargetT> target_;
};



template<class TargetT>
class Reference
: public ReferenceBase<TargetT, Expression>
, public LanguageElement<Reference<TargetT>>
{
public:
	using ReferenceBase<TargetT, Expression>::ReferenceBase;
	virtual ~Reference() override = default;

	virtual void attach_semantics(SemanticsFactory &f) override
	{
		if (!this->semantics_) {
			this->semantics_ = f.make_semantics(*this);
			ReferenceBase<TargetT, Expression>::attach_semantics(f);
		}
	}
};



template<class TargetT>
class Grounding
: public ReferenceBase<TargetT, AbstractConstant>
{
public:
	using ReferenceBase<TargetT, AbstractConstant>::ReferenceBase;

	Grounding(const Grounding<TargetT> &other)
	: ReferenceBase<TargetT, AbstractConstant>(other.target(), copy(other.args()))
	{}

	virtual ~Grounding() override = default;


	virtual Scope &parent_scope() override
	{ return global_scope(); }

	virtual const Scope &parent_scope() const override
	{ return global_scope(); }


	bool operator == (const Grounding<TargetT> &other) const
	{
		if (this->target() != other.target())
			return false;
		for (arity_t i = 0; i < this->target()->arity(); ++i) {
			if (*this->args()[i] != *other.args()[i])
				return false;
		}
		return true;
	}

	bool operator != (const Grounding<TargetT> &other) const
	{ return !(*this == other); }


	size_t hash() const
	{
		size_t rv = this->target()->hash();
		for (const unique_ptr<AbstractConstant> &c : this->args())
			boost::hash_combine(rv, c->hash());

		return rv;
	}


	virtual void attach_semantics(SemanticsFactory &f) override
	{
		if (!this->semantics_) {
			this->semantics_ = f.make_semantics(*this);
			ReferenceBase<TargetT, AbstractConstant>::attach_semantics(f);
		}
	}


	struct Hash {
		std::size_t operator () (const shared_ptr<Grounding<TargetT>> &t) const
		{ return t->hash(); }
	};


	struct Equals {
		bool operator () (
			const shared_ptr<Grounding<TargetT>> &lhs,
			const shared_ptr<Grounding<TargetT>> &rhs
		) const
		{ return *lhs == *rhs; }
	};
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
