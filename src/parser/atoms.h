#ifndef GOLOGPP_PARSER_ATOMS_H_
#define GOLOGPP_PARSER_ATOMS_H_

#include "utilities.h"
#include "types.h"


namespace gologpp {
namespace parser {


extern rule<shared_ptr<Variable>(Scope &)> var_decl;
extern rule<shared_ptr<Variable>(Scope &, Typename)> var_usage;
extern rule<shared_ptr<Variable>(Scope &)> any_var_usage;


extern rule<Constant *()> numeric_constant;
extern rule<Constant *()> boolean_constant;
extern rule<Constant *()> string_constant;
extern rule<Constant *()> symbolic_constant;
extern rule<Constant *()> symbolic_constant_def;
extern rule<Constant *()> compound_constant;

extern rule<Constant *()> any_constant;

extern rule<Constant *(Typename)> constant;

extern rule<Expression *()> &atom;


} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_ATOMS_H_

