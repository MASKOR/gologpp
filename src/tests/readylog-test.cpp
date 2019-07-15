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
		on_scope->get_var(VarDefinitionMode::FORCE, NumberType::name(), "X");
		Fluent *on = new Fluent(
			on_scope,
			NumberType::name(),
			"on",
			on_scope->lookup_vars({"X"})
		);
		InitialValue *iv1 = new InitialValue {
			{ new Constant(NumberType::name(), 1) },
			new Constant(NumberType::name(), 0)
		};
		on->define({iv1});
		global_scope().register_global(on);
	}
	shared_ptr<Fluent> on = global_scope().lookup_global<Fluent>("on", 1);

	{
		Scope *act_scope = new Scope(global_scope());
		Action *put = new Action(act_scope, "", "put", {});
		put->set_args( {
			put->scope().get_var(VarDefinitionMode::FORCE, NumberType::name(), "X"),
			put->scope().get_var(VarDefinitionMode::FORCE, NumberType::name(), "Y")
		});
		global_scope().register_global(put);

		put->set_precondition(new Comparison(
			on->make_ref({ put->arg_ref("X") }),
			ComparisonOperator::NEQ,
			put->arg_ref("Y")
		));
	}
	shared_ptr<Action> put = global_scope().lookup_global<Action>("put", 2);

	{ vector<unique_ptr<Expression>> arg;
		EffectAxiom<Reference<Fluent>> *effect = new EffectAxiom<Reference<Fluent>>();
		effect->define(
			new Constant(BoolType::name(), true),
			on->make_ref({
				put->arg_ref("X")
			} ),
			put->arg_ref("Y")
		);

		put->add_effect(effect);
	}

#ifdef GOLOGPP_TEST_READYLOG

	{
		eclipse_opts options;
		options.trace = false;
		options.guitrace = false;

		Block main(new Scope(global_scope()), {
			put->make_ref({new Constant(NumberType::name(), 1), new Constant(NumberType::name(), 2)})
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
	Expression *mainproc = parser::parse_file(SOURCE_DIR "/examples/blocksworld.gpp").release();

	for (shared_ptr<const Global> g : global_scope().globals())
		std::cout << g->str() << std::endl;

	std::cout << mainproc->str() << std::endl;

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





