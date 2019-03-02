#include "field_access.h"
#include "assignment.h"
#include "atoms.h"
#include "arithmetic.h"
#include "formula.h"
#include "types.h"
#include "symbolic_expression.h"
#include "string_expression.h"
#include "compound_expression.h"

#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_string.hpp>
#include <boost/spirit/include/qi_action.hpp>

#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/object/dynamic_cast.hpp>
#include <boost/phoenix/operator/self.hpp>


namespace gologpp {
namespace parser {



template<class ExpressionT>
AssignmentParser<Fluent<ExpressionT>>::AssignmentParser()
: AssignmentParser<Fluent<ExpressionT>>::base_type(assignment, string("assignment_to_") + type_descr<ExpressionT>() + "_fluent")
{
	assignment = (fluent_ref(_r1) >> "=" > expression(_r1)) [
		_val = new_<Assignment<Fluent<ExpressionT>>>(_1, _2)
	];
	assignment.name(string("assignment_to_") + type_descr<ExpressionT>() + "_fluent");
}

#define GOLOGPP_PARSER_INSTANTIATE_FLUENT_ASSIGNMENT(r, data, T) \
	template \
	AssignmentParser<Fluent<T>>::AssignmentParser();

BOOST_PP_SEQ_FOR_EACH(GOLOGPP_PARSER_INSTANTIATE_FLUENT_ASSIGNMENT, (), GOLOGPP_VALUE_TYPES)



template<class ExpressionT>
AssignmentParser<Variable<ExpressionT>>::AssignmentParser()
: AssignmentParser<Variable<ExpressionT>>::base_type(assignment, string("assignment_to_") + type_descr<ExpressionT>() + "_variable")
{
	assignment = (var_ref(_r1) >> "=" > expression(_r1)) [
		_val = new_<Assignment<Variable<ExpressionT>>>(_1, _2)
	];

	var_ref = var<ExpressionT>()(_r1) [ _val = new_<Reference<Variable<ExpressionT>>>(_1) ];
	var_ref.name("reference_to_" + type_descr<ExpressionT>() + "_variable");
}

#define GOLOGPP_PARSER_INSTANTIATE_VARIABLE_ASSIGNMENT(r, data, T) \
	template \
	AssignmentParser<Variable<T>>::AssignmentParser();

BOOST_PP_SEQ_FOR_EACH(GOLOGPP_PARSER_INSTANTIATE_VARIABLE_ASSIGNMENT, (), GOLOGPP_VALUE_TYPES)



} // namespace parser
} // namespace gologpp

