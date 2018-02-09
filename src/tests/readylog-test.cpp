#include <gologpp/Formula.h>
#include <gologpp/Reference.h>

#include <gologpp/Action.h>
#include <gologpp/Fluent.h>
#include <gologpp/Procedure.h>

#include <backends/readylog/Execution.h>

int main(int, const char **) {
	using namespace gologpp;
	using namespace gologpp::generic;

	readylog::ExecutionContext &ctx = readylog::ExecutionContext::instance();

	shp<Fluent> on = ctx.add_fluent(Fluent::construct("on", std::vector<string>{"X", "Y"}));
	shp<Action> put = ctx.add_action(Action::construct("put", std::vector<string>{"X", "Y"}));
	put->set_precondition(Negation::construct(make_unique<Reference<Fluent>>(on, put->args(), put->scope()), put->scope()));
	put->set_effect(make_unique<EffectAxiom>(
	                    Reference<Action>(put, put->args(), put->scope()),
	                    Reference<Fluent>(on, put->args(), put->scope()),
	                    make_shared<AnyValue>(true)));

	ctx.compile(put->impl_cast<Action>().prim_action());
	ctx.compile(put->impl_cast<Action>().poss());
	ctx.compile(put->impl_cast<Action>().causes_val());
}

