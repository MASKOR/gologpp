/*************************************************************************
 * This file is part of golog++.
 *
 * golog++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * golog++ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with golog++.  If not, see <https://www.gnu.org/licenses/>.
**************************************************************************/

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

	void ensure_consistent();
};


template<class ArgsT>
using ParamsToArgs = std::unordered_map<
	shared_ptr<const Variable>,
	std::reference_wrapper<ArgsT>
>;


template<class TargetT, class ArgsT>
class ReferenceBase
: public virtual AbstractReference
, public NoScopeOwner
{
public:
	ReferenceBase(const shared_ptr<TargetT> &target, vector<unique_ptr<ArgsT>> &&args)
	: args_(std::move(args))
	, target_(target)
	{
		size_t idx = 0;
		while (idx < this->args().size() && idx < this->target()->params().size()) {
			ArgsT &arg = *this->args()[idx];
			shared_ptr<Variable> param = this->target()->params()[idx];
			params_to_args_.insert( { param, arg } );
			dynamic_cast<Expression &>(arg).set_parent(this);
			++idx;
		}
		ensure_consistent();
	}

	ReferenceBase(const shared_ptr<TargetT> &target, std::unordered_map< Reference<Variable>, unique_ptr<ArgsT> > params_to_args)
	: target_(target)
	{
		args_.resize(params_to_args.size());
		for (auto it = params_to_args.begin(); it != params_to_args.end(); it++){

			auto target_it = std::find(this->target()->params().begin(), this->target()->params().end(), it->first.target());
			int pos_in_target = std::distance(this->target()->params().begin(), target_it);
			args_[pos_in_target] = std::move(it->second);
			std::reference_wrapper<ArgsT> var = std::ref((*args_[pos_in_target]));
			params_to_args_.insert(std::make_pair(
				it->first.target()
				, var
			));
		}
		ensure_consistent();
	}

	ReferenceBase(const shared_ptr<TargetT> &target, const vector<ArgsT *> &args)
	: ReferenceBase(target, vector<unique_ptr<ArgsT>>(args.begin(), args.end()))
	{}


	ReferenceBase(const string &target_name, const vector<ArgsT *> &args)
	: ReferenceBase(
		global_scope().lookup_global<TargetT>(target_name, arity_t(args.size())),
		args
	)
	{}

	ReferenceBase(const string &target_name, const boost::optional<vector<ArgsT *>> &args)
	: ReferenceBase(target_name, args.get_value_or({}))
	{}

	ReferenceBase(ReferenceBase<TargetT, ArgsT> &&other)
	: args_(std::move(other.args_))
	, target_(std::move(other.target_))
	{ ensure_consistent(); }


	virtual ~ReferenceBase() override = default;

	TargetT &operator * () const
	{ return target(); }

	TargetT *operator -> () const
	{ return &target(); }

	bool operator == (const ReferenceBase<TargetT, ArgsT> &other) const
	{
		if (this->target() != other.target())
			return false;
		for (arity_t i = 0; i < this->target()->arity(); ++i) {
			if (*this->args()[i] != *other.args()[i])
				return false;
		}
		return true;
	}

	bool operator != (const ReferenceBase<TargetT, ArgsT> &other) const
	{ return !(*this == other); }

	const string &name() const
	{ return target_.lock()->name(); }

	arity_t arity() const
	{ return target_.lock()->arity(); }

	virtual bool bound() const override
	{ return !target_.expired(); }

	const vector<unique_ptr<ArgsT>> &args() const
	{ return args_; }

	vector<unique_ptr<ArgsT>> &args()
	{ return args_; }


	const ArgsT &arg_for_param(shared_ptr<const Variable> param) const
	{
		auto it = params_to_args_.find(param);
		if (it == params_to_args_.end())
			throw Bug(target()->str() + " has no parameter by the name " + param->str());

		return it->second;
	}


	virtual bool consistent() const override
	{
		if (!bound() || args().size() != target()->params().size())
			return false;

		// Compare target argument types with this reference's argument types
		auto it_rarg = args().begin();
		auto it_targ = target()->params().begin();
		for (; it_rarg < args().end() && it_targ < target()->params().end(); ++it_rarg, ++it_targ) {
			const Type &t_ref = (*it_rarg)->type();
			const Type &t_tgt = (*it_targ)->type();
			if (!(t_tgt >= **it_rarg)
				&& !(t_ref.is<SymbolType>() && t_tgt.is<StringType>())
				// TODO: Hack: Allow passing a symbol value to a string argument
				// This is needed because ReadyLog can't deal with strings.
			)
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

	virtual string to_string(const string &pfx) const override
	{ return pfx + name() + '(' + concat_list(args(), ", ", "") + ')'; }

	virtual const Type &type() const override
	{ return this->target()->type(); }

	size_t hash() const
	{
		size_t rv = this->target()->hash();
		for (const unique_ptr<ArgsT> &c : this->args())
			boost::hash_combine(rv, c->hash());

		return rv;
	}

	virtual ParamsToArgs<ArgsT> &params_to_args()
	{ return params_to_args_; }

	virtual const ParamsToArgs<ArgsT> &params_to_args() const
	{ return params_to_args_; }

private:
	vector<unique_ptr<ArgsT>> args_;
	weak_ptr<TargetT> target_;

	ParamsToArgs<ArgsT> params_to_args_;
};



template<class TargetT>
class Reference
: public ReferenceBase<TargetT, Expression>
, public LanguageElement<Reference<TargetT>>
, public TargetT::SignifierT
{
public:
	using ReferenceBase<TargetT, Expression>::ReferenceBase;
	virtual ~Reference() override = default;

	virtual void attach_semantics(SemanticsFactory &f) override
	{
		if (!this->semantics_) {
			this->semantics_ = f.make_semantics(*this);
			for (auto &arg : this->args())
				arg->attach_semantics(f);
		}
	}
};


template<class> class Grounding;

class AbstractGrounding
: public virtual AbstractReference
{
public:
	virtual const ParamsToArgs<Value> &params_to_args() const = 0;
	virtual ParamsToArgs<Value> &params_to_args() = 0;
};



template<>
class Grounding<AbstractAction>
: public virtual AbstractReference
, public AbstractGrounding
{};



template<class TargetT>
class Grounding
: public std::conditional<
	std::is_base_of<AbstractAction, TargetT>::value,
	Grounding<AbstractAction>, // Have a specific superclass for all Action groundings
	                           // (used for the exogenous event queue)
	AbstractGrounding          // All others use some placeholder
  >::type
, public ReferenceBase<TargetT, Value>
{
public:
	using ReferenceBase<TargetT, Value>::ReferenceBase;

	Grounding(const Grounding<TargetT> &other)
	: ReferenceBase<TargetT, Value>(other.target(), copy(other.args()))
	{}

	virtual ~Grounding() override = default;


	virtual Scope &parent_scope() override
	{ return global_scope(); }

	virtual const Scope &parent_scope() const override
	{ return global_scope(); }

	virtual const ParamsToArgs<Value> &params_to_args() const override
	{ return ReferenceBase<TargetT, Value>::params_to_args(); }

	virtual ParamsToArgs<Value> &params_to_args() override
	{ return ReferenceBase<TargetT, Value>::params_to_args(); }
};



template<>
class Reference<Variable>
: public Expression
, public AbstractReference
, public NoScopeOwner
, public LanguageElement<Reference<Variable>>
{
public:
	Reference(const shared_ptr<Variable> &target)
	: target_(target)
	{}

	Reference(Reference<Variable> &&other)
	: target_(std::move(other.target_))
	{}

	virtual ~Reference() override = default;

	const Variable &operator * () const
	{ return *target(); }

	const Variable *operator -> () const
	{ return target().get(); }

	const string &name() const
	{ return target()->name(); }

	virtual bool bound() const override
	{ return target_.get(); }

	shared_ptr<Variable> target()
	{ return target_; }

	shared_ptr<const Variable> target() const
	{ return std::dynamic_pointer_cast<const Variable>(target_); }

	bool operator == (const Reference<Variable> &other) const
	{ return *target() == *other.target(); }

	bool operator != (const Reference<Variable> &other) const
	{ return !(*this == other); }


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

	virtual const Type &type() const override
	{ return target()->type(); }

	size_t hash() const
	{ return target()->hash(); }


private:
	shared_ptr<Variable> target_;
};



} // namespace gologpp



namespace std {


template<class TargetT>
struct hash<gologpp::Reference<TargetT>> {
	size_t operator () (const gologpp::Reference<TargetT> &o) const
	{ return o.hash(); }
};

template<class TargetT>
struct hash<gologpp::Reference<TargetT> *> {
	size_t operator () (const gologpp::Reference<TargetT> *o) const
	{ return o->hash(); }
};

template<class TargetT>
struct hash<gologpp::unique_ptr<gologpp::Reference<TargetT>>> {
	size_t operator () (const gologpp::unique_ptr<gologpp::Reference<TargetT>> &o) const
	{ return o->hash(); }
};

template<class TargetT>
struct hash<gologpp::shared_ptr<gologpp::Reference<TargetT>>> {
	size_t operator () (const gologpp::shared_ptr<gologpp::Reference<TargetT>> &o) const
	{ return o->hash(); }
};

template<class TargetT>
struct equal_to<gologpp::unique_ptr<gologpp::Reference<TargetT>>> {
	bool operator () (
		const gologpp::unique_ptr<gologpp::Reference<TargetT>> &lhs,
		const gologpp::unique_ptr<gologpp::Reference<TargetT>> &rhs
	) const {
		return *lhs == *rhs;
	}
};

template<class TargetT>
struct equal_to<gologpp::shared_ptr<gologpp::Reference<TargetT>>> {
	bool operator () (
		const gologpp::shared_ptr<gologpp::Reference<TargetT>> &lhs,
		const gologpp::shared_ptr<gologpp::Reference<TargetT>> &rhs
	) const {
		return *lhs == *rhs;
	}
};

template<class TargetT>
struct equal_to<gologpp::Reference<TargetT> *> {
	bool operator () (
		const gologpp::Reference<TargetT> *lhs,
		const gologpp::Reference<TargetT> *rhs
	) const {
		return *lhs == *rhs;
	}
};


template<class TargetT>
struct hash<gologpp::Grounding<TargetT>> {
	size_t operator () (const gologpp::Grounding<TargetT> &o) const
	{ return o.hash(); }
};

template<class TargetT>
struct hash<gologpp::unique_ptr<gologpp::Grounding<TargetT>>> {
	size_t operator () (const gologpp::unique_ptr<gologpp::Grounding<TargetT>> &o) const
	{ return o->hash(); }
};

template<class TargetT>
struct hash<gologpp::shared_ptr<gologpp::Grounding<TargetT>>> {
	size_t operator () (const gologpp::shared_ptr<gologpp::Grounding<TargetT>> &o) const
	{ return o->hash(); }
};


template<class TargetT>
struct equal_to<gologpp::unique_ptr<gologpp::Grounding<TargetT>>> {
	bool operator () (
		const gologpp::unique_ptr<gologpp::Grounding<TargetT>> &lhs,
		const gologpp::unique_ptr<gologpp::Grounding<TargetT>> &rhs
	) const {
		return *lhs == *rhs;
	}
};

template<class TargetT>
struct equal_to<gologpp::shared_ptr<gologpp::Grounding<TargetT>>> {
	bool operator () (
		const gologpp::shared_ptr<gologpp::Grounding<TargetT>> &lhs,
		const gologpp::shared_ptr<gologpp::Grounding<TargetT>> &rhs
	) const {
		return *lhs == *rhs;
	}
};


}

#endif // GOLOGPP_REFERENCE_H_
