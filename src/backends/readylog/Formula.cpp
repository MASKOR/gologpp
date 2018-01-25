#include "Formula.h"
#include <gologpp/Formula.h>

namespace gologpp {

Implementation<generic::Negation>::Implementation(const generic::Negation &neg)
: negation_(neg)
{}


EC_word Implementation<generic::Negation>::term()
{ return dynamic_cast<ReadylogExpression &>(negation_.implementation()).term(); }


}