#include "domain.h"

namespace gologpp {

AbstractDomain::AbstractDomain(const string &name, bool implicit)
: Name(name)
, implicit_(implicit)
{}

}
