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

	Scope *on_scope = new Scope(nullptr);
	on_scope->variable<NumericExpression>("X");
	NumericFluent *on = new NumericFluent(on_scope,
		"on",
		on_scope->variables({"X"}),
		std::make_unique<NumericConstant>(0)
	);

	Scope *put_scope = new Scope(nullptr);
	put_scope->variable<NumericExpression>("X");
	put_scope->variable<NumericExpression>("Y");
	Action *put = new Action(put_scope, "put", put_scope->variables({"X", "Y"}));

	put->set_precondition(new Comparison(
		new Reference<NumericFluent>("on", *put_scope, vector<Expression *>{ put_scope->variable<NumericExpression>("X")->ref() }
		),
		ComparisonOperator::neq,
		new Reference<NumericVariable>(put_scope->variable<NumericExpression>("Y"), *put_scope),
		*put_scope
	));

	Reference<NumericFluent> *on_ref = on->ref(
		*put_scope,
		{ unique_ptr<Expression>(put_scope->variable<NumericExpression>("X")->ref()) }
	);
	put->add_effect(new EffectAxiom<NumericExpression>(
		put->shared(),
		new BooleanConstant("true"),
		on_ref,
	    put_scope->variable<NumericExpression>("Y")->ref()
	) );

	vector<Statement *> code;
	code.push_back(
		put->ref(
			global_scope(),
			{
				std::make_unique<Constant<NumericExpression>>("1"),
				std::make_unique<Constant<NumericExpression>>("2")
			}
		)
	);
	ctx.run(Block(std::move(code), global_scope() ));
#endif
}


void test_parser()
{
#ifdef GOLOGPP_TEST_PARSER
	std::string formula("?x || true && false");
	BooleanExpression *expr;
	boost::spirit::qi::phrase_parse(
		formula.cbegin(),
		formula.cend(),
		parser::BooleanExpressionParser()(boost::phoenix::ref(global_scope())),
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





