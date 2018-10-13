#include "scope.h"
#include "variable.h"

namespace gologpp {

Semantics<Scope>::Semantics(const Scope &s)
: scope_(s)
{}


EC_word *Semantics<Scope>::variables(const vector<string> &names)
{
	EC_word *rv = new EC_word[names.size()];
	arity_t i = 0;
	for (const string &name : names)
		rv[i++] = scope_.lookup_var(name)->semantics().plterm();
	return rv;
}


void Semantics<Scope>::init_vars()
{
	for (auto &entry : scope_.var_map())
		if (&scope_ == &(global_scope()) || !scope_.parent_scope().lookup_var(entry.first))
			// Only init variables that aren't in the parent scope
			// (They are initialized there).
			entry.second->semantics<AbstractVariable>().init();
}


} // namespace gologpp
