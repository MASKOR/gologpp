#include "Formula.h"

namespace gologpp {
namespace readylog {


EC_word Negation::define()
{
	return ::term(EC_functor("neg", 1), )
}


}
}