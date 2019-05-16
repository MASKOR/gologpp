#include "reference.h"

namespace gologpp {


AbstractReference::AbstractReference()
{}

AbstractReference::~AbstractReference()
{}


void AbstractReference::ensure_consistent()
{
	if (!consistent())
		throw UserError("Inconsistent reference: " + str());
}

}
