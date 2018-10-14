#ifndef GOLOGPP_PARSER_ATOMS_H_
#define GOLOGPP_PARSER_ATOMS_H_

#include "utilities.h"


namespace gologpp {
namespace parser {


/******************
 * Variables
 ******************/

template<class ExpressionT>
rule<shared_ptr<Variable<ExpressionT>>(Scope &)> &var();

#define GOLOGPP_PARSER_INSTANTIATE_VARIABLES(r, data, T) \
	extern template \
	rule<shared_ptr<Variable<T>>(Scope &)> &var<T>();

BOOST_PP_SEQ_FOR_EACH(GOLOGPP_PARSER_INSTANTIATE_VARIABLES, (), GOLOGPP_EXPRESSION_TYPES)


rule<shared_ptr<AbstractVariable> (Scope &)> &abstract_var();


/******************
* Constants
******************/

template<class ExprT>
rule<Constant<ExprT> *> &constant(bool allow_symbol_def = false);

#define GOLOGPP_PARSER_DECLARE_CONSTANTS(r, data, T) \
	template<> \
	rule<Constant<T> *> &constant(bool allow_symbol_def);

BOOST_PP_SEQ_FOR_EACH(GOLOGPP_PARSER_DECLARE_CONSTANTS, (), GOLOGPP_EXPRESSION_TYPES)

rule<AbstractConstant *> &abstract_constant(bool allow_symbol_def = false);


/******************
* General atoms
******************/

rule<Expression *(Scope &)> &atom(bool allow_symbol_def = false);



} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_ATOMS_H_

