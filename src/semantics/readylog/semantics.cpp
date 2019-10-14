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

namespace gologpp {

#define GOLOGPP_DEFINE_MAKE_SEMANTICS_IMPL(_r, _data, GologT) \
unique_ptr<AbstractSemantics> ReadylogSemanticsFactory::make_semantics(GologT &obj) \
{ return unique_ptr<AbstractSemantics>(new Semantics<GologT>(obj)); }

BOOST_PP_SEQ_FOR_EACH(GOLOGPP_DEFINE_MAKE_SEMANTICS_IMPL, (), GOLOGPP_SEMANTIC_TYPES)

}
