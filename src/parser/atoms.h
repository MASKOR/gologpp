#ifndef GOLOGPP_PARSER_ATOMS_H_
#define GOLOGPP_PARSER_ATOMS_H_

#include "utilities.h"
#include "types.h"


namespace gologpp {
namespace parser {


/******************
 * Variables
 ******************/

template<class ExpressionT, VarDefinitionMode var_def_mode = VarDefinitionMode::DENY>
rule<shared_ptr<Variable<ExpressionT>>(Scope &), locals<string>> &var();

#define GOLOGPP_PARSER_DECLARE_TEMPLATE_VAR(_, seq) \
	extern template \
	rule < shared_ptr < Variable < BOOST_PP_SEQ_ELEM(0, seq) > >(Scope &), locals<string>> & \
	var < BOOST_PP_SEQ_ELEM(0, seq), BOOST_PP_SEQ_ELEM(1, seq) > ();

BOOST_PP_SEQ_FOR_EACH_PRODUCT(
	GOLOGPP_PARSER_DECLARE_TEMPLATE_VAR,
	(GOLOGPP_VALUE_TYPES) ((VarDefinitionMode::DENY)(VarDefinitionMode::ALLOW)(VarDefinitionMode::FORCE))
)



template<VarDefinitionMode var_def_mode>
rule<shared_ptr<AbstractVariable> (Scope &)> &abstract_var();

extern template
rule<shared_ptr<AbstractVariable> (Scope &)> &abstract_var<VarDefinitionMode::DENY>();

extern template
rule<shared_ptr<AbstractVariable> (Scope &)> &abstract_var<VarDefinitionMode::ALLOW>();

extern template
rule<shared_ptr<AbstractVariable> (Scope &)> &abstract_var<VarDefinitionMode::FORCE>();


/******************
* Constants
******************/

template<class ExprT, bool allow_symbol_def = false>
struct ConstantParser : public grammar<Constant<ExprT> *()> {
	ConstantParser()
	: ConstantParser<ExprT, allow_symbol_def>::base_type(constant, type_descr<ExprT>()() + "_constant")
	{ init(constant); }

	static void init(rule<Constant<ExprT> *()> &constant);

	rule<Constant<ExprT> *()> constant;
};

template<bool allow_symbol_def>
struct ConstantParser<CompoundExpression, allow_symbol_def> : public grammar<Constant<CompoundExpression> *()> {
	ConstantParser();

	rule<Constant<CompoundExpression> *()> constant;
	rule<AbstractConstant *()> abstract_constant;
};


extern ConstantParser<BooleanExpression> boolean_constant;
extern ConstantParser<NumericExpression> numeric_constant;
extern ConstantParser<StringExpression> string_constant;
extern ConstantParser<SymbolicExpression> symbolic_constant;
extern ConstantParser<SymbolicExpression, true> symbolic_constant_def;
extern ConstantParser<CompoundExpression> compound_constant;


template<bool allow_symbol_def>
rule<AbstractConstant *()> &abstract_constant();



/******************
* General atoms
******************/

rule<Expression *(Scope &)> &atom();



} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_ATOMS_H_

