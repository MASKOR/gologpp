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

#include "value.h"
#include "scope.h"
#include "procedural.h"
#include "reference.h"
#include "execution.h"
#include "action.h"
#include "history.h"
#include "plan.h"

#include <model/procedural.h>



extern "C"
int p_eval_exog_function()
{
	using namespace gologpp;

	EC_word arg_fname = EC_arg(1);
	EC_atom atom_fname;
	if (arg_fname.is_atom(&atom_fname) != EC_succeed)
		throw Bug(string(__func__) + ": Invalid call");
	string fname = Name::demangle(atom_fname.name());
	shared_ptr<ExogFunction> efunc = global_scope().lookup_identifier<ExogFunction>(fname);
	if (!efunc)
		throw Bug(string(__func__) + ": No exog_function by the name " + fname);

	EC_word arg_type = EC_arg(2);
	EC_atom atom_type;
	if (arg_type.is_atom(&atom_type) != EC_succeed)
		throw Bug(string(__func__) + ": Invalid call");
	string rtype_name = Type::demangle_name(atom_type.name());
	shared_ptr<const Type> ret_type = global_scope().lookup_type(rtype_name);
	if (!ret_type)
		throw Bug(string(__func__) + ": No type by the name " + rtype_name);

	EC_word arg_fargs = EC_arg(3);
	EC_word head, tail;
	std::unordered_map<string, Value> backend_args;
	arity_t idx = 0;
	while (arg_fargs.is_list(head, tail) == EC_succeed) {
		backend_args.emplace(efunc->parameter(idx++)->name(), pl_term_to_value(head));
		arg_fargs = tail;
	}

	Value rv = ReadylogContext::instance().backend().eval_exog_function(
		*ret_type,
		efunc->name(),
		backend_args
	);
	rv.attach_semantics(ReadylogContext::instance().semantics_factory());

	return rv.semantics().plterm().unify(EC_arg(4));
};



namespace gologpp {


EC_word Semantics<Function>::plterm()
{
	if (element().arity() > 0)
		return ::term(EC_functor(element().mangled_name().c_str(), element().arity()),
			to_ec_words(element().params()).data()
		);
	else
		return EC_atom(element().mangled_name().c_str());
}


EC_word Semantics<Function>::definition()
{
	if (element().type().is<BoolType>()) {
		return ::term(EC_functor("proc", 2),
			plterm(),
			element().definition().semantics().plterm()
		);
	}
	else {
		return_var_ = ::newvar();

		return ::term(EC_functor("function", 3),
			plterm(),
			return_var_,
			::term(EC_functor("=", 2),
				return_var_,
				element().definition().semantics().plterm()
			)
		);
	}
}


EC_word Semantics<Function>::return_var()
{
	if (element().type().is<VoidType>())
		throw Bug("Attempt to get a return var for a procedure");

	return return_var_;
}



EC_word Semantics<ExogFunction>::plterm()
{
	if (element().arity() > 0)
		return ::term(EC_functor(element().mangled_name().c_str(), element().arity()),
			to_ec_words(element().params()).data()
		);
	else
		return EC_atom(element().mangled_name().c_str());
}


EC_word Semantics<ExogFunction>::definition()
{
	if (element().type().is<BoolType>()) {
		return ::term(EC_functor("proc", 2),
			plterm(),
			::term(EC_functor("eval_exog_function", 4),
				EC_atom(element().mangled_name().c_str()),
				EC_atom(element().type().mangled_name().c_str()),
				to_ec_list(to_ec_words(element().params())),
				EC_atom("true")
			)
		);
	}
	else {
		EC_word rv = ::newvar();

		return ::term(EC_functor("function", 3),
			plterm(),
			rv,
			::term(EC_functor("eval_exog_function", 4),
				EC_atom(element().mangled_name().c_str()),
				EC_atom(element().type().mangled_name().c_str()),
				to_ec_list(to_ec_words(element().params())),
				rv
			)
		);
	}
}



EC_word Semantics<Procedure>::plterm()
{
	if (element().arity() > 0)
		return ::term(EC_functor(element().mangled_name().c_str(), element().arity()),
			to_ec_words(element().params()).data()
		);
	else
		return EC_atom(element().mangled_name().c_str());
}


EC_word Semantics<Procedure>::definition()
{
	return ::term(EC_functor("proc", 2),
		plterm(),
		element().definition().semantics().plterm()
	);
}







template<>
EC_word Semantics<Block>::plterm()
{
	EC_word rv;

	const Function *parent_fn = dynamic_cast<const Function *>(element().parent());
	if (parent_fn && !parent_fn->type().is<VoidType>()) {
		if (element().elements().size() == 1)
			rv = element().elements()[0]->semantics().plterm();
		else
			throw std::runtime_error("Functions must contain exactly one return statement and nothing else.");
	}
	else
		rv = to_ec_list(element().elements(), element().elements().begin());

	return rv;
}



template<>
EC_word Semantics<Choose>::plterm()
{ return ::term(EC_functor("nondet", 1), to_ec_list(element().alternatives())); }



template<>
EC_word Semantics<Conditional<Instruction>>::plterm()
{
	return ::term(EC_functor("if", 3),
		element().condition().semantics().plterm(),
		element().block_true().semantics().plterm(),
		element().block_false().semantics().plterm()
	);
}


template<>
EC_word Semantics<Conditional<Expression>>::plterm()
{
	return ::term(EC_functor("func_if", 3),
		element().condition().semantics().plterm(),
		element().block_true().semantics().plterm(),
		element().block_false().semantics().plterm()
	);
}



template<>
EC_word Semantics<Concurrent>::plterm()
{ return to_ec_term("pconc", element().procs()); }



template<>
EC_word Semantics<Assignment<Reference<Fluent>>>::plterm()
{
	return ::term(EC_functor("set", 2),
		element().lhs().semantics().plterm(),
		element().rhs().semantics().plterm()
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
			field_list = ::list(fa->special_semantics().pl_field_name(), field_list);
			sub = &fa->subject();
		}
		else if (la) {
			field_list = ::list(la->special_semantics().pl_index(), field_list);
			sub = &la->subject();
		}
		else
			throw Bug("Invalid FieldAccess statement: " + fa->str());

		fa = dynamic_cast<const FieldAccess *>(sub);
		la = dynamic_cast<const ListAccess *>(sub);
	} while (sub->is_a<FieldAccess>() || sub->is_a<ListAccess>());

