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

#include "effect_axiom.h"
#include "action.h"

namespace gologpp {

AbstractEffectAxiom::AbstractEffectAxiom()
: condition_(new Value(BoolType::name(), true))
{}

AbstractEffectAxiom::~AbstractEffectAxiom()
{}


const AbstractAction &AbstractEffectAxiom::action() const
{ return *action_; }

AbstractAction &AbstractEffectAxiom::action()
{ return *action_; }

void AbstractEffectAxiom::set_action(AbstractAction &action)
{ action_ = &action; }

const Expression &AbstractEffectAxiom::condition() const
{ return *condition_; }

Expression &AbstractEffectAxiom::condition()
{ return *condition_; }

void AbstractEffectAxiom::set_condition(Expression *condition)
{
	condition_ = condition;
	condition_->set_parent(this);
}




template<>
const Reference<Fluent> &EffectAxiom<Reference<Fluent>>::fluent() const
{ return lhs(); }

template<>
const Reference<Fluent> &EffectAxiom<FieldAccess>::fluent() const
{ return dynamic_cast<const Reference<Fluent> &>(lhs().subject()); }

template<>
const Reference<Fluent> &EffectAxiom<ListAccess>::fluent() const
{ return dynamic_cast<const Reference<Fluent> &>(lhs().subject()); }



} // namespace gologpp
