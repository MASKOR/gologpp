#include "scope.h"
#include "atoms.h"

namespace gologpp {

Implementation<Scope>::Implementation(const Scope &s)
: scope_(s)
{}


EC_word *Implementation<Scope>::variables(const vector<string> &names)
{
	EC_word *rv = new EC_word[names.size()];
	arity_t i = 0;
	for (const string &name : names)
		rv[i++] = scope_.lookup_var(name)->implementation().term();
	return rv;
}


void Implementation<Scope>::init_vars()
{
	for (auto &entry : scope_.var_map())
		if (&scope_ == &(global_scope()) || !scope_.parent_scope().lookup_var(entry.first))
			// Only init variables that aren't in the parent scope
			// (They are initialized there).
			entry.second->implementation<AbstractVariable>().init();
}


} // namespace gologpp
