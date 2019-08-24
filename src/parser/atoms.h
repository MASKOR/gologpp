#ifndef GOLOGPP_PARSER_ATOMS_H_
#define GOLOGPP_PARSER_ATOMS_H_

#include "utilities.h"
#include "types.h"


namespace gologpp {
namespace parser {


rule<shared_ptr<Variable>(Scope &)> &var_decl();
rule<shared_ptr<Variable>(Scope &, Typename)> &var_usage();
rule<shared_ptr<Variable>(Scope &)> &any_var_usage();


rule<Value *()> &numeric_literal();
rule<Value *()> &boolean_literal();
rule<Value *()> &string_literal();
rule<Value *()> &symbolic_literal();
rule<Value *()> &symbolic_literal_def();
rule<Value *()> &compound_literal();
rule<Value *()> &list_literal();

rule<Value *()> &any_literal();

rule<Value *(Typename, bool)> &literal();

rule<Expression *()> &atom();


void initialize_cyclic_literals();


} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_ATOMS_H_

