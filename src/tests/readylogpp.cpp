#include <gologpp/Action.h>
#include <gologpp/Fluent.h>
#include <gologpp/Formula.h>
#include <gologpp/Reference.h>
#include <backends/readylog/Execution.h>

int main(int, const char **) {
	using namespace gologpp;
	using namespace gologpp::generic;

	readylog::ExecutionContext &ctx = readylog::ExecutionContext::instance();

	shp<Fluent> on = ctx.add_fluent(Fluent("on", {"X", "Y"}));
	shp<Action> put = ctx.add_action(Action("put", {"X", "Y"}));
	put->set_precondition(Negation(make_unique<Reference<Fluent>>(on, put->args(), put->scope()), put->scope()));
	put->set_effect(make_unique<EffectAxiom>(
	                    Reference<Action>(put, put->args(), put->scope()),
	                    Reference<Fluent>(on, put->args(), put->scope()),
	                    make_shared<Value<bool>>(true)));


}