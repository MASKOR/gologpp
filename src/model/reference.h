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


class Binding : public ModelElement {
public:
	using MapT = std::unordered_map <
		shared_ptr<const Variable>,
		unique_ptr<Expression>
	>;

	Binding(const Binding &);
	Binding(Binding &&);
	Binding() = default;

	virtual ~Binding() = default;

	void bind(shared_ptr<const Variable> var, unique_ptr<Expression> &&expr);
	virtual Expression &get(shared_ptr<const Variable> param) const;
	const MapT &map() const;

	virtual void attach_semantics(SemanticsFactory &f) override;
	virtual string to_string(const string &pfx) const override;

private:
	MapT var_bindings_;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

template<>
class GeneralSemantics<Binding>
: public virtual GeneralSemantics<ModelElement>
{
public:
	GeneralSemantics(const Binding &elem, AExecutionController &context);

	virtual ~GeneralSemantics<Binding>() = default;

	const Binding &element() const;
	void update_element(const Binding *new_element);
	virtual AExecutionController &context() const override;

	virtual GeneralSemantics<Binding> *copy(const Binding &target_element) const = 0;

private:
	const Binding *element_;
	AExecutionController &context_;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

template<class TargetT>
class ReferenceBase
: public virtual AbstractReference
, public NoScopeOwner
{
public:
	ReferenceBase(const shared_ptr<TargetT> &target, vector<unique_ptr<Expression>> &&args)
	: target_(target)
	{
		size_t idx = 0;
		while (idx < args.size() && idx < this->target()->params().size()) {
			unique_ptr<Expression> arg { std::move(args[idx]) };
			arg->set_parent(this);
			args_.emplace_back(arg.get());
			binding_.bind(this->target()->params()[idx], std::move(arg));
			++idx;
		}
		ensure_consistent();
	}

	ReferenceBase(const shared_ptr<TargetT> &target, Binding &&binding)
	: target_(target)
	, binding_(std::move(binding))
	{
		for (auto param : this->target()->params())
			args_.emplace_back(&this->binding().get(param));
		ensure_consistent();
	}

	ReferenceBase(const shared_ptr<TargetT> &target, const vector<Expression *> &args)
	: ReferenceBase(target, vector<unique_ptr<Expression>>(args.begin(), args.end()))
	{}


	ReferenceBase(const string &target_name, const vector<Expression *> &args)
	: ReferenceBase(
		global_scope().lookup_global<TargetT>(target_name),
		args
	)
	{}

	ReferenceBase(const string &target_name, const boost::optional<vector<Expression *>> &args)
	: ReferenceBase(target_name, args.get_value_or({}))
	{}

	ReferenceBase(ReferenceBase<TargetT> &&other)
	: args_(std::move(other.args_))
	, target_(std::move(other.target_))
	, binding_(std::move(other.binding_))
	{ ensure_consistent(); }


	ReferenceBase(const ReferenceBase<TargetT> &other)
	: args_(copy(other.args_))
	, target_(other.target_)
	, binding_(other.binding_)
	{ ensure_consistent(); }


	virtual ~ReferenceBase() override = default;

	virtual const TargetT &operator * () const
	{ return *this->target(); }

	virtual TargetT &operator * ()
	{ return *this->target(); }

	virtual const TargetT *operator -> () const
	{ return this->target().get(); }

	virtual TargetT *operator -> ()
	{ return this->target().get(); }


	bool operator == (const ReferenceBase<TargetT> &other) const
	{
		if (this->target() != other.target())
			return false;
		for (arity_t i = 0; i < this->target()->arity(); ++i) {
			if (*this->args()[i] != *other.args()[i])
				return false;
		}
		return true;
	}

	bool operator != (const ReferenceBase<TargetT> &other) const
	{ return !(*this == other); }

	const string &name() const
	{ return target_.lock()->name(); }

	string mangled_name() const
	{ return target_.lock()->mangled_name(); }

	arity_t arity() const
	{ return target_.lock()->arity(); }

	virtual bool bound() const override
	{ return !target_.expired(); }

	const vector<Expression *> &args() const
	{ return args_; }

	vector<Expression *> &args()
	{ return args_; }

	virtual const Expression &arg_for_param(shared_ptr<const Variable> param) const override
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
		for (auto &c : this->args())
			boost::hash_combine(rv, dynamic_cast<Value &>(*c).hash());

		return rv;
	}

	virtual Binding &binding()
	{ return binding_; }

	virtual const Binding &binding() const
	{ return binding_; }

protected:
	vector<Expression *> args_;
	weak_ptr<TargetT> target_;
	Binding binding_;
};


template<>
class Reference<AbstractAction>
: public virtual AbstractReference
{
public:
	virtual const AbstractAction &operator * () const = 0;
	virtual AbstractAction &operator * () = 0;
	virtual const AbstractAction *operator -> () const = 0;
	virtual AbstractAction *operator -> () = 0;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

template<class TargetT>
class Reference
: public ReferenceBase<TargetT>
, public LanguageElement<Reference<TargetT>>
, public TargetT::SignifierT
, public virtual std::conditional <
	std::is_base_of<AbstractAction, TargetT>::value,
	Reference<AbstractAction>,
	AbstractReference
  >::type
{
public:
	using ReferenceBase<TargetT>::ReferenceBase;

	Reference(const Reference<TargetT> &other)
	: ReferenceBase<TargetT>(other)
	{}

	virtual ~Reference() override = default;

	virtual const TargetT &operator * () const override
	{ return *this->target(); }

	virtual TargetT &operator * () override
	{ return *this->target(); }

	virtual const TargetT *operator -> () const override
	{ return this->target().get(); }

	virtual TargetT *operator -> () override
	{ return this->target().get(); }

	virtual void attach_semantics(SemanticsFactory &f) override
	{
		if (!this->semantics_) {
			this->semantics_ = f.make_semantics(*this);
			this->binding().attach_semantics(f);
			for (auto &arg : this->args())
				arg->attach_semantics(f);
		}
	}

	unique_ptr<Reference<TargetT>> ground_args(const Binding &b, const History &h) const
	{
		Binding gb;
		for (auto &pair : this->binding().map())
			gb.bind(
				pair.first,
				unique_ptr<Expression>(new Value(
					std::move(pair.second->general_semantics().evaluate(b, h))
				))
			);
		return unique_ptr<Reference<TargetT>>(new Reference<TargetT>(this->target(), std::move(gb)));
	}

};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

template<class TargetT>
class ZeroArityReference
: public virtual TargetT::ElementType
, public AbstractReference
, public NoScopeOwner
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

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

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

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

template<>
class Reference<Variable>
: public ZeroArityReference<Variable>
, public LanguageElement<Reference<Variable>>
{
public:
	using ZeroArityReference<Variable>::ZeroArityReference;

	virtual void attach_semantics(SemanticsFactory &implementor) override;
};



} // namespace gologpp



/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

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
