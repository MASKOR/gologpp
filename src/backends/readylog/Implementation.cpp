#include "Implementation.h"
#include <gologpp/Execution.h>
#include "Execution.h"

namespace gologpp {

void ReadylogExpression::apply_to(generic::ExecutionContext &c)
{ apply_to(dynamic_cast<readylog::ExecutionContext &>(c)); }

}