#include "error.h"
#include "global.h"
#include "scope.h"
#include "variable.h"
#include "reference.h"


namespace gologpp {



Global::Global(const string &name, const vector<shared_ptr<Variable>> &args)
: Identifier (name, static_cast<arity_t>(args.size()))
{ set_params(args); }

const vector<shared_ptr<Variable>> &Global::params() const
{ return params_; }

vector<shared_ptr<Variable>> &Global::params()
{ return params_; }

shared_ptr<Variable> Global::parameter(arity_t idx) const
{ return params_[idx]; }

void Global::set_params(const vector<shared_ptr<Variable>> &args)
{
	if (static_cast<arity_t>(args.size()) != arity() &&
		global_scope().exists_global(this->name(), this->arity())
	)
		throw Bug("Cannot change the arity of a Global that is already registered.");

	for (const shared_ptr<Variable> &var : args) {
		params_.push_back(var);
		dynamic_cast<Expression *>(var.get())->set_parent(this);
	}
	set_arity(static_cast<arity_t>(args.size()));
}


Reference<Variable> *Global::param_ref(const string &name)
{
	for (shared_ptr<Variable> &var : params_)
		if (var->name() == name)
			return std::dynamic_pointer_cast<Variable>(var)->ref();

	return nullptr;
}


Scope &Global::parent_scope()
{ return global_scope(); }

const Scope &Global::parent_scope() const
{ return global_scope(); }



} // namespace gologpp
