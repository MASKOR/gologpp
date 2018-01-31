#include "Execution.h"
#include "Fluent.h"
#include "Action.h"
#include "Procedure.h"

namespace gologpp {
namespace generic {

using namespace std;


shared_ptr<Fluent> ExecutionContext::fluent(const string &name, arity_t arity)
{ return get_global(fluents_, name, arity); }

shared_ptr<Action> ExecutionContext::action(const string &name, arity_t arity)
{ return get_global(actions_, name, arity); }

shared_ptr<Procedure> ExecutionContext::procedure(const string &name, arity_t arity)
{ return get_global(procedures_, name, arity); }


void ExecutionContext::init()
{
	for (auto entry : fluents_)
		entry.second->implementation().apply_to(*this);
	for (auto entry : actions_)
		entry.second->implementation().apply_to(*this);
	for (auto entry : procedures_)
		entry.second->implementation().apply_to(*this);
}

}
}