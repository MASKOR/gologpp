#ifndef GOLOGPP_PARSER_ATOMS_H_
#define GOLOGPP_PARSER_ATOMS_H_

#include "utilities.h"
#include "types.h"


namespace gologpp {
namespace parser {


rule<shared_ptr<Variable>(Scope &)> &var_decl();
rule<shared_ptr<Variable>(Scope &, Typename)> &var_usage();
rule<shared_ptr<Variable>(Scope &)> &any_var_usage();


rule<Value *()> &numeric_value();
rule<Value *()> &boolean_value();
rule<Value *()> &string_value();
rule<Value *()> &symbolic_value();
rule<Value *()> &symbolic_value_def();
rule<Value *()> &compound_value();

rule<Value *()> &any_value();

rule<Value *(Typename, bool)> &value();

rule<Expression *()> &atom();


} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_ATOMS_H_

