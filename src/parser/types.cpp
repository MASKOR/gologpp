#include "types.h"

#include <boost/spirit/include/qi_expect.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_kleene.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_action.hpp>

#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/object/dynamic_cast.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>



namespace gologpp {
namespace parser {


template<class ExprT>
rule<void(CompoundType *)> &field_definition()
{
	static rule<void(CompoundType *)> field_definition {
		(type_mark<ExprT>() > r_name() > type_specifier<ExprT>()) [
			phoenix::bind(&CompoundType::add_field, _r1, _1, _2)
		]
	};
	return field_definition;
}


CompoundTypeParser::CompoundTypeParser()
: CompoundTypeParser::base_type(type_definition, "compound_type_definition")
{
	type_definition = (lit("compound") > r_name() > '{') [
		_val = new_<CompoundType>(_1)
	] > *(
		field_definition<NumericExpression>()(_val)
		| field_definition<BooleanExpression>()(_val)
		| field_definition<StringExpression>()(_val)
		| field_definition<SymbolicExpression>()(_val)
		| field_definition<CompoundExpression>()(_val)
	) > '}';
}


template<>
rule<string()> &
type_specifier<CompoundExpression>()
{
	// CompoundExpressions: Colon followed by type name
	static rule<string()> type_specifier {
		qi::lit(':') > r_name()
	};
	return type_specifier;
}


} // namespace parser
} // namespace gologpp
