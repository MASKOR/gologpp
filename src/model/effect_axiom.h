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

#ifndef GOLOGPP_EFFECTAXIOM_H_
#define GOLOGPP_EFFECTAXIOM_H_

#include "utilities.h"
#include "action.h"
#include "reference.h"
#include "language.h"
#include "expressions.h"
#include "fluent.h"
#include "procedural.h"

#include <boost/optional.hpp>

#include <type_traits>

namespace gologpp {

class Expression;


class AbstractEffectAxiom
: public virtual AbstractLanguageElement
{
public:
	AbstractEffectAxiom();
	AbstractEffectAxiom(AbstractEffectAxiom &&) = default;
	virtual ~AbstractEffectAxiom();

	const AbstractAction &action() const;
	AbstractAction &action();
	void set_action(AbstractAction &action);
	const Expression &condition() const;
	Expression &condition();
	void set_condition(Expression *condition);

protected:
	AbstractAction *action_;
	SafeExprOwner<BoolType> condition_;
};


template<class LhsT>
class EffectAxiom
: public AbstractEffectAxiom
, public NoScopeOwner
, public LanguageElement<EffectAxiom<LhsT>>
{
public:
	using ElementType = ModelElement;

	EffectAxiom()
	{}

	EffectAxiom(EffectAxiom<LhsT> &&o) = default;

	virtual ~EffectAxiom() override
	{}

	const LhsT &lhs() const
	{ return assignment_->lhs(); }

	const Expression &value() const
	{ return assignment_->rhs(); }


	virtual string to_string(const string &pfx) const override
	{
		return linesep + pfx + condition().to_string(pfx) + "?" + linesep
			+ pfx + indent + assignment_->to_string("") + ";";
	}

	void define(boost::optional<Expression *> condition, LhsT *lhs, Expression *value)
	{
		if (!(lhs->type() >= *value))
			throw TypeError(*value, lhs->type());

		if (condition)
			set_condition(*condition);

		assignment_.reset(new Assignment<LhsT>(lhs, value));
		assignment_->set_parent(this);
	}

	virtual Scope &parent_scope() override
	{ return action().scope(); }

	virtual const Scope &parent_scope() const override
	{ return action().scope(); }


	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*condition_, *assignment_)

protected:
	unique_ptr<Assignment<LhsT>> assignment_;
};


} // namespace gologpp

#endif /* GOLOGPP_EFFECTAXIOM_H_ */
