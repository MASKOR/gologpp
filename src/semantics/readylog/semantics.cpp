#include "semantics.h"
#include "effect_axiom.h"
#include "action.h"
#include "fluent.h"
#include "variable.h"
#include "constant.h"
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
