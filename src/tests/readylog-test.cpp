#include <gologpp/Formula.h>
#include <gologpp/Reference.h>

#include <gologpp/Action.h>
#include <gologpp/Fluent.h>
#include <gologpp/Procedure.h>

#include <backends/readylog/Action.h>
#include <backends/readylog/EffectAxiom.h>
#include <backends/readylog/Formula.h>
#include <backends/readylog/Execution.h>
#include <backends/readylog/Reference.h>
#include <backends/readylog/Fluent.h>
#include <backends/readylog/atoms.h>

#include <backends/readylog/Implementation.h>

#include <memory>

#include <iostream>

int main(int, const char **) {
	using namespace gologpp;

	EclipseContext &ctx = EclipseContext::instance();

	shp<Fluent> on = ctx.add_fluent(Fluent("on", std::vector<string>{"X", "Y"}));
	shp<Action> put = ctx.add_action(Action("put", std::vector<string>{"X", "Y"}));
	put->set_precondition(Negation(
		std::unique_ptr<BooleanExpression>(new Reference<Fluent>(on, put->args(), put->scope())), put->scope())
	);
	put->set_effect(EffectAxiom(
	                    Reference<Action>(put, put->args(), put->scope()),
	                    Reference<Fluent>(on, put->args(), put->scope()),
	                    make_shared<AnyValue>(AnyValue(true))));

	ReadylogImplementor implementor;

	on->implement(implementor);
	put->implement(implementor);

	ctx.compile(put->impl_cast<Action>().poss());
	ctx.compile(put->impl_cast<Action>().prim_action());
	ctx.compile(put->impl_cast<Action>().causes_val());
}

