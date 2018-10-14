#ifndef GOLOGPP_PARSER_REFERENCE_H_
#define GOLOGPP_PARSER_REFERENCE_H_

#include "utilities.h"
#include "atoms.h"

#include <boost/spirit/home/qi/nonterminal/error_handler.hpp>
#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_expect.hpp>
#include <boost/spirit/include/qi_optional.hpp>
#include <boost/spirit/include/qi_list.hpp>
#include <boost/spirit/include/qi_attr.hpp>
#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_char.hpp>

#include <boost/phoenix/object/dynamic_cast.hpp>
#include <boost/phoenix/object/delete.hpp>
#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/operator/logical.hpp>
#include <boost/phoenix/statement/if.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/phoenix/bind/bind_function.hpp>

#include <model/reference.h>


namespace gologpp {
namespace parser {


/******************
* References
******************/

template<class GologT>
struct ReferenceParser : grammar<Reference<GologT> *(Scope &)> {
	ReferenceParser() : ReferenceParser::base_type(pred_ref, string("reference_to_") + typeid(GologT).name())
	{
		pred_ref = (((r_name() >> "(") > -(
			(atom()(_r1) | any_pred_ref(_r1)) %  ","
		) ) > ")") [
			_val = new_<Reference<GologT>>(_1, _2),
			if_(!phoenix::bind(&Reference<GologT>::consistent, *_val)) [
				_pass = false,
				delete_(_val)
			]
		];

		any_pred_ref = (((r_name() > "(") > -(
			(atom()(_r1) | any_pred_ref(_r1)) % ","
		) ) > ")") [
			_val = phoenix::bind(&ref_to_global, _1, _2),
			if_(!phoenix::bind(&AbstractReference::consistent, dynamic_cast_<AbstractReference &>(*_val))) [
				_pass = false,
				delete_(_val)
			]
		];

		BOOST_SPIRIT_DEBUG_NODES((pred_ref)(any_pred_ref));
	}

	rule<Reference<GologT> *(Scope &)> pred_ref;
	rule<Expression *(Scope &)> any_pred_ref;
};



} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_REFERENCE_H_

