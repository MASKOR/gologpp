#ifndef GOLOGPP_PARSER_ATOMS_H_
#define GOLOGPP_PARSER_ATOMS_H_

#include "utilities.h"


namespace gologpp {
namespace parser {


/******************
 * Variables
 ******************/

template<class ExpressionT, bool only_local = false, bool allow_def = true>
rule<shared_ptr<Variable<ExpressionT>>(Scope &)> &var();

#define GOLOGPP_PARSER_DECLARE_TEMPLATE_VAR(_, seq) \
	extern template \
	rule < shared_ptr < Variable < BOOST_PP_SEQ_ELEM(0, seq) > >(Scope &)> & \
	var < BOOST_PP_SEQ_ELEM(0, seq), BOOST_PP_SEQ_ELEM(1, seq), BOOST_PP_SEQ_ELEM(2, seq) > ();

BOOST_PP_SEQ_FOR_EACH_PRODUCT(
	GOLOGPP_PARSER_DECLARE_TEMPLATE_VAR,
	(GOLOGPP_VALUE_TYPES) ((true)(false)) ((true)(false))
)



template<bool only_local = false>
rule<shared_ptr<AbstractVariable> (Scope &)> &abstract_var();

extern template
rule<shared_ptr<AbstractVariable> (Scope &)> &abstract_var<true>();

extern template
rule<shared_ptr<AbstractVariable> (Scope &)> &abstract_var<false>();


/******************
* Constants
******************/

template<class ExprT, bool allow_symbol_def = false>
rule<Constant<ExprT> *> &constant();

#define GOLOGPP_PARSER_DECLARE_CONSTANTS(r, allow_symbol_def, T) \
	template<> \
	rule<Constant<T> *> &constant<T, allow_symbol_def>();

BOOST_PP_SEQ_FOR_EACH(GOLOGPP_PARSER_DECLARE_CONSTANTS, false, GOLOGPP_EXPRESSION_TYPES)
BOOST_PP_SEQ_FOR_EACH(GOLOGPP_PARSER_DECLARE_CONSTANTS, true, GOLOGPP_EXPRESSION_TYPES)


template<bool allow_symbol_def = false>
rule<AbstractConstant *> &abstract_constant();

extern template
rule<AbstractConstant *> &abstract_constant<false>();

extern template
rule<AbstractConstant *> &abstract_constant<true>();



/******************
* General atoms
******************/

rule<Expression *(Scope &)> &atom();



} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_ATOMS_H_

