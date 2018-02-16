#include "Fluent.h"
#include <gologpp/Fluent.h>
#include "Scope.h"

#include <iostream>

namespace gologpp {

Implementation<generic::Fluent>::Implementation(const generic::Fluent &f)
: fluent_(&f)
{
	std::cout << sizeof(Implementation<generic::Fluent>) << std::endl;
}


EC_word Implementation<generic::Fluent>::term()
{
	return ::term(
		EC_functor(fluent_->name().c_str(), fluent_->arity()),
		fluent_->scope().impl_cast<generic::Scope>().variables(fluent_->args())
	);
}

} // namespace gologpp
