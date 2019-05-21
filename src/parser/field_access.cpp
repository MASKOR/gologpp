#include "field_access.h"
#include "types.h"
#include "compound_expression.h"
#include "reference.h"

#include <boost/spirit/include/qi_expect.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_char.hpp>

#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/object/delete.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/operator/comparison.hpp>
#include <boost/phoenix/statement/if.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>

#include <model/procedural.h>


namespace gologpp {
namespace parser {


rule<FieldAccess *(Scope &, Typename)> field_access { (
	(compound_expression(_r1) >> lit('.')) > r_name()) [
		_val = new_<FieldAccess>(_1, _2)
	],
	"field_access"
};


} // namespace parser
} // namespace gologpp
