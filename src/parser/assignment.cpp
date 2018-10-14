#include "assignment.h"
#include "atoms.h"
#include "arithmetic.h"
#include "formula.h"
#include "types.h"

#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_string.hpp>

#include <boost/phoenix/object/new.hpp>


namespace gologpp {
namespace parser {



template<class ExpressionT>
AssignmentParser<Fluent<ExpressionT>>::AssignmentParser()
: AssignmentParser<Fluent<ExpressionT>>::base_type(assignment, string("assignment_to_") + type_descr<ExpressionT>() + "_fluent")
{
	assignment = (fluent_ref(_r1) >> "=" > expression(_r1)) [
		_val = new_<Assignment<Fluent<ExpressionT>>>(_1, _2)
	];
}


template
AssignmentParser<Fluent<NumericExpression>>::AssignmentParser();

template
AssignmentParser<Fluent<BooleanExpression>>::AssignmentParser();

template
AssignmentParser<Fluent<SymbolicExpression>>::AssignmentParser();


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

template
AssignmentParser<Variable<NumericExpression>>::AssignmentParser();

template
AssignmentParser<Variable<BooleanExpression>>::AssignmentParser();

template
AssignmentParser<Variable<SymbolicExpression>>::AssignmentParser();




} // namespace parser
} // namespace gologpp

