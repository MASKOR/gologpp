#include "Procedure.h"
#include <gologpp/Procedure.h>

namespace gologpp {

Implementation<generic::Procedure>::Implementation(const generic::Procedure &proc)
: procedure_(proc)
{}


EC_word Implementation<generic::Procedure>::term()
{
	return EC_atom("false");
}


Implementation<generic::Block>::Implementation(const generic::Block &b)
: block_(b)
{}


EC_word Implementation<generic::Block>::term()
{
	return EC_atom("false");
}


Implementation<generic::Choose>::Implementation(const generic::Choose &c)
: choose_(c)
{}


EC_word Implementation<generic::Choose>::term()
{
	return EC_atom("false");
}


Implementation<generic::Conditional>::Implementation(const generic::Conditional &c)
: conditional_(c)
{}


EC_word Implementation<generic::Conditional>::term()
{
	return EC_atom("false");
}


Implementation<generic::Assignment>::Implementation(const generic::Assignment &proc)
: assignment_(proc)
{}


EC_word Implementation<generic::Assignment>::term()
{
	return EC_atom("false");
}


Implementation<generic::Pick>::Implementation(const generic::Pick &)
{}


EC_word Implementation<generic::Pick>::term()
{
	return EC_atom("false");
}


Implementation<generic::Search>::Implementation(const generic::Search &)
{}


EC_word Implementation<generic::Search>::term()
{
	return EC_atom("false");
}


Implementation<generic::Test>::Implementation(const generic::Test &)
{}


EC_word Implementation<generic::Test>::term()
{
	return EC_atom("false");
}


Implementation<generic::While>::Implementation(const generic::While &)
{}


EC_word Implementation<generic::While>::term()
{
	return EC_atom("false");
}




}
