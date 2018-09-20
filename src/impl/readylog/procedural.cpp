#include "scope.h"
#include "procedural.h"
#include "atoms.h"
#include "reference.h"
#include "execution.h"
#include <model/procedural.h>


namespace gologpp {



EC_word Semantics<AbstractFunction>::plterm()
{
	if (function_.arity() > 0)
		return ::term(EC_functor(function_.name().c_str(), function_.arity()),
			to_ec_words(function_.args())
		);
	else
		return EC_atom(function_.name().c_str());
}



Semantics<Block>::Semantics(const Block &b)
: block_(b)
{}


EC_word Semantics<Block>::plterm()
{
	block_.scope().semantics().init_vars();

	if (dynamic_cast<const AbstractFunction *>(block_.parent())) {
		if (block_.elements().size() == 1)
			current_program_ = block_.elements()[0]->semantics().plterm();
		else
			throw std::runtime_error("Functions must contain exactly one return statement and nothing else.");
	}
	else
		current_program_ = to_ec_list(block_.elements(), block_.elements().begin());

	return current_program_;
}


EC_word Semantics<Block>::current_program()
{
	EC_word rv = ::newvar();
	rv.unify(current_program_);
	return rv;
}


void Semantics<Block>::set_current_program(EC_word e)
{ current_program_ = e; }



Semantics<Choose>::Semantics(const Choose &c)
: choose_(c)
{}


EC_word Semantics<Choose>::plterm()
{
	choose_.scope().semantics().init_vars();
	EC_word tail = ::nil();
	for (const unique_ptr<Statement> &stmt : choose_.alternatives())
		tail = ::list(stmt->semantics().plterm(), tail);
	return ::term(EC_functor("nondet", 1), tail);
}



Semantics<Conditional>::Semantics(const Conditional &c)
: conditional_(c)
{}


EC_word Semantics<Conditional>::plterm()
{
	return ::term(EC_functor("lif", 3),
		conditional_.condition().semantics().plterm(),
		conditional_.block_true().semantics().plterm(),
		conditional_.block_false().semantics().plterm()
	);
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
	return test_.expression().semantics().plterm();
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



template<>
EC_word Semantics<Return<BooleanExpression>>::plterm()
{ return ret_.expression().semantics().plterm(); }



}
