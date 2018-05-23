#include <memory>
#include <iostream>
#include <atomic>

#include <parser/grammar.h>

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


using namespace gologpp;

void test_objectmodel()
{
#ifdef GOLOGPP_TEST_OBJECTMODEL
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
#endif
}


void test_parser()
{
#ifdef GOLOGPP_TEST_PARSER
	std::string formula("$x || true && false");
	BooleanExpression *expr;
	boost::spirit::qi::phrase_parse(
		formula.cbegin(),
		formula.cend(),
		parser::FormulaParser()(boost::phoenix::ref(global_scope())),
		boost::spirit::ascii::space_type(),
		expr
	);

	std::string action_str("action wurst($y) { precondition: $x");
	shp<Action> action;
	std::cout << boost::spirit::qi::phrase_parse(
		action_str.cbegin(),
		action_str.cend(),
		parser::ActionParser(),
		boost::spirit::ascii::space_type(),
		action
	) << std::endl;
	std::cout << action->name() << std::endl;
#endif
}


void test_eclipse()
{
#ifdef GOLOGPP_TEST_ECLIPSE
	{
		ec_set_option_ptr(EC_OPTION_ECLIPSEDIR, (void *)ECLIPSE_DIR);
		ec_init();

		EC_ref wtf;

		EC_ref start;

		EC_ref X;
		wtf = ::term(EC_functor("wtf", 1), X);

		post_goal("assert(wtf(1))");
		post_goal("assert(wtf(2))");
		if (EC_resume(start) == EC_status::EC_succeed) {
			//start.cut_to();
		}


		post_goal(wtf);
		post_goal(::term(EC_functor("writeln", 1), X));
		EC_resume(start);
		post_goal(EC_atom("fail"));
		EC_resume(start);

		//start.cut_to();
	}
	ec_cleanup();
#endif
}



int main(int, const char **)
{
	test_objectmodel();
	test_parser();
	test_eclipse();
	return 0;
}





