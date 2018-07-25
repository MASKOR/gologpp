#include <memory>
#include <iostream>
#include <atomic>

#include <model/formula.h>
#include <model/reference.h>

#include <model/action.h>
#include <model/effect_axiom.h>
#include <model/fluent.h>
#include <model/procedural.h>

#ifdef GOLOGPP_TEST_READYLOG
#include <impl/readylog/action.h>
#include <impl/readylog/effect_axiom.h>
#include <impl/readylog/formula.h>
#include <impl/readylog/execution.h>
#include <impl/readylog/reference.h>
#include <impl/readylog/fluent.h>
#include <impl/readylog/atoms.h>
#include <impl/readylog/implementation.h>
#endif

#ifdef GOLOGPP_TEST_PARSER
#include <parser/parser.h>
#endif


#ifdef GOLOGPP_TEST_ECLIPSE
#include <eclipseclass.h>
#endif


using namespace gologpp;

void test_objectmodel()
{
#ifdef GOLOGPP_TEST_OBJECTMODEL
	Scope *on_scope = new Scope(nullptr);
	on_scope->variable<NumericExpression>("X");
	NumericFluent *on = new NumericFluent(on_scope,
		"on",
		on_scope->variables({"X"}),
		new NumericConstant(0)
	);
	global_scope().register_global(on);
	shared_ptr<NumericFluent> on_shared = global_scope().lookup_global<NumericFluent>({"on", 1});

	Scope *put_scope = new Scope(nullptr);
	put_scope->variable<NumericExpression>("X");
	put_scope->variable<NumericExpression>("Y");
	Action *put = new Action(put_scope, "put", put_scope->variables({"X", "Y"}));
	global_scope().register_global(put);

	put->set_precondition(new Comparison(
		new Reference<NumericFluent>("on", *put_scope, vector<Expression *>{ put_scope->variable<NumericExpression>("X")->ref(*put_scope) }
		),
		ComparisonOperator::NEQ,
		new Reference<NumericVariable>(put_scope->variable<NumericExpression>("Y"), *put_scope),
		*put_scope
	));

	{ vector<unique_ptr<Expression>> arg;
		arg.emplace_back(put_scope->variable<NumericExpression>("X")->ref(*put_scope));

		Reference<NumericFluent> *on_ref = on->ref<NumericFluent>(
			*put_scope,
			std::move(arg)
		);
		put->add_effect(new EffectAxiom<NumericExpression>(
			put,
			new BooleanConstant(true),
			on_ref,
			put_scope->variable<NumericExpression>("Y")->ref(*put_scope)
		) );
	}

#ifdef GOLOGPP_TEST_READYLOG

	{ vector<unique_ptr<Expression>> args;

		Scope *main_scope = new Scope(nullptr);
		args.emplace_back(new NumericConstant(1));
		args.emplace_back(new NumericConstant(2));
		vector<Statement *> code;
		code.push_back(put->ref<Action>(*main_scope, std::move(args)));

		EclipseContext::init(unique_ptr<AExecutionBackend>(nullptr));
		EclipseContext &ctx = EclipseContext::instance();

		ctx.run(Block(main_scope, std::move(code)));
	}
#endif // TEST_READYLOG

	global_scope().clear();
#endif
}


void test_parser()
{
#ifdef GOLOGPP_TEST_PARSER
	unique_ptr<Statement> mainproc = parser::parse_file(SOURCE_DIR "/blocksworld.gpp");

	shared_ptr<NumericFluent> on = global_scope().lookup_global<NumericFluent>({"on", 1});
	shared_ptr<Action> put = global_scope().lookup_global<Action>({"put", 2});
	shared_ptr<BooleanFunction> goal = global_scope().lookup_global<BooleanFunction>({"goal", 0});

	if (on && put && goal)
		std::cout << on->name() << " " << put->name() << " " << goal->name() << std::endl;

#ifdef GOLOGPP_TEST_READYLOG
	EclipseContext::init(unique_ptr<AExecutionBackend>(nullptr));
	EclipseContext &ctx = EclipseContext::instance();

	ctx.run(Block(
		new Scope(nullptr, global_scope()),
		{ mainproc.release() }
	));
#endif // TEST_READYLOG
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





