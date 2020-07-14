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

#ifndef GOLOGPP_TRANSITION_H_
#define GOLOGPP_TRANSITION_H_

#include <model/gologpp.h>
#include <model/reference.h>
#include <model/action.h>
#include <model/expressions.h>
#include <model/procedural.h>

#include "grounding.h"

namespace gologpp {


class Transition
: public Grounding<Action>
, public LanguageElement<Transition>
, public std::enable_shared_from_this<Transition>
{
public:
	using Hook = DurativeCall::Hook;

	Transition(const shared_ptr<Action> &action, vector<unique_ptr<Value>> &&args, Hook hook);
	Transition(const Transition &);

	Hook hook() const;
	virtual string to_string(const string &pfx) const override;
	virtual void attach_semantics(SemanticsFactory &) override;

private:
	Hook hook_;
};



template<>
class GeneralSemantics<Transition>
: public virtual GeneralSemantics<Instruction>
{
public:
	GeneralSemantics(const Transition &elem, ExecutionContext &context);
	GeneralSemantics(const GeneralSemantics<Transition> &other);

	virtual unique_ptr<Plan> trans(const ABinding &, History &) override;

	const Transition &element() const;
	virtual const ModelElement &model_element() const override;
	virtual const Instruction &instruction() const override;

	void update_element(const Transition *new_element);
	virtual GeneralSemantics<Transition> *copy(const Transition &target_element) const = 0;

	virtual ExecutionContext &context() const override;

private:
	const Transition *element_;
	ExecutionContext &context_;
	bool final_;
};



}

#endif // GOLOGPP_TRANSITION_H_
