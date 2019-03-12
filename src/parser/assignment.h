#ifndef GOLOGPP_PARSER_ASSIGNMENT_H_
#define GOLOGPP_PARSER_ASSIGNMENT_H_

#include <model/fluent.h>
#include <model/procedural.h>

#include "utilities.h"
#include "expressions.h"
#include "reference.h"


namespace gologpp {
namespace parser {


template<class LhsT>
struct AssignmentParser;


template<class ExpressionT>
struct AssignmentParser<Reference<Fluent<ExpressionT>>> : grammar<Assignment<Reference<Fluent<ExpressionT>>> *(Scope &)> {
	AssignmentParser();

	rule<Assignment<Reference<Fluent<ExpressionT>>> *(Scope &)> assignment;
	ReferenceParser<Fluent<ExpressionT>> fluent_ref;
	ExpressionParser<ExpressionT> expression;
};


template<class ExpressionT>
struct AssignmentParser<Reference<Variable<ExpressionT>>> : grammar<Assignment<Reference<Variable<ExpressionT>>> *(Scope &)> {
	AssignmentParser();

	rule<Assignment<Reference<Variable<ExpressionT>>> *(Scope &)> assignment;
	rule<Reference<Variable<ExpressionT>> *(Scope &)> var_ref;
	ExpressionParser<ExpressionT> expression;
};



} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_ASSIGNMENT_H_

