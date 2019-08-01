#include "list_access.h"
#include "expressions.h"

#include <model/procedural.h>

#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_char.hpp>

#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/object/delete.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/operator/comparison.hpp>
#include <boost/phoenix/object/dynamic_cast.hpp>
#include <boost/phoenix/statement/if.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>


namespace gologpp {
namespace parser {


rule<Expression *(Scope &, Typename)> &list_access()
{
	static rule<Expression *(Scope &, Typename)> rv {
		(
			(
				list_expression(_r1) [
					if_(phoenix::bind(&AbstractLanguageElement::type, _1) == _r2) [
						_val = _1
					].else_[
						delete_(_1),
						_pass = false
					]
				]
				>> '['
			)
			> numeric_expression(_r1) > ']'
		) [
			_val = new_<ListAccess>(_1, _2)
		]
		, "list_access"
	};

	return rv;
}



} // namespace parser
} // namespace gologpp


