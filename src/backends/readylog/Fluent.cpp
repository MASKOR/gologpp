#include "Fluent.h"
#include <gologpp/Fluent.h>
#include "Scope.h"

#include <iostream>

namespace gologpp {

Implementation<Fluent>::Implementation(const Fluent &f)
: fluent_(&f)
{
	std::cout << sizeof(Implementation<Fluent>) << std::endl;
}


EC_word Implementation<Fluent>::term()
{
	return ::term(
		EC_functor(fluent_->name().c_str(), fluent_->arity()),
		fluent_->scope().impl_cast<Scope>().variables(fluent_->args())
	);
}

} // namespace gologpp
