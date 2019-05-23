#ifndef GOLOGPP_PARSER_ATOMS_H_
#define GOLOGPP_PARSER_ATOMS_H_

#include "utilities.h"
#include "types.h"


namespace gologpp {
namespace parser {


rule<shared_ptr<Variable>(Scope &)> &var_decl();
rule<shared_ptr<Variable>(Scope &, Typename)> &var_usage();
rule<shared_ptr<Variable>(Scope &)> &any_var_usage();


rule<Constant *()> &numeric_constant();
rule<Constant *()> &boolean_constant();
rule<Constant *()> &string_constant();
rule<Constant *()> &symbolic_constant();
rule<Constant *()> &symbolic_constant_def();
rule<Constant *()> &compound_constant();

rule<Constant *()> &any_constant();

rule<Constant *(Typename, bool)> &constant();

rule<Expression *()> &atom();


} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_ATOMS_H_

