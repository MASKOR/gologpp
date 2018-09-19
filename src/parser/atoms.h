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

extern template
rule<shared_ptr<Variable<BooleanExpression>>(Scope &)> &var<BooleanExpression>();

extern template
rule<shared_ptr<Variable<NumericExpression>>(Scope &)> &var<NumericExpression>();


rule<shared_ptr<AbstractVariable> (Scope &)> &abstract_var();


/******************
* Constants
******************/

template<class ExprT>
rule<Constant<ExprT> *> &constant();

rule<AbstractConstant *> &abstract_constant();


/******************
* General atoms
******************/

rule<Expression *(Scope &)> &atom();



} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_ATOMS_H_

