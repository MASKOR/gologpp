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

}
