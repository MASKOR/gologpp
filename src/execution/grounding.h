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

#include <model/reference.h>
#include <model/action.h>
#include "plan.h"


namespace gologpp {


template<class> class Grounding;

class AGrounding
: public virtual AbstractReference
{
public:
	virtual const Binding<Value> &binding() const = 0;
	virtual Binding<Value> &binding() = 0;
};



template<>
class Grounding<AbstractAction>
: public virtual AbstractReference
, public AGrounding
, public Instruction
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
	AGrounding          // All others use some placeholder
  >::type
, public ReferenceBase<TargetT, Value>
{
public:
	using ElementType = ModelElement;
	using ReferenceBase<TargetT, Value>::ReferenceBase;
	using ReferenceBase<TargetT, Value>::hash;

	Grounding(const Grounding<TargetT> &other)
	: ReferenceBase<TargetT, Value>(other.target(), copy(other.args()))
	{
		if (other.semantics_) {
			binding().set_semantics(
				std::unique_ptr<GeneralSemantics<ModelElement>>(
					other.binding().template general_semantics<Binding<Value>>().copy(binding())
				)
			);
		}
	}

	virtual ~Grounding() override = default;

	virtual const AbstractAction &operator * () const override
	{ return *this->target(); }

	virtual AbstractAction &operator * () override
	{ return *this->target(); }

	virtual const AbstractAction *operator -> () const override
	{ return this->target().get(); }

	virtual AbstractAction *operator -> () override
	{ return this->target().get(); }

	Grounding<TargetT> &operator = (const Grounding<TargetT> &other) = delete;

	virtual Scope &parent_scope() override
	{ return global_scope(); }

	virtual const Scope &parent_scope() const override
	{ return global_scope(); }

	virtual const Binding<Value> &binding() const override
	{ return ReferenceBase<TargetT, Value>::binding(); }

	virtual Binding<Value> &binding() override
	{ return ReferenceBase<TargetT, Value>::binding(); }
};



class ExogEvent : public Grounding<ExogAction>, public LanguageElement<ExogEvent> {
public:
	using Grounding<ExogAction>::Grounding;

	virtual void attach_semantics(SemanticsFactory &) override;
};



} // namespace gologpp



namespace std {


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


