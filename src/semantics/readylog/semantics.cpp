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

#include "semantics.h"
#include "effect_axiom.h"
#include "action.h"
#include "fluent.h"
#include "variable.h"
#include "value.h"
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

namespace gologpp {


Value Semantics<Expression>::evaluate(const Activity &context, const History &h)
{
	context.target()->scope().semantics().init_vars();

	vector<EC_word> queries;
	for (const ParamsToArgs<Value>::value_type &param_value : context.params_to_args()) {
		queries.push_back(
			::term(EC_functor("=", 2),
				param_value.first->semantics().plterm(),
				param_value.second.get().semantics().plterm()
			)
		);
	}
	EC_ref Result;
	queries.push_back(
		::term(EC_functor("subf", 3),
			this->plterm(),
			Result,
			h.semantics().current_history()
		)
	);
	EC_word query = to_ec_term(",", queries);
	if (ReadylogContext::instance().ec_query(query))
		return pl_term_to_value(Result);
	else
		throw EclipseError("Failed to evaluate " + ReadylogContext::instance().to_string(plterm()));
}



#define GOLOGPP_DEFINE_MAKE_SEMANTICS_IMPL(_r, _data, GologT) \
unique_ptr<AbstractSemantics<AbstractLanguageElement>> ReadylogSemanticsFactory::make_semantics(GologT &obj) \
{ return unique_ptr<AbstractSemantics<AbstractLanguageElement>>(new Semantics<GologT>(obj)); }

BOOST_PP_SEQ_FOR_EACH(GOLOGPP_DEFINE_MAKE_SEMANTICS_IMPL, (), GOLOGPP_SEMANTIC_TYPES)



} // namespace gologpp



