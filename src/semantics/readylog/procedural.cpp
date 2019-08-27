#include "scope.h"
#include "procedural.h"
#include "reference.h"
#include "execution.h"
#include "constant.h"

#include <model/procedural.h>


namespace gologpp {



EC_word Semantics<Function>::plterm()
{
	if (function_.arity() > 0)
		return ::term(EC_functor(function_.name().c_str(), function_.arity()),
			to_ec_words(function_.params()).data()
		);
	else
		return EC_atom(function_.name().c_str());
}


EC_word Semantics<Function>::definition()
{
	if (function_.type().is<VoidType>() || function_.type().is<BoolType>()) {
		function_.scope().semantics().init_vars();
		return ::term(EC_functor("proc", 2),
			plterm(),
			function_.definition().semantics().plterm()
		);
	}
	else {
		function_.scope().semantics().init_vars();
		return_var_ = ::newvar();

		return ::term(EC_functor("function", 3),
			plterm(),
			return_var_,
			function_.definition().semantics().plterm()
		);
	}
}


EC_word Semantics<Function>::return_var()
{
	if (function_.type().is<VoidType>())
		throw Bug("Attempt to get a return var for a procedure");

	return return_var_;
}




Semantics<Block>::Semantics(const Block &b)
: block_(b)
{}


EC_word Semantics<Block>::plterm()
{
	block_.scope().semantics().init_vars();

	EC_word rv;

	const Function *parent_fn = dynamic_cast<const Function *>(block_.parent());
	if (parent_fn && !parent_fn->type().is<VoidType>()) {
		if (block_.elements().size() == 1)
			rv = block_.elements()[0]->semantics().plterm();
		else
			throw std::runtime_error("Functions must contain exactly one return statement and nothing else.");
	}
	else
		rv = to_ec_list(block_.elements(), block_.elements().begin());

	set_current_program(rv);

	return rv;
}


EC_word Semantics<Block>::current_program()
{ return current_program_; }

void Semantics<Block>::set_current_program(EC_word e)
{ current_program_ = e; }



Semantics<Choose>::Semantics(const Choose &c)
: choose_(c)
{}

EC_word Semantics<Choose>::plterm()
{
	choose_.scope().semantics().init_vars();
	return ::term(EC_functor("nondet", 1), to_ec_list(choose_.alternatives()));
}



Semantics<Conditional>::Semantics(const Conditional &c)
: conditional_(c)
{}


EC_word Semantics<Conditional>::plterm()
{
	EC_functor fn("if", 3);

	const AbstractLanguageElement *parent = conditional_.parent();
	while (parent) {
		if (parent->is_a<Function>() && !parent->type().is<VoidType>()) {
			// An actual ReadyLog function (not a procedure): have to use lif/3
			fn = EC_functor("lif", 3);
			break;
		}
		parent = dynamic_cast<const Expression *>(parent)->parent();
	}
	return ::term(fn,
		conditional_.condition().semantics().plterm(),
		conditional_.block_true().semantics().plterm(),
		conditional_.block_false().semantics().plterm()
	);
}



Semantics<Concurrent>::Semantics(const Concurrent &c)
: concurrent_(c)
{}


EC_word Semantics<Concurrent>::plterm()
{
	concurrent_.scope().semantics().init_vars();
	return ::term(EC_functor("pconc", 1), to_ec_list(concurrent_.procs()));
}


Semantics<Assignment<Reference<Fluent>>>::Semantics(const Assignment<Reference<Fluent>> &ass)
: assignment_(ass)
{}

EC_word Semantics<Assignment<Reference<Fluent>>>::plterm()
{
	return ::term(EC_functor("set", 2),
		assignment_.lhs().semantics().plterm(),
		assignment_.rhs().semantics().plterm()
	);
}


Semantics<Assignment<FieldAccess>>::Semantics(const Assignment<FieldAccess> &ass)
: assignment_(ass)
, field_access_(ass.lhs())
{}


EC_word Semantics<Assignment<FieldAccess>>::plterm()
{
	const FieldAccess *fa = &assignment_.lhs();
	const Reference<Fluent> *fluent = nullptr;
	const Expression *sub;
	EC_word field_list = ::nil();

	do {
		field_list = ::list(EC_atom(fa->field_name().c_str()), field_list);
		sub = &fa->subject();
		fa = dynamic_cast<const FieldAccess *>(sub);
	} while (fa && fa->type().is<CompoundType>());

	fluent = dynamic_cast<const Reference<Fluent> *>(sub);

	return ::term(EC_functor("set", 2),
		fluent->semantics().plterm(),
		::term(EC_functor("gpp_field_assign", 3),
			field_list,
			assignment_.rhs().semantics().plterm(),
			fluent->semantics().plterm()
		)
	);
}



Semantics<Pick>::Semantics(const Pick &pick)
: pick_(pick)
{
	if (pick_.domain().empty())
		throw std::runtime_error("ReadyLog requires a domain for pick()!");
}

EC_word Semantics<Pick>::plterm()
{
	// Make sure the `pick'ed variable is a Golog variable
	// No init_vars() is needed in this case.
	{ GologVarMutator guard(pick_.variable().semantics());
		return ::term(EC_functor("pickBest", 3),
			pick_.variable().semantics().plterm(),
			to_ec_list(pick_.domain(), pick_.domain().begin()),
			pick_.statement().semantics().plterm()
		);
	}
}



Semantics<Search>::Semantics(const Search &search)
: search_(search)
{}


EC_word Semantics<Search>::plterm()
{
	return ::term(EC_functor("search", 1),
		search_.statement().semantics().plterm()
	);
}



Semantics<Solve>::Semantics(const Solve &solve)
: solve_(solve)
{}


EC_word Semantics<Solve>::plterm()
{
	return ::term(EC_functor("solve", 3),
		solve_.statement().semantics().plterm(),
		solve_.horizon().semantics().plterm(),
		solve_.reward().semantics().plterm()
	);
}



Semantics<Test>::Semantics(const Test &test)
: test_(test)
{}


EC_word Semantics<Test>::plterm()
{
	return ::term(EC_functor("?", 1),
		test_.expression().semantics().plterm()
	);
}



Semantics<While>::Semantics(const While &w)
: while_(w)
{}


EC_word Semantics<While>::plterm()
{
	return ::term(EC_functor("while", 2),
		while_.expression().semantics().plterm(),
		while_.statement().semantics().plterm()
	);
}



Semantics<Return>::Semantics(const Return &r)
: ret_(r)
{}

EC_word Semantics<Return>::plterm() {
	const AbstractLanguageElement *root_parent = ret_.parent();
	const Expression *parent_expr = nullptr;
	while ((parent_expr = dynamic_cast<const Expression *>(root_parent))) {
		root_parent = parent_expr->parent();
	}

	try {
		const Function &function = dynamic_cast<const Function &>(*root_parent);
		if (function.type() != ret_.expression().type())
			throw ExpressionTypeMismatch(function, ret_.expression());

		if (ret_.expression().type().is<BoolType>())
			return ret_.expression().semantics().plterm();
		else
			return ::term(EC_functor("=", 2),
				function.semantics().return_var(),
				ret_.expression().semantics().plterm()
			);
	} catch (std::bad_cast &) {
		throw Bug(ret_.str() + " outside of function");
	}
}



Semantics<DurativeCall>::Semantics(const DurativeCall &call)
: call_(call)
{}


EC_word Semantics<DurativeCall>::plterm()
{
	return ::term(EC_functor(to_string(call_.hook()).c_str(), 2),
		reference_term(call_.action()),
		EC_atom("now")
	);
}



Semantics<FieldAccess>::Semantics(const FieldAccess &field_access)
: field_access_(field_access)
, is_lvalue_(false)
{}

EC_word Semantics<FieldAccess>::plterm()
{
	return ::term(EC_functor("gpp_field_value", 2),
		EC_atom(field_access_.field_name().c_str()),
		field_access_.subject().semantics().plterm()
	);
}


EC_word Semantics<FieldAccess>::field_assign(const Expression &value)
{
	return ::term(EC_functor("gpp_field_assign", 3),
		EC_atom(field_access_.field_name().c_str()),
		value.semantics().plterm(),
		field_access_.subject().semantics().plterm()
	);
}


void Semantics<FieldAccess>::set_lvalue(bool lvalue)
{ is_lvalue_ = lvalue; }



}
