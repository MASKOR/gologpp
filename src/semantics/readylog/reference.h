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

#include "semantics.h"
#include "utilities.h"
#include "variable.h"
#include "value.h"

#include <eclipseclass.h>


namespace gologpp {

template<class GologT> class Reference;


template<>
class Semantics<ABinding>
: public Semantics<ModelElement>
{
public:
	virtual void init_vars() = 0;
};


template<class ArgsT>
class Semantics<Binding<ArgsT>>
: public Semantics<ABinding>
, public GeneralSemantics<Binding<ArgsT>>
{
public:
	using GeneralSemantics<Binding<ArgsT>>::GeneralSemantics;

	virtual EC_word plterm() override
	{
		vector<EC_word> pl_binds;
		for (const auto &pval : this->element().map()) {
			pl_binds.push_back(
				::term(EC_functor("=", 2),
					pval.first->semantics().plterm(),
					pval.second.get().semantics().plterm()
				)
			);
		}

		return to_ec_term(",", pl_binds);
	}

	virtual const Binding<ArgsT> &model_element() const override
	{ return this->element(); }

	virtual void init_vars() override
	{
		for (const auto &pval : this->element().map())
			pval.first->semantics().init();
	}
};




template<>
class Semantics<Binding<Value>>
: public Semantics<ABinding>
, public GeneralSemantics<Binding<Value>>
{
public:
	using GeneralSemantics<Binding<Value>>::GeneralSemantics;

	virtual EC_word plterm() override
	{
		vector<EC_word> pl_binds;
		for (const auto &pval : this->element().map()) {
			pl_binds.push_back(
				::term(EC_functor("=", 2),
					pval.first->semantics().plterm(),
					pval.second.get().semantics().plterm()
				)
			);
		}

		return to_ec_term(",", pl_binds);
	}

	virtual Semantics<Binding<Value>> *copy(const Binding<Value> &target_element) const override;

	virtual const Binding<Value> &model_element() const override;
	void init_vars();
};



template<class GologT, class ExprT>
EC_word reference_term(const ReferenceBase<GologT, ExprT> &ref)
{
	if (ref.arity() > 0)
		return ::term(EC_functor(ref.name().c_str(), ref.arity()),
			to_ec_words(ref.args()).data()
		);
	else
		return EC_atom(ref.name().c_str());
}


EC_word reference_term(const Reference<Variable> &ref);



template<class TargetT>
class Semantics<Reference<TargetT>>
: public GeneralSemantics<Reference<TargetT>>
, public Semantics<typename Reference<TargetT>::ElementType>
{
public:
	using GeneralSemantics<Reference<TargetT>>::GeneralSemantics;

	virtual EC_word plterm() override
	{ return reference_term(this->element()); }

	bool args_need_eval() {
		for (const unique_ptr<Expression> &expr : this->element().args())
			if (!expr->is_a<Reference<Variable>>() && !expr->is_a<Value>())
				return true;

		return false;
	}

	EC_word plterm_free_args()
	{
		if (this->element().arity() == 0)
			return EC_atom(this->element().name().c_str());
		else {
			vector<EC_word> args;
			arity_t i = 0;
			for (const unique_ptr<Expression> &expr : this->element().args()) {
				if (!expr->is_a<Reference<Variable>>() && !expr->is_a<Value>())
					args.push_back(this->element().target()->parameter(i)->semantics().plterm());
				else
					args.push_back(expr->semantics().plterm());
				++i;
			}

			return ::term(EC_functor(this->element().name().c_str(), this->element().arity()),
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
						param->semantics().plterm(),
						arg.semantics().plterm()
					),
					list
				);
			}
		}

		return ::term(EC_functor("and", 1), list);
	}
};




} // namespace gologpp



#endif // READYLOG_REFERENCE_H_
