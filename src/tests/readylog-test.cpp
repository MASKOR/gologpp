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
#include <semantics/readylog/execution.h>
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
	{
		Scope *on_scope = new Scope(global_scope());
		on_scope->get_var<NumericExpression>("X");
		NumericFluent *on = new NumericFluent(
			on_scope,
			"on",
			on_scope->lookup_vars({"X"})
		);
		InitialValue<NumericExpression> *iv1 = new InitialValue<NumericExpression> {
			{ new NumericConstant(1) },
			new NumericConstant(0)
		};
		on->define({iv1});
		global_scope().register_global(on);
	}
	shared_ptr<NumericFluent> on = global_scope().lookup_global<NumericFluent>("on", 1);

	{
		Action *put = new Action(global_scope(), "put");
		put->set_args( {
			put->scope().get_var<NumericExpression>("X"),
			put->scope().get_var<NumericExpression>("Y")
		});
		global_scope().register_global(put);

		put->set_precondition(new Comparison<NumericExpression>(
			on->make_ref({ put->arg_ref<NumericExpression>("X") }),
			ComparisonOperator::NEQ,
			put->arg_ref<NumericExpression>("Y")
		));
	}
	shared_ptr<Action> put = global_scope().lookup_global<Action>("put", 2);

	{ vector<unique_ptr<Expression>> arg;
		EffectAxiom<Reference<NumericFluent>> *effect = new EffectAxiom<Reference<NumericFluent>>();
		effect->define(
			new BooleanConstant(true),
			on->make_ref({
				put->arg_ref<NumericExpression>("X")
			} ),
			put->arg_ref<NumericExpression>("Y")
		);

		put->add_effect(effect);
	}

#ifdef GOLOGPP_TEST_READYLOG

	{
		eclipse_opts options;
		options.trace = false;
		options.guitrace = false;

		Block main(new Scope(global_scope()), {
			put->make_ref({new NumericConstant(1), new NumericConstant(2)})
		});

		ReadylogContext::init(options);
		ReadylogContext &ctx = ReadylogContext::instance();

		ctx.run(std::move(main));

		ReadylogContext::shutdown();
	}
#endif // TEST_READYLOG

	global_scope().clear();
#endif
}


void test_parser()
{
#ifdef GOLOGPP_TEST_PARSER
	VoidExpression *mainproc = parser::parse_file(SOURCE_DIR "/examples/blocksworld.gpp").release();

#ifdef GOLOGPP_TEST_READYLOG
	eclipse_opts options;
	options.trace = false;
	options.toplevel = false;
	options.guitrace = false;

	ReadylogContext::init(options);
	ReadylogContext &ctx = ReadylogContext::instance();

	ctx.run(Block(
		new Scope(global_scope()),
		{ mainproc }
	));

	ReadylogContext::shutdown();
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





