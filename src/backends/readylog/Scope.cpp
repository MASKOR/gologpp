#include <gologpp/Scope.h>
#include "Scope.h"
#include "atoms.h"
#include <gologpp/atoms.h>

namespace gologpp {

Implementation<generic::Scope>::Implementation(const generic::Scope &s)
: scope_(s)
{}


EC_word *Implementation<generic::Scope>::variables(const vector<string> &names)
{
	EC_word *rv = new EC_word[names.size()];
	arity_t i = 0;
	for (const string &name : names)
		rv[i++] = scope_.variable(name)->impl_cast<generic::Variable>().term();
	return rv;
}


void Implementation<generic::Scope>::init_vars()
{
	for (auto &entry : scope_.map())
		entry.second->impl_cast<generic::Variable>().init();
}


} // namespace gologpp
