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

#ifndef READYLOG_REFERENCE_H_
#define READYLOG_REFERENCE_H_

#include <model/scope.h>
#include <model/reference.h>

// Include headers for everything we might have a reference to (see plterm() method)
#include <model/action.h>
#include <model/fluent.h>
#include <model/procedural.h>
#include <model/variable.h>

#include "semantics.h"
#include "utilities.h"
#include "variable.h"

#include "wrap_eclipseclass.h"


namespace gologpp {


EC_word pl_binding_chain(const BindingChain &b);


/// Disambiguate this against the generic Semantics<GologT, Cond>, which
/// is also a partial specialization but requires this to be false in Cond.
template<class ExprT>
bool partially_specialized<Semantics<Binding<ExprT>>> = true;

template<class ExprT>
class Semantics<Binding<ExprT>>
: public GeneralSemantics<Binding<ExprT>>
, public Semantics<ModelElement>
{
public:
	using GeneralSemantics<Binding<ExprT>>::GeneralSemantics;

	virtual EC_word plterm() override;
	virtual Semantics<Binding<ExprT>> *copy(const Binding<ExprT> &target_element) const override;
};



template<class GologT, class ArgsT>
EC_word reference_term(const ReferenceBase<GologT, ArgsT> &ref)
{
	if (ref.arity() > 0)
		return ::term(EC_functor(ref.mangled_name().c_str(), ref.arity()),
			to_ec_words(ref.args()).data()
		);
	else
		return EC_atom(ref.mangled_name().c_str());
}


EC_word reference_term(const Reference<Variable> &ref);



/*/// Disambiguate this against the generic Semantics<GologT, Cond>, which
/// is also a partial specialization but requires this to be false in Cond.
template<class TargetT, class ArgsT>
bool partially_specialized<Semantics<ReferenceBase<TargetT, ArgsT>>> = true;*/

template<class TargetT, class ArgsT>
class ReferenceSemantics
: public GeneralSemantics<Reference<TargetT, ArgsT>>
{
public:
	using GeneralSemantics<Reference<TargetT, ArgsT>>::GeneralSemantics;

	bool args_need_eval() {
		for (auto &expr : this->element().args())
			if (!expr->template is_a<Reference<Variable>>() && !expr->template is_a<Value>())
				return true;

		return false;
	}

	EC_word plterm_free_args()
	{
		if (this->element().arity() == 0)
			return EC_atom(this->element().mangled_name().c_str());
		else {
			vector<EC_word> args;
			arity_t i = 0;
			for (auto &expr : this->element().args()) {
				if (!expr->template is_a<Reference<Variable>>() && !expr->template is_a<Value>())
					args.push_back(this->element().target()->parameter(i)->special_semantics().plterm());
				else
					args.push_back(expr->semantics().plterm());
				++i;
			}

			return ::term(EC_functor(this->element().mangled_name().c_str(), this->element().arity()),
				args.data()
			);
		}
	}

	EC_word args_binding() {
		EC_word list = ::nil();
		for (const shared_ptr<Variable> &param : this->element().target()->params()) {
			const Expression &arg = this->element().arg_for_param(param);
			if (!arg.is_a<Reference<Variable>>() && !arg.is_a<Value>()) {
				list = ::list(
					::term(EC_functor("=", 2),
						param->special_semantics().plterm(),
						arg.semantics().plterm()
					),
					list
				);
			}
		}

		return ::term(EC_functor("and", 1), list);
	}
};



template<class TargetT, class ArgsT>
bool partially_specialized<Semantics<Reference<TargetT, ArgsT>>> = true;


template<class TargetT>
class Semantics<Reference<TargetT, Value>>
: public ReferenceSemantics<TargetT, Value>
, public Semantics<typename TargetT::SignifierT>
{
public:
	using ReferenceSemantics<TargetT, Value>::ReferenceSemantics;

	Semantics<Reference<TargetT, Value>> *copy(const Reference<TargetT, Value> &target) const override
	{ return new Semantics<Reference<TargetT, Value>>(target, this->context()); }

	virtual EC_word plterm() override
	{ return reference_term(this->element()); }
};



template<class TargetT>
class Semantics<Reference<TargetT, Expression>>
: public ReferenceSemantics<TargetT, Expression>
, public Semantics<typename TargetT::SignifierT>
{
public:
	using ReferenceSemantics<TargetT, Expression>::ReferenceSemantics;

	virtual EC_word plterm() override
	{ return reference_term(this->element()); }
};



template<>
class Semantics<Reference<Variable>>
: public Semantics<Expression>
, public GeneralSemantics<Reference<Variable>>
{
public:
	using GeneralSemantics<Reference<Variable>>::GeneralSemantics;

	virtual EC_word plterm() override;
};



template<>
class Semantics<Reference<ExogFunction>>
: public GeneralSemantics<Reference<ExogFunction>>
, public Semantics<Expression>
{
public:
	using GeneralSemantics<Reference<ExogFunction>>::GeneralSemantics;

	virtual EC_word plterm() override;
	virtual Value evaluate(const BindingChain &bc, const History &h) override;
	virtual const Expression &expression() const override;
};




} // namespace gologpp



#endif // READYLOG_REFERENCE_H_
