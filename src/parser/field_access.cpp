#include "field_access.h"
#include "list_access.h"
#include "types.h"
#include "compound_expression.h"

#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_kleene.hpp>
#include <boost/spirit/include/qi_plus.hpp>

#include <boost/phoenix/fusion/at.hpp>
#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/operator/comparison.hpp>
#include <boost/phoenix/object/dynamic_cast.hpp>
#include <boost/phoenix/statement/if.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/phoenix/bind/bind_function.hpp>

#include <model/procedural.h>


namespace gologpp {
namespace parser {



static Expression *build_mixed_field_access(
	Expression *subj,
	vector<fusion_wtf_vector<vector<string>, vector<Expression *>>> members,
	vector<string> field_names
) {
	Expression *rv = subj;
	for (auto &pair : members) {
		for (const string &field : at_c<0>(pair))
			rv = new FieldAccess(rv, field);
		for (Expression *idx : at_c<1>(pair))
			rv = new ListAccess(rv, idx);
	}
	for (const string &field : field_names)
		rv = new FieldAccess(rv, field);

	return rv;
}



rule<string()> field_access {
	lit('.') >> r_name()
	, "field_access"
};



rule<Expression *(Scope &, Typename)> &mixed_field_access()
{
	static rule<Expression *(Scope &, Typename)> rv {
		(
			compound_atom(_r1)
			>> *(
				+field_access
				>> +list_access(_r1)
			) >> *field_access
		) [
			_val = phoenix::bind(&build_mixed_field_access, _1, _2, _3),
			if_(phoenix::bind(&AbstractLanguageElement::type, _val) != _r2) [
				_pass = false
			]
		]
		, "mixed_field_access"
	};
	GOLOGPP_DEBUG_NODE(rv)
	return rv;
}


} // namespace parser
} // namespace gologpp
