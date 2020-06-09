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

#include "value.h"
#include "semantics.h"
#include "effect_axiom.h"
#include "action.h"
#include "fluent.h"
#include "variable.h"
#include "formula.h"
#include "procedural.h"
#include "reference.h"
#include "scope.h"
#include "arithmetic.h"
#include "execution.h"
#include "domain.h"
#include "string.h"
#include "history.h"
#include "compound_expression.h"
#include "list_expression.h"
#include "activity.h"
#include "transition.h"
#include "execution.h"
#include "plan.h"

namespace gologpp {


ReadylogContext &Semantics<ModelElement>::rl_context() const
{ return dynamic_cast<ReadylogContext &>(context()); }


Value Semantics<Expression>::evaluate(const Binding &b, const History &h)
{
	if (expression().type().is<BoolType>()) {
		return Value(get_type<BoolType>(),
			ReadylogContext::instance().ec_query(
				::term(EC_functor(",", 2),
					b.semantics().plterm(),
					::term(EC_functor("holds", 2),
						this->plterm(),
						h.semantics().plterm()
					)
				)
			)
		);
	}
	else {
		EC_ref Result;
		EC_word query = ::term(EC_functor(",", 2),
			b.semantics().plterm(),
			::term(EC_functor("subf", 3),
				this->plterm(),
				Result,
				h.semantics().plterm()
			)
		);
		if (ReadylogContext::instance().ec_query(query))
			return pl_term_to_value(Result);
		else
			throw EclipseError("Failed to evaluate " + ReadylogContext::instance().to_string(plterm()));
	}
}

const Expression &Semantics<Expression>::expression() const
{ return dynamic_cast<const Expression &>(model_element()); }


unique_ptr<Plan> Semantics<Instruction>::trans(const Binding &, History &history)
{
	if (!rl_context().options().guitrace && rl_context().options().toplevel) {
		post_goal("toplevel");
		EC_resume();
	}

	EC_ref h1, e1;

	EC_word trans = ::term(EC_functor("trans", 4),
		next_readylog_term(),
		history.semantics().plterm(),
		e1, h1
	);

	EC_ref Ball;
	EC_word catch_trans = ::term(EC_functor("catch", 3),
		trans, Ball, EC_atom("true")
	);

	EC_word q;
	if (rl_context().options().trace)
		q = ::term(EC_functor("trace", 1), catch_trans);
	else
		q = catch_trans;


	if (rl_context().ec_query(q)) {
		if (EC_word(Ball).is_var() != EC_succeed)
			// Caught eclipse exception
			throw EclipseError(rl_context().to_string(q) + ": " + rl_context().to_string(Ball));
		else {
			// Successful transition

			EC_word prog(e1), head, tail;

			if (prog.is_list(head, tail) != EC_succeed)
				throw EclipseError("Output program ist not a list: " + rl_context().to_string(prog));

			unique_ptr<Plan> rv;

			if (functor_name(head) == "applyPolicy") {
				// program executed a solve operator: a policy was produced
				rv = plan_from_ec_word(head);
			}
			else {
				// Normal transition
				rv.reset(new Plan());
				shared_ptr<Transition> trans = history.semantics().get_last_transition();
				if (trans)
					rv->append(new Transition(*trans));
			}

			history.semantics().extend_history(h1);
			next_readylog_term_ = e1;

			return rv;
		}
	}
	else
		return nullptr;
}


EC_word gologpp::Semantics<Instruction>::next_readylog_term()
{
	if (!next_readylog_term_.initialized())
		next_readylog_term_ = plterm();
	return next_readylog_term_;
}


bool Semantics<Instruction>::final(const Binding &, const History &h)
{
	EC_word final = ::term(EC_functor("final", 2),
		next_readylog_term(),
		h.semantics().plterm()
	);
	bool rv = rl_context().ec_query(final);
	return rv;
}

const Instruction &Semantics<Instruction>::instruction() const
{ return dynamic_cast<const Instruction &>(model_element()); }



ReadylogSemanticsFactory::ReadylogSemanticsFactory(ReadylogContext &context)
: context_(context)
{}

#define GOLOGPP_DEFINE_MAKE_SEMANTICS_IMPL(_r, _data, GologT) \
unique_ptr<AbstractSemantics<ModelElement>> ReadylogSemanticsFactory::make_semantics(GologT &obj) \
{ return unique_ptr<AbstractSemantics<ModelElement>>(new Semantics<GologT>(obj, context_)); }

BOOST_PP_SEQ_FOR_EACH(GOLOGPP_DEFINE_MAKE_SEMANTICS_IMPL, (), GOLOGPP_SEMANTIC_TYPES)




} // namespace gologpp



