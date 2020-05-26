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

#pragma once

#include "reference.h"
#include "action.h"
#include "plan.h"


namespace gologpp {


template<class> class Grounding;

class AbstractGrounding
: public virtual AbstractReference
{
public:
	virtual const TBinding<Value> &params_to_args() const = 0;
	virtual TBinding<Value> &params_to_args() = 0;
};



template<>
class Grounding<AbstractAction>
: public virtual AbstractReference
, public AbstractGrounding
{
public:
	virtual const AbstractAction &operator * () const = 0;
	virtual AbstractAction &operator * () = 0;
	virtual const AbstractAction *operator -> () const = 0;
	virtual AbstractAction *operator -> () = 0;
};



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
	using ElementType = ModelElement;
	using ReferenceBase<TargetT, Value>::ReferenceBase;

	Grounding(const Grounding<TargetT> &other)
	: ReferenceBase<TargetT, Value>(other.target(), copy(other.args()))
	{}

	virtual ~Grounding() override = default;

	virtual const AbstractAction &operator * () const override
	{ return *this->target(); }

	virtual AbstractAction &operator * () override
	{ return *this->target(); }

	virtual const AbstractAction *operator -> () const override
	{ return this->target().get(); }

	virtual AbstractAction *operator -> () override
	{ return this->target().get(); }

	virtual Scope &parent_scope() override
	{ return global_scope(); }

	virtual const Scope &parent_scope() const override
	{ return global_scope(); }

	virtual const TBinding<Value> &params_to_args() const override
	{ return ReferenceBase<TargetT, Value>::params_to_args(); }

	virtual TBinding<Value> &params_to_args() override
	{ return ReferenceBase<TargetT, Value>::params_to_args(); }
};



class ExogEvent : public Grounding<ExogAction>, public LanguageElement<ExogEvent> {
public:
	using Grounding<ExogAction>::Grounding;

	virtual void attach_semantics(SemanticsFactory &) override;
};



}
