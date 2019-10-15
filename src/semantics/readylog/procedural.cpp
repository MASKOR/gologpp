/*************************************************************************
 * This file is part of golog++.
 *
 * golog++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * golog++ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with golog++.  If not, see <https://www.gnu.org/licenses/>.
**************************************************************************/

#include "scope.h"
#include "procedural.h"
#include "reference.h"
#include "execution.h"
#include "value.h"

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


/**
 * Transform nested field- and list accesses into a pair of the innermost subject (must be a
 * list- or compound-valued fluent) and a sequence of list indices and field names. Only one of either
 * @a fa or @a la may be set, depending on whether the outermost expression is a list access or a field access.
 *
 * @param fa The rightmost field access or nullptr
 * @param la The rightmost list access or nullptr
 * @return A reference to the fluent (either list- or compound-valued), and an eclipse list
 *         with a mixture of list indices and field names that sequentially index (deeply) into the
 *         returned fluent.
 */
std::pair<const Reference<Fluent> *, EC_word> traverse_mixed_field_access(const FieldAccess *fa, const ListAccess *la) {
	const Expression *sub;
	EC_word field_list = ::nil();

	do {
		if (fa) {
			field_list = ::list(fa->semantics().pl_field_name(), field_list);
			sub = &fa->subject();
		}
		else if (la) {
			field_list = ::list(la->semantics().pl_index(), field_list);
			sub = &la->subject();
		}
		else
			throw Bug("Invalid FieldAccess statement: " + fa->str());

		fa = dynamic_cast<const FieldAccess *>(sub);
		la = dynamic_cast<const ListAccess *>(sub);
	} while (sub->is_a<FieldAccess>() || sub->is_a<ListAccess>());

	return { dynamic_cast<const Reference<Fluent> *>(sub), std::move(field_list) };
};


Semantics<Assignment<FieldAccess>>::Semantics(const Assignment<FieldAccess> &ass)
: assignment_(ass)
, field_access_(ass.lhs())
{}


EC_word Semantics<Assignment<FieldAccess>>::plterm()
{
	std::pair<const Reference<Fluent> *, EC_word> fluent_access
		= traverse_mixed_field_access(&assignment_.lhs(), nullptr);

	return ::term(EC_functor("set", 2),
		fluent_access.first->semantics().plterm(),
		::term(EC_functor("gpp_mixed_assign", 3),
			fluent_access.second,
			assignment_.rhs().semantics().plterm(),
			fluent_access.first->semantics().plterm()
		)
	);
}



Semantics<Assignment<ListAccess>>::Semantics(const Assignment<ListAccess> &ass)
: assignment_(ass)
, field_access_(ass.lhs())
{}


EC_word Semantics<Assignment<ListAccess>>::plterm()
{
	std::pair<const Reference<Fluent> *, EC_word> fluent_access
		= traverse_mixed_field_access(nullptr, &assignment_.lhs());

	return ::term(EC_functor("set", 2),
		fluent_access.first->semantics().plterm(),
		::term(EC_functor("gpp_mixed_assign", 3),
			fluent_access.second,
			assignment_.rhs().semantics().plterm(),
			fluent_access.first->semantics().plterm()
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

	const Function *function = dynamic_cast<const Function *>(root_parent);
	if (!function)
	throw Bug(ret_.str() + " outside of function");

	if (function->type() != ret_.expression().type())
		throw ExpressionTypeMismatch(*function, ret_.expression());

	if (ret_.expression().type().is<BoolType>())
		return ret_.expression().semantics().plterm();
	else
		return ::term(EC_functor("=", 2),
			function->semantics().return_var(),
			ret_.expression().semantics().plterm()
		);
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
		pl_field_name(),
		field_access_.subject().semantics().plterm()
	);
}


EC_atom Semantics<FieldAccess>::pl_field_name()
{ return EC_atom(field_access_.field_name().c_str()); }


EC_word Semantics<FieldAccess>::field_assign(const Expression &value)
{
	return ::term(EC_functor("gpp_field_assign", 3),
		pl_field_name(),
		value.semantics().plterm(),
		field_access_.subject().semantics().plterm()
	);
}

void Semantics<FieldAccess>::set_lvalue(bool lvalue)
{ is_lvalue_ = lvalue; }



Semantics<ListAccess>::Semantics(const ListAccess &list_access)
: list_access_(list_access)
{}

EC_word Semantics<ListAccess>::pl_index()
{ return list_access_.index().semantics().plterm(); }


EC_word Semantics<ListAccess>::plterm()
{
	return ::term(EC_functor("gpp_list_access", 2),
		list_access_.subject().semantics().plterm(),
		pl_index()
	);
}



Semantics<ListPop>::Semantics(const ListPop &list_pop)
: list_pop_(list_pop)
{}

EC_word Semantics<ListPop>::plterm()
{
	string fn;
	switch(list_pop_.which_end()) {
	case ListOpEnd::BACK:
		fn = "gpp_list_pop_back";
		break;
	case ListOpEnd::FRONT:
		fn = "gpp_list_pop_front";
	}

	if (fn.empty())
		throw Bug("Invalid ListOpEnd Enum value: " + std::to_string(list_pop_.which_end()));

	return ::term(EC_functor("set", 2),
		list_pop_.list().semantics().plterm(),
		::term(EC_functor(fn.c_str(), 1),
			list_pop_.list().semantics().plterm()
		)
	);

}



Semantics<ListPush>::Semantics(const ListPush &list_push)
: list_push_(list_push)
{}

EC_word Semantics<ListPush>::plterm()
{
	string fn;
	switch(list_push_.which_end()) {
	case ListOpEnd::BACK:
		fn = "gpp_list_push_back";
		break;
	case ListOpEnd::FRONT:
		fn = "gpp_list_push_front";
	}

	if (fn.empty())
		throw Bug("Invalid ListOpEnd Enum value: " + std::to_string(list_push_.which_end()));

	return ::term(EC_functor("set", 2),
		list_push_.list().semantics().plterm(),
		::term(EC_functor(fn.c_str(), 2),
			list_push_.list().semantics().plterm(),
			list_push_.what().semantics().plterm()
		)
	);
}



Semantics<ListLength>::Semantics(const ListLength &list_length)
: list_length_(list_length)
{}

EC_word Semantics<ListLength>::plterm()
{
	return ::term(EC_functor("gpp_list_length", 1),
		list_length_.subject().semantics().plterm()
	);
}



}
