#include "Procedure.h"
#include <gologpp/Procedure.h>

namespace gologpp {

Implementation<Procedure>::Implementation(const Procedure &proc)
: procedure_(proc)
{}


EC_word Implementation<Procedure>::term()
{
	return EC_atom("false");
}


Implementation<Block>::Implementation(const Block &b)
: block_(b)
{}


EC_word Implementation<Block>::term()
{
	return EC_atom("false");
}


Implementation<Choose>::Implementation(const Choose &c)
: choose_(c)
{}


EC_word Implementation<Choose>::term()
{
	return EC_atom("false");
}


Implementation<Conditional>::Implementation(const Conditional &c)
: conditional_(c)
{}


EC_word Implementation<Conditional>::term()
{
	return EC_atom("false");
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
