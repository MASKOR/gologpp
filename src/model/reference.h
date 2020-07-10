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

#include "abstract_reference.h"
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


class ABinding : public ModelElement {
public:
	virtual Expression &get(shared_ptr<const Variable> param) const = 0;
};


template<class ExprT>
class Binding : public ABinding {
public:
	using MapT = std::unordered_map <
		shared_ptr<const Variable>,
		std::reference_wrapper<ExprT>
	>;

	Binding(const Binding<ExprT> &) = default;
	Binding() = default;

	virtual ~Binding() = default;

	void bind(shared_ptr<const Variable> var, ExprT &expr)
	{ var_bindings_.insert(std::make_pair(var, std::ref(expr))); }

	virtual ExprT &get(shared_ptr<const Variable> param) const override
	{
		auto it = var_bindings_.find(param);
		if (it == var_bindings_.end())
			throw Bug("No parameter by the name " + param->str());

		return it->second;
	}

	virtual void attach_semantics(SemanticsFactory &f) override
	{
		if (!semantics_) {
			set_semantics(f.make_semantics(*this));
			for (auto &entry : var_bindings_)
				entry.second.get().attach_semantics(f);
		}
	}

	const MapT &map() const
	{ return var_bindings_; }

	virtual string to_string(const string &pfx) const override
	{
		string rv;
		for (auto &entry : var_bindings_)
			rv += entry.first->to_string(pfx) + "=" + entry.second.get().to_string(pfx) + ", ";
		if (!rv.empty())
			rv = rv.substr(0, rv.length() - 2);
		return rv;
	}

private:
	MapT var_bindings_;
};



template<>
class GeneralSemantics<Binding<Value>>
: public virtual GeneralSemantics<ModelElement>
{
public:
	GeneralSemantics(const Binding<Value> &elem, ExecutionContext &context);

	virtual ~GeneralSemantics<Binding<Value>>() = default;

	const Binding<Value> &element() const;
	void update_element(const Binding<Value> *new_element);
	virtual ExecutionContext &context() const override;

	virtual GeneralSemantics<Binding<Value>> *copy(const Binding<Value> &target_element) const = 0;

private:
	const Binding<Value> *element_;
	ExecutionContext &context_;
};



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
			binding_.bind(param, arg);
			dynamic_cast<Expression &>(arg).set_parent(this);
			++idx;
		}
		ensure_consistent();
	}

	ReferenceBase(const shared_ptr<TargetT> &target, std::unordered_map< Reference<Variable>, unique_ptr<ArgsT> > params_to_args)
	: target_(target)
	{
		args_.resize(params_to_args.size());
		for (auto it = params_to_args.begin(); it != params_to_args.end(); it++) {
			auto target_it = std::find(this->target()->params().begin(), this->target()->params().end(), it->first.target());
			int pos_in_target = std::distance(this->target()->params().begin(), target_it);
			args_[pos_in_target] = std::move(it->second);
			std::reference_wrapper<ArgsT> var = std::ref((*args_[pos_in_target]));
			binding_.bind(it->first.target(), var);
		}
		ensure_consistent();
	}

	ReferenceBase(const shared_ptr<TargetT> &target, const vector<ArgsT *> &args)
	: ReferenceBase(target, vector<unique_ptr<ArgsT>>(args.begin(), args.end()))
	{}


	ReferenceBase(const string &target_name, const vector<ArgsT *> &args)
	: ReferenceBase(
		global_scope().lookup_global<TargetT>(target_name),
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

	const TargetT &operator * () const
	{ return target(); }

	TargetT &operator * ()
	{ return target(); }

	const TargetT *operator -> () const
	{ return &target(); }

	TargetT *operator -> ()
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

	string mangled_name() const
	{ return target_.lock()->mangled_name(); }

	arity_t arity() const
	{ return target_.lock()->arity(); }

	virtual bool bound() const override
	{ return !target_.expired(); }

	const vector<unique_ptr<ArgsT>> &args() const
	{ return args_; }

	vector<unique_ptr<ArgsT>> &args()
	{ return args_; }

	virtual const ArgsT &arg_for_param(shared_ptr<const Variable> param) const override
	{ return binding_.get(param); }


	virtual bool consistent() const override
	{
		if (!bound() || args().size() != target()->params().size())
			return false;

		// Compare target argument types with this reference's argument types
		auto it_arg = args().begin();
		auto it_param = target()->params().begin();
		for (; it_arg < args().end() && it_param < target()->params().end(); ++it_arg, ++it_param) {
			const Type &t_arg = (*it_arg)->type();
			const Type &t_param = (*it_param)->type();
			if (!(t_param >= **it_arg)
				&& !(t_arg.is<SymbolType>() && t_param.is<StringType>())
				// TODO: Hack: Allow passing a symbol value to a string argument
				// This is needed because ReadyLog can't deal with strings.
			)
				return false;

			if ((*it_arg)->is_ref() && !dynamic_cast<AbstractReference &>(**it_arg).consistent())
				return false;
		}

		return true;
	}


	shared_ptr<TargetT> target() const
	{ return target_.lock(); }

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

	virtual Binding<ArgsT> &binding()
	{ return binding_; }

	virtual const Binding<ArgsT> &binding() const
	{ return binding_; }

protected:
	vector<unique_ptr<ArgsT>> args_;
	weak_ptr<TargetT> target_;
	Binding<ArgsT> binding_;
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
			this->binding().attach_semantics(f);
			for (auto &arg : this->args())
				arg->attach_semantics(f);
		}
	}
};



