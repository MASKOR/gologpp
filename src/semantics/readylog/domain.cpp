#include "domain.h"
#include "constant.h"

namespace gologpp {

Semantics<Domain>::Semantics(const Domain &domain)
: domain_(domain)
{}

EC_word Semantics<Domain>::plterm()
{ return to_ec_list(domain_.elements()); }


}
