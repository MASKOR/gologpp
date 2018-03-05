#include "Procedure.h"
#include <gologpp/Procedure.h>

#include "Scope.h"

namespace gologpp {

Implementation<Procedure>::Implementation(const Procedure &proc)
: procedure_(proc)
{}


EC_word Implementation<Procedure>::definition()
{
	procedure_.scope().impl_cast<Scope>().init_vars();
	return ::term(EC_functor("proc", 2),
		term(),
		procedure_.block().impl_cast<Block>().term()
	);
}


EC_word Implementation<Procedure>::term()
{
	return ::term(EC_functor(procedure_.name().c_str(), procedure_.arity()),
		procedure_.scope().impl_cast<Scope>().variables(procedure_.args())
	);
}


Implementation<Block>::Implementation(const Block &b)
: block_(b)
{}


EC_word Implementation<Block>::term()
{
	EC_word tail = ::nil();
	for (const unique_ptr<Statement> &stmt : block_.elements())
		tail = ::list(stmt->impl().term(), tail);
	return tail;
}


Implementation<Choose>::Implementation(const Choose &c)
: choose_(c)
{}


EC_word Implementation<Choose>::term()
{
	EC_word tail = ::nil();
	for (const Block &block : choose_.alternatives())
		tail = ::list(block.impl().term(), tail);
	return ::term(EC_functor("nondet", 1), tail);
}


Implementation<Conditional>::Implementation(const Conditional &c)
: conditional_(c)
{}


EC_word Implementation<Conditional>::term()
{
	return ::term(EC_functor("if", 3),
		dynamic_cast<ReadylogImplementation &>(conditional_.condition().implementation()).term(),
		conditional_.block_true().impl().term(),
		conditional_.block_false().impl().term()
	);
}


Implementation<Assignment>::Implementation(const Assignment &proc)
: assignment_(proc)
{}


EC_word Implementation<Assignment>::term()
{
	return EC_atom("false");
}


Implementation<Pick>::Implementation(const Pick &)
{}


EC_word Implementation<Pick>::term()
{
	return EC_atom("false");
}


Implementation<Search>::Implementation(const Search &)
{}


EC_word Implementation<Search>::term()
{
	return EC_atom("false");
}


Implementation<Test>::Implementation(const Test &)
{}


EC_word Implementation<Test>::term()
{
	return EC_atom("false");
}


Implementation<While>::Implementation(const While &)
{}


EC_word Implementation<While>::term()
{
	return EC_atom("false");
}




}
