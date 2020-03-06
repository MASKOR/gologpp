#include <model/platform_model.h>

#include "platform_model.h"

#include <boost/spirit/include/qi_action.hpp>

#include <boost/spirit/include/qi_plus.hpp>
#include <boost/spirit/include/qi_list.hpp>
#include <boost/spirit/include/qi_string.hpp>
#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_expect.hpp>
#include <boost/spirit/include/qi_eoi.hpp>
#include <boost/spirit/home/qi/nonterminal/error_handler.hpp>
#include <boost/spirit/include/qi_int.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_char_class.hpp>
#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/bind/bind_function.hpp>


namespace gologpp {
namespace parser {

	PlatformParser::PlatformParser()
	: PlatformParser::base_type(platform)
	{
		platform	= (states >> transitions)[::boost::phoenix::new_<PlatformModel>(_1,_2)];
		platform.name("platform");

		states = state % ',' >> ';';
		states.name("states");

		state = text >> '[' >> duration >> ']';
		state.name("state");

		transitions = transition % ',' >> ';';
		transitions.name("transitions");

		transition = text >> "->" >> text >> '[' >> duration >> ']';
		transition.name("transition");

		duration = ::boost::spirit::qi::int_ >> ',' >> ::boost::spirit::qi::int_;
		duration.name("duration");

		text	= +::boost::spirit::qi::char_("a-z");
		//text	= +~::boost::spirit::qi::char_("a");
		text.name("text");
		BOOST_SPIRIT_DEBUG_NODES((platform)(states)(state)(transitions)(transition)(duration)(text))

	}
} // namespace parser
} // namespace gologpp
