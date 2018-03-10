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

	shp<Fluent<BooleanExpression>> on = ctx.add_fluent<BooleanExpression>(
		"on",
		std::vector<string>{"X", "Y"},
		make_unique<Constant<BooleanExpression>>("false")
	);
	on->declare_variable<ValueExpression>("X");
	on->declare_variable<ValueExpression>("Y");

	shp<Action> put = ctx.add_action(Action("put", std::vector<string>{"X", "Y"}));
	put->declare_variable<ValueExpression>("X");
	put->declare_variable<ValueExpression>("Y");

	put->set_precondition(Negation(
		make_unique<Reference<Fluent<BooleanExpression>>>(
			on,
			put->scope().variables(put->arg_names()), put->scope()),
			put->scope()
		)
	);

	put->set_effect(EffectAxiom<BooleanExpression>(
		Reference<Action>(put, put->scope().variables(put->arg_names()), put->scope()),
	    Reference<Fluent<BooleanExpression>>(on, put->scope().variables(put->arg_names()), put->scope()),
	    make_unique<Constant<BooleanExpression>>("true"))
	);

	ReadylogImplementor implementor;

	on->implement(implementor);
	put->implement(implementor);

	ctx.compile(put->implementation().poss());
	ctx.compile(put->implementation().prim_action());
	ctx.compile(put->implementation().causes_val());
}

