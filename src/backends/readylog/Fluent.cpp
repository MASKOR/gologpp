#include "Fluent.h"
#include <gologpp/Fluent.h>
#include "utilities.h"

namespace gologpp {

Implementation<generic::Fluent>::Implementation(const generic::Fluent &f)
: fluent_(f)
{}


EC_word Implementation<generic::Fluent>::term()
{
	return ::term(
		EC_functor(fluent_.name().c_str(), fluent_.arity()),
		compile_list(fluent_.args())
	);
}

} // namespace gologpp