	return { dynamic_cast<const Reference<Fluent> *>(sub), std::move(field_list) };
};



template<>
EC_word Semantics<Assignment<FieldAccess>>::plterm()
{
	std::pair<const Reference<Fluent> *, EC_word> fluent_access
		= traverse_mixed_field_access(&element().lhs(), nullptr);

	return ::term(EC_functor("set", 2),
		fluent_access.first->semantics().plterm(),
		::term(EC_functor("gpp_mixed_assign", 3),
			fluent_access.second,
			element().rhs().semantics().plterm(),
			fluent_access.first->semantics().plterm()
		)
	);
}



template<>
EC_word Semantics<Assignment<ListAccess>>::plterm()
{
	std::pair<const Reference<Fluent> *, EC_word> fluent_access
		= traverse_mixed_field_access(nullptr, &element().lhs());

	return ::term(EC_functor("set", 2),
		fluent_access.first->semantics().plterm(),
		::term(EC_functor("gpp_mixed_assign", 3),
			fluent_access.second,
			element().rhs().semantics().plterm(),
			fluent_access.first->semantics().plterm()
		)
	);
}

Semantics<Pick>::Semantics(const Pick &pick, ReadylogContext &context)
: GeneralSemantics<Pick>(pick, context)
{
	if (element().domain().empty())
		throw std::runtime_error("ReadyLog requires a domain for pick()!");
}

EC_word Semantics<Pick>::plterm()
{
	// Make sure the `pick'ed variable is a Golog variable
	// No init_vars() is needed in this case.
	{ GologVarMutator guard(element().variable().special_semantics());
		return ::term(EC_functor("pickBest", 3),
			element().variable().special_semantics().plterm(),
			to_ec_list(element().domain(), element().domain().begin()),
			element().statement().semantics().plterm()
		);
	}
}



template<>
EC_word Semantics<Search>::plterm()
{
	return ::term(EC_functor("search", 1),
		element().statement().semantics().plterm()
	);
}



template<>
EC_word Semantics<Solve>::plterm()
{
	return ::term(EC_functor("solve", 3),
		element().statement().semantics().plterm(),
		element().horizon().semantics().plterm(),
		element().reward().semantics().plterm()
	);
}



