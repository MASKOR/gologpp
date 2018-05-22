#include "procedural.h"
#include <model/procedural.h>

#include "scope.h"

namespace gologpp {

Implementation<Procedure>::Implementation(const Procedure &proc)
: procedure_(proc)
{}


EC_word Implementation<Procedure>::definition()
{
	procedure_.scope().implementation().init_vars();
	return ::term(EC_functor("proc", 2),
		term(),
		procedure_.block().implementation().term()
	);
}


EC_word Implementation<Procedure>::term()
{
	return ::term(EC_functor(procedure_.name().c_str(), procedure_.arity()),
		procedure_.scope().implementation().variables(procedure_.args())
	);
}


Implementation<AbstractFunction>::Implementation(const AbstractFunction &function)
: function_(function)
{}


EC_word Implementation<AbstractFunction>::term()
{
	return ::term(EC_functor(function_.name().c_str(), function_.arity()),
	function_.scope().implementation().variables(function_.args())
	);
}


EC_word Implementation<AbstractFunction>::definition()
{
	function_.scope().implementation().init_vars();
	return ::term(EC_functor("function", 3),
		term(),
		return_var_,
		function_.block().implementation().term()
	);
}


EC_word Implementation<AbstractFunction>::return_var()
{ return return_var_; }




Implementation<Block>::Implementation(const Block &b)
: block_(b)
{}


EC_word Implementation<Block>::term()
{
	EC_word tail = ::nil();
	for (const unique_ptr<Statement> &stmt : block_.elements())
		tail = ::list(stmt->implementation().term(), tail);
	current_program_ = tail;
	return tail;
}


EC_word Implementation<Block>::current_program()
{
	EC_word rv = ::newvar();
	rv.unify(current_program_);
	return rv;
}


void Implementation<Block>::set_current_program(EC_word e)
{ current_program_ = e; }


Implementation<Choose>::Implementation(const Choose &c)
: choose_(c)
{}


EC_word Implementation<Choose>::term()
{
	EC_word tail = ::nil();
	for (const Block &block : choose_.alternatives())
		tail = ::list(block.implementation().term(), tail);
	return ::term(EC_functor("nondet", 1), tail);
}


Implementation<Conditional>::Implementation(const Conditional &c)
: conditional_(c)
{}


EC_word Implementation<Conditional>::term()
{
	return ::term(EC_functor("if", 3),
		conditional_.condition().implementation().term(),
		conditional_.block_true().implementation().term(),
		conditional_.block_false().implementation().term()
	);
}


Implementation<Pick>::Implementation(const Pick &pick)
: pick_(pick)
{}


EC_word Implementation<Pick>::term()
{
	return ::term(EC_functor("pick", 2),
		pick_.variable().implementation().term(),
		pick_.block().implementation().term()
	);
}


Implementation<Search>::Implementation(const Search &search)
: search_(search)
{}


EC_word Implementation<Search>::term()
{
	return EC_atom("fail");
}


Implementation<Test>::Implementation(const Test &test)
: test_(test)
{}


EC_word Implementation<Test>::term()
{
	return EC_atom("fail");
}


Implementation<While>::Implementation(const While &w)
: while_(w)
{}


EC_word Implementation<While>::term()
{
	return EC_atom("fail");
}




}
