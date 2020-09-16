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

#include <model/platform/semantics.h>
#include <model/platform/switch_state_action.h>
#include <model/logger.h>

namespace gologpp {

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

unique_ptr<SemanticsFactory> SemanticsFactory::construct()
{
	return unique_ptr<SemanticsFactory>(new ReadylogSemanticsFactory(
		platform::SemanticsFactory::construct()
	));
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

ReadylogContext &ReadylogSemantics::rl_context() const
{ return dynamic_cast<ReadylogContext &>(context()); }



/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

Value Semantics<Expression>::evaluate(const Binding &b, const History &h)
{
	b.semantics<Binding>().init_vars();
	if (expression().type().is<BoolType>()) {
		EC_word query = ::term(EC_functor("eval_formula", 1), ::term(EC_functor(",", 2),
			b.semantics<Binding>().plterm(),
			::term(EC_functor("holds", 2),
				this->plterm(),
				h.special_semantics().plterm()
			)
		) );
		if (rl_context().options().trace)
			query = ::term(EC_functor("trace", 1), query);

		return Value(get_type<BoolType>(), ReadylogContext::instance().ec_query(query));
	}
	else {
		EC_ref Result;
		EC_word query = ::term(EC_functor(",", 2),
			b.semantics<Binding>().plterm(),
			::term(EC_functor("subf", 3),
				this->plterm(),
				Result,
				h.special_semantics().plterm()
			)
		);
		if (rl_context().options().trace)
			query = ::term(EC_functor("trace", 1), query);

		if (ReadylogContext::instance().ec_query(query))
			return pl_term_to_value(Result);
		else
			throw EclipseError("Failed to evaluate " + ReadylogContext::instance().to_string(plterm()));
	}
}

const Expression &Semantics<Expression>::expression() const
{ return dynamic_cast<const Expression &>(model_element()); }



/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

unique_ptr<Plan> Semantics<Instruction>::trans(const Binding &, History &history)
{
	log(LogLevel::DBG) << "Readylog trans: " << instruction() << flush;

	if (!rl_context().options().guitrace && rl_context().options().toplevel) {
		post_goal("toplevel");
		EC_resume();
	}

	EC_ref h1, e1;

	EC_word trans = ::term(EC_functor("trans", 4),
		next_readylog_term(),
		history.special_semantics().plterm(),
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

			ManagedTerm prog(e1);

			history.special_semantics().extend_history(h1);

			EC_word head, tail;

			if (EC_word(prog).is_list(head, tail) != EC_succeed && EC_word(prog).is_nil() != EC_succeed)
				throw EclipseError("Output program ist not a list: " + rl_context().to_string(prog));

			unique_ptr<Plan> rv;

			if (EC_word(prog).is_nil() != EC_succeed && functor_name(head) == "applyPolicy") {
				// program executed a solve operator: a policy was produced
				next_readylog_term_ = tail;
				rv = plan_from_ec_word(head);
			}
			else {
				// Normal transition
				rv.reset(new Plan());
				shared_ptr<Transition> trans = history.special_semantics().get_last_transition();
				if (trans)
					rv->append(new Transition(*trans));
				next_readylog_term_ = prog;
			}


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
		h.special_semantics().plterm()
	);
	bool rv = rl_context().ec_query(final);
	return rv;
}

const Instruction &Semantics<Instruction>::instruction() const
{ return dynamic_cast<const Instruction &>(model_element()); }



/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

ReadylogSemanticsFactory::ReadylogSemanticsFactory(
	unique_ptr<platform::SemanticsFactory> &&psf
)
: SemanticsFactory(std::move(psf))
{}


BOOST_PP_SEQ_FOR_EACH(GOLOGPP_DEFINE_MAKE_SEMANTICS, ReadylogSemanticsFactory, GOLOGPP_SEMANTIC_TYPES)

ReadylogContext &ReadylogSemanticsFactory::context()
{ return dynamic_cast<ReadylogContext &>(SemanticsFactory::context()); }





} // namespace gologpp



