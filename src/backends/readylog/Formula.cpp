#include "Formula.h"

namespace gologpp {
namespace readylog {


void Negation::init(EC_word &cache)
{
	cache = ::term(EC_functor("neg", 1),
	               dynamic_cast<Translatable<BooleanExpression, EC_word> &>(*expression_).translate());
}


}
}