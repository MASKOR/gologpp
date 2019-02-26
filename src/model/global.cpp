#include "error.h"
#include "global.h"
#include "scope.h"
#include "variable.h"
#include "reference.h"


namespace gologpp {



Global::Global(const string &name, const vector<shared_ptr<AbstractVariable>> &args)
: Identifier (name, static_cast<arity_t>(args.size()))
{ set_args(args); }

const vector<shared_ptr<AbstractVariable>> &Global::args() const
{ return args_; }

vector<shared_ptr<AbstractVariable>> &Global::args()
{ return args_; }

shared_ptr<AbstractVariable> Global::argument(arity_t idx) const
{ return args_[idx]; }

void Global::set_args(const vector<shared_ptr<AbstractVariable>> &args)
{
	if (static_cast<arity_t>(args.size()) != arity() &&
		global_scope().exists_global(this->name(), this->arity())
	)
		throw Bug("Cannot change the arity of a Global that is already registered.");

	for (const shared_ptr<AbstractVariable> &var : args) {
		args_.push_back(var);
		dynamic_cast<Expression *>(var.get())->set_parent(this);
	}
	set_arity(static_cast<arity_t>(args.size()));
}


template<class ExprT>
Reference<Variable<ExprT>> *Global::arg_ref(const string &name)
{
	for (shared_ptr<AbstractVariable> &var : args_)
		if (var->name() == name)
			return std::dynamic_pointer_cast<Variable<ExprT>>(var)->ref();

	return nullptr;
}

#define GOLOGPP_GLOBAL_INSTANTIATE_ARG_REF(_r, _, T) \
	template \
	Reference<Variable<T>> *Global::arg_ref(const string &name);

BOOST_PP_SEQ_FOR_EACH(GOLOGPP_GLOBAL_INSTANTIATE_ARG_REF, (), GOLOGPP_VALUE_TYPES)


Scope &Global::parent_scope()
{ return global_scope(); }

const Scope &Global::parent_scope() const
{ return global_scope(); }



} // namespace gologpp