template<class TargetT>
class ZeroArityReference
: public Expression
, public AbstractReference
, public NoScopeOwner
, public LanguageElement<Reference<TargetT>>
{
public:
	ZeroArityReference(const shared_ptr<TargetT> &target);

	ZeroArityReference(ZeroArityReference<TargetT> &&other);

	virtual ~ZeroArityReference() override = default;

	const TargetT &operator * () const;
	TargetT &operator * ();
	const TargetT *operator -> () const;
	TargetT *operator -> ();

	bool operator == (const ZeroArityReference<TargetT> &other) const;
	bool operator != (const ZeroArityReference<TargetT> &other) const;

	const string &name() const;
	virtual bool bound() const override;
	shared_ptr<TargetT> target();
	shared_ptr<const TargetT> target() const;
	virtual bool consistent() const override;

	virtual string to_string(const string &pfx) const override;
	virtual const Type &type() const override;
	size_t hash() const;

	virtual const Expression &arg_for_param(shared_ptr<const Variable> param) const override;

private:
	shared_ptr<TargetT> target_;
};



template<>
class Reference<Variable>
: public ZeroArityReference<Variable>
{
public:
	using ZeroArityReference<Variable>::ZeroArityReference;

	virtual void attach_semantics(SemanticsFactory &implementor) override;
};



template<class TargetT>
ZeroArityReference<TargetT>::ZeroArityReference(const shared_ptr<TargetT> &target)
: target_(target)
{}

template<class TargetT>
ZeroArityReference<TargetT>::ZeroArityReference(ZeroArityReference<TargetT> &&other)
: target_(std::move(other.target_))
{}

template<class TargetT>
TargetT *ZeroArityReference<TargetT>::operator ->()
{ return target().get(); }

template<class TargetT>
const TargetT &ZeroArityReference<TargetT>::operator *() const
{ return *target(); }

template<class TargetT>
TargetT &ZeroArityReference<TargetT>::operator *()
{ return *target(); }

template<class TargetT>
const TargetT *ZeroArityReference<TargetT>::operator ->() const
{ return target().get(); }

template<class TargetT>
const string &ZeroArityReference<TargetT>::name() const
{ return target()->name(); }

template<class TargetT>
bool ZeroArityReference<TargetT>::bound() const
{ return target_.get(); }

template<class TargetT>
shared_ptr<TargetT> ZeroArityReference<TargetT>::target()
{ return target_; }

template<class TargetT>
shared_ptr<const TargetT> ZeroArityReference<TargetT>::target() const
{ return std::dynamic_pointer_cast<const TargetT>(target_); }

template<class TargetT>
bool ZeroArityReference<TargetT>::operator ==(const ZeroArityReference<TargetT> &other) const
{ return *target() == *other.target(); }

template<class TargetT>
bool ZeroArityReference<TargetT>::operator !=(const ZeroArityReference<TargetT> &other) const
{ return !(*this == other); }

template<class TargetT>
bool ZeroArityReference<TargetT>::consistent() const
{ return bound(); }

template<class TargetT>
string ZeroArityReference<TargetT>::to_string(const string &pfx) const
{ return target()->to_string(pfx); }

template<class TargetT>
const Type &ZeroArityReference<TargetT>::type() const
{ return target()->type(); }

template<class TargetT>
size_t ZeroArityReference<TargetT>::hash() const
{ return target()->hash(); }

template<class TargetT>
const Expression &ZeroArityReference<TargetT>::arg_for_param(shared_ptr<const Variable>) const
{ throw Bug("This method is undefined and should not have been called"); }



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


}

#endif // GOLOGPP_REFERENCE_H_
