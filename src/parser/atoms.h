#ifndef GOLOGPP_PARSER_ATOMS_H_
#define GOLOGPP_PARSER_ATOMS_H_

#include "utilities.h"
#include "types.h"


namespace gologpp {
namespace parser {


rule<shared_ptr<Variable>(Scope &)> &var_decl();
rule<shared_ptr<Variable>(Scope &, Typename)> &var_usage();
rule<shared_ptr<Variable>(Scope &)> &any_var_usage();


rule<Value *()> &numeric_constant();
rule<Value *()> &boolean_constant();
rule<Value *()> &string_constant();
rule<Value *()> &symbolic_constant();
rule<Value *()> &symbolic_constant_def();
rule<Value *()> &compound_constant();

rule<Value *()> &any_constant();

rule<Value *(Typename, bool)> &constant();

rule<Expression *()> &atom();


} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_ATOMS_H_

