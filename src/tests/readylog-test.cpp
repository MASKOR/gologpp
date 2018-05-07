#include <model/formula.h>
#include <model/reference.h>

#include <model/action.h>
#include <model/fluent.h>
#include <model/procedural.h>

#include <impl/readylog/action.h>
#include <impl/readylog/effect_axiom.h>
#include <impl/readylog/formula.h>
#include <impl/readylog/execution.h>
#include <impl/readylog/reference.h>
#include <impl/readylog/fluent.h>
#include <impl/readylog/atoms.h>

#include <impl/readylog/implementation.h>

#include <memory>

#include <iostream>

int main(int, const char **) {
	using namespace gologpp;

	EclipseContext &ctx = EclipseContext::instance();

	shp<Fluent<BooleanExpression>> on = ctx.add_fluent<BooleanExpression>(
		"on",
		std::vector<string>{"X", "Y"},
		std::make_unique<Constant<BooleanExpression>>("false")
	);
	on->declare_variable<NumericExpression>("X");
	on->declare_variable<NumericExpression>("Y");

	shp<Action> put = ctx.add_action("put", std::vector<string>{"X", "Y"});
	put->declare_argument<NumericExpression>("X");
	put->declare_argument<NumericExpression>("Y");

	put->set_precondition(Negation(
		std::make_unique<Reference<Fluent<BooleanExpression>>>(
			on,
			put->scope().variables(put->arg_names()), put->scope()),
			put->scope()
		)
	);

	put->add_effect(EffectAxiom<BooleanExpression>(
		Reference<Action>(put, put->scope().variables(put->arg_names()), put->scope()),
		std::make_unique<Constant<BooleanExpression>>("true"),
	    Reference<Fluent<BooleanExpression>>(on, put->scope().variables(put->arg_names()), put->scope()),
	    std::make_unique<Constant<BooleanExpression>>("true"))
	);

	vector<unique_ptr<Statement>> code;
	code.push_back(unique_ptr<Statement>(
		new Reference<Action>(
			put,
			{
				std::make_shared<Constant<NumericExpression>>("1"),
				std::make_shared<Constant<NumericExpression>>("2")
			},
			global_scope()
		)
	));
	ctx.run(Block(std::move(code), global_scope() ));
}

