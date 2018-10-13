#include "variable.h"

namespace gologpp {



Semantics<AbstractVariable>::Semantics(const AbstractVariable &var)
: var_(var)
, as_golog_var_(false)
{
	const AbstractLanguageElement *parent = dynamic_cast<const Expression &>(var_).parent();
	size_t level = 1;
	while (parent && parent != &var_ && dynamic_cast<const Expression *>(parent)) {
		parent = dynamic_cast<const Expression *>(parent)->parent();
		++level;
	}
	golog_var_ = EC_atom(
		(var_.name() + "_lv" + std::to_string(level)).c_str()
	);
}

Semantics<AbstractVariable>::~Semantics()
{}

void Semantics<AbstractVariable>::init()
{ ec_var_ = ::newvar(); }

EC_word Semantics<AbstractVariable>::plterm()
{
	if (as_golog_var_)
		return golog_var_;
	else
		return ec_var_;
}


void Semantics<AbstractVariable>::translate_as_golog_var(bool gv)
{ as_golog_var_ = gv; }


EC_word Semantics<AbstractVariable>::member_restriction()
{
	return ::term(EC_functor("member", 2),
		var_.semantics().plterm(),
		var_.domain().semantics().plterm()
	);
}



GologVarMutator::GologVarMutator(Semantics<AbstractVariable> &var_impl)
: var_impl_(var_impl)
{ var_impl_.translate_as_golog_var(true); }

GologVarMutator::~GologVarMutator()
{ var_impl_.translate_as_golog_var(false); }



} // namespace gologpp