EC_word Semantics<Test>::plterm()
{
	if (is_plan_marker_)
		return ::term(EC_functor("?", 1), marker_cond_);
	else
		return ::term(EC_functor("?", 1),
			element().expression().semantics().plterm()
		);
}


void Semantics<Test>::make_plan_marker(EC_word cond)
{
	marker_cond_ = cond;
	is_plan_marker_ = true;
}



template<>
EC_word Semantics<While>::plterm()
{
	return ::term(EC_functor("while", 2),
		element().expression().semantics().plterm(),
		element().statement().semantics().plterm()
	);
}



template<>
EC_word Semantics<DurativeCall>::plterm()
{
	return ::term(EC_functor(to_string(element().hook()).c_str(), 1),
		reference_term(element().action())
	);
}



EC_word Semantics<FieldAccess>::plterm()
{
	return ::term(EC_functor("gpp_field_value", 2),
		pl_field_name(),
		element().subject().semantics().plterm()
	);
}


EC_atom Semantics<FieldAccess>::pl_field_name()
{ return EC_atom(("#" + element().field_name()).c_str()); }


EC_word Semantics<FieldAccess>::field_assign(const Expression &value)
{
	return ::term(EC_functor("gpp_field_assign", 3),
		pl_field_name(),
		value.semantics().plterm(),
		element().subject().semantics().plterm()
	);
}



EC_word Semantics<ListAccess>::pl_index()
{ return element().index().semantics().plterm(); }


EC_word Semantics<ListAccess>::plterm()
{
	return ::term(EC_functor("gpp_list_access", 2),
		element().subject().semantics().plterm(),
		pl_index()
	);
}



template<>
EC_word Semantics<ListPop>::plterm()
{
	string fn;
	switch(element().which_end()) {
	case ListOpEnd::BACK:
		fn = "gpp_list_pop_back";
		break;
	case ListOpEnd::FRONT:
		fn = "gpp_list_pop_front";
	}

	if (fn.empty())
		throw Bug("Invalid ListOpEnd Enum value: " + std::to_string(element().which_end()));

	return ::term(EC_functor("set", 2),
		element().list().semantics().plterm(),
		::term(EC_functor(fn.c_str(), 1),
			element().list().semantics().plterm()
		)
	);

}



template<>
EC_word Semantics<ListPush>::plterm()
{
	string fn;
	switch(element().which_end()) {
	case ListOpEnd::BACK:
		fn = "gpp_list_push_back";
		break;
	case ListOpEnd::FRONT:
		fn = "gpp_list_push_front";
	}

	if (fn.empty())
		throw Bug("Invalid ListOpEnd Enum value: " + std::to_string(element().which_end()));

	return ::term(EC_functor("set", 2),
		element().list().semantics().plterm(),
		::term(EC_functor(fn.c_str(), 2),
			element().list().semantics().plterm(),
			element().what().semantics().plterm()
		)
	);
}



template<>
EC_word Semantics<ListLength>::plterm()
{
	return ::term(EC_functor("gpp_list_length", 1),
		element().subject().semantics().plterm()
	);
}



template<>
EC_word Semantics<During>::plterm()
{
	EC_word start = ::term(EC_functor(to_string(Transition::Hook::START).c_str(), 1),
		reference_term(element().action_call())
	);
	EC_word end = ::term(EC_functor(to_string(Transition::Hook::END).c_str(), 1),
		reference_term(element().action_call())
	);
	EC_word if_failed = ::term(EC_functor("if", 2),
		::term(EC_functor("=", 2),
			::term(EC_functor("state", 1), reference_term(element().action_call())),
			::EC_atom(to_string(Activity::State::FAILED).c_str())
		),
		element().on_fail().semantics().plterm()
	);
	EC_word if_cancelled = ::term(EC_functor("if", 2),
		::term(EC_functor("=", 2),
			::term(EC_functor("state", 1), reference_term(element().action_call())),
			::EC_atom(to_string(Activity::State::CANCELLED).c_str())
		),
		element().on_cancel().semantics().plterm()
	);

	return make_ec_list( {
		start,
		element().parallel_block().semantics().plterm(),
		end,
		if_failed,
		if_cancelled
	} );
}


}
