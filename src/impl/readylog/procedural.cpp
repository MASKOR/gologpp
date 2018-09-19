#include "scope.h"
#include "procedural.h"
#include "atoms.h"
#include "reference.h"
#include "execution.h"
#include <model/procedural.h>


namespace gologpp {



EC_word Implementation<AbstractFunction>::term()
{
	if (function_.arity() > 0)
		return ::term(EC_functor(function_.name().c_str(), function_.arity()),
			to_ec_words(function_.args())
		);
	else
		return EC_atom(function_.name().c_str());
}



Implementation<Block>::Implementation(const Block &b)
: block_(b)
{}


EC_word Implementation<Block>::term()
{
	block_.scope().implementation().init_vars();

	current_program_ = to_ec_list(block_.elements(), block_.elements().begin());
	return current_program_;
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
	choose_.scope().implementation().init_vars();
	EC_word tail = ::nil();
	for (const unique_ptr<Statement> &stmt : choose_.alternatives())
		tail = ::list(stmt->implementation().term(), tail);
	return ::term(EC_functor("nondet", 1), tail);
}



Implementation<Conditional>::Implementation(const Conditional &c)
: conditional_(c)
{}


EC_word Implementation<Conditional>::term()
{
	return ::term(EC_functor("lif", 3),
		conditional_.condition().implementation().term(),
		conditional_.block_true().implementation().term(),
		conditional_.block_false().implementation().term()
	);
}



Implementation<Search>::Implementation(const Search &search)
: search_(search)
{}


EC_word Implementation<Search>::term()
{
	return ::term(EC_functor("search", 1),
		search_.statement().implementation().term()
	);
}



Implementation<Solve>::Implementation(const Solve &solve)
: solve_(solve)
{}


EC_word Implementation<Solve>::term()
{
	return ::term(EC_functor("solve", 3),
		solve_.statement().implementation().term(),
		solve_.horizon().implementation().term(),
		solve_.reward().implementation().term()
	);
}



Implementation<Test>::Implementation(const Test &test)
: test_(test)
{}


EC_word Implementation<Test>::term()
{
	return test_.expression().implementation().term();
}



Implementation<While>::Implementation(const While &w)
: while_(w)
{}


EC_word Implementation<While>::term()
{
	return ::term(EC_functor("while", 2),
		while_.expression().implementation().term(),
		while_.statement().implementation().term()
	);
}



template<>
EC_word Implementation<Return<BooleanExpression>>::term() {
	return ret_.expression().implementation().term();
}



}
