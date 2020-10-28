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


EC_word pl_binding_chain(const BindingChain &b);


template<>
class Semantics<Binding>
: public GeneralSemantics<Binding>
, public ReadylogSemantics
{
public:
	using GeneralSemantics<Binding>::GeneralSemantics;

	virtual EC_word plterm() override;
	virtual Semantics<Binding> *copy(const Binding &target_element) const override;
	virtual const Binding &model_element() const override;
};



template<class GologT>
EC_word reference_term(const ReferenceBase<GologT> &ref)
{
	if (ref.arity() > 0)
		return ::term(EC_functor(ref.mangled_name().c_str(), ref.arity()),
			to_ec_words(ref.args()).data()
		);
	else
		return EC_atom(ref.mangled_name().c_str());
}


EC_word reference_term(const Reference<Variable> &ref);


template<>
class Semantics<AbstractReference>
{
public:
	virtual EC_word plterm() = 0;
};


template<class TargetT>
class ReferenceSemantics
: public GeneralSemantics<Reference<TargetT>>
, public Semantics<AbstractReference>
{
public:
	using GeneralSemantics<Reference<TargetT>>::GeneralSemantics;

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


template<>
class Semantics<Reference<ExogFunction>>
: public ReferenceSemantics<ExogFunction>
, public Semantics<typename Reference<ExogFunction>::ElementType>
{
public:
	using ReferenceSemantics<ExogFunction>::ReferenceSemantics;

	virtual EC_word plterm() override;
	virtual Value evaluate(const BindingChain &bc, const History &h) override;
	virtual const Expression &expression() const override;
};




template<class TargetT>
class Semantics<Reference<TargetT>>
: public ReferenceSemantics<TargetT>
, public Semantics<typename Reference<TargetT>::ElementType>
{
public:
	using ReferenceSemantics<TargetT>::ReferenceSemantics;

	virtual EC_word plterm() override
	{ return reference_term(this->element()); }
};



} // namespace gologpp



#endif // READYLOG_REFERENCE_H_
