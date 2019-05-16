#include "fluent.h"
#include "execution.h"

namespace gologpp {



Scope &InitialValue::parent_scope()
{ return fluent().scope(); }

const Scope &InitialValue::parent_scope() const
{ return fluent().scope(); }


Fluent::Fluent(Scope *own_scope, const string &type_name, const string &name, const vector<shared_ptr<Variable>> &args)
: Global(name, args)
, ScopeOwner(own_scope)
, domain_(new Domain(
	type_name,
	"implicit_domain(" + name + "/" + std::to_string(args.size()) + ")",
	{},
	true
) )
{
	set_type_by_name(type_name);
	domain_->add_subject(*this);
}

Fluent::Fluent(Scope &parent_scope, const string &type_name, const string &name)
: Global(name, {})
, ScopeOwner(new Scope(parent_scope))
{ set_type_by_name(type_name); }


void Fluent::compile(AExecutionContext &ctx)
{ ctx.compile(*this); }


} // namespace gologpp
