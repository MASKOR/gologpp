#include <string>

#include "atoms.h"
#include "types.h"

#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_expect.hpp>
#include <boost/spirit/include/qi_attr.hpp>
#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_int.hpp>
#include <boost/spirit/include/qi_real.hpp>
#include <boost/spirit/include/qi_lexeme.hpp>
#include <boost/spirit/include/qi_char_class.hpp>
#include <boost/spirit/include/qi_char_.hpp>
#include <boost/spirit/include/qi_difference.hpp>
#include <boost/spirit/include/qi_kleene.hpp>
#include <boost/spirit/include/qi_no_skip.hpp>
#include <boost/spirit/include/qi_as_string.hpp>
#include <boost/spirit/include/qi_list.hpp>
#include <boost/spirit/include/qi_lazy.hpp>
#include <boost/spirit/include/qi_lit.hpp>

#include <boost/phoenix/bind/bind_function.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/operator/comparison.hpp>
#include <boost/phoenix/statement/if.hpp>
#include <boost/phoenix/fusion/at.hpp>
#include <boost/phoenix/operator/logical.hpp>
#include <boost/phoenix/object/dynamic_cast.hpp>
#include <boost/phoenix/core/value.hpp>

#include <model/scope.h>
#include <model/reference.h>
#include <model/variable.h>
#include <model/constant.h>

#include <unordered_map>
#include <functional>


namespace gologpp {
namespace parser {



rule<shared_ptr<Variable>(Scope &)> var_decl {
	(any_type_specifier >> r_name()) [
		_val = phoenix::bind(
			&Scope::get_var, _r1,
			VarDefinitionMode::FORCE,
			_1, _2
		)
	],
	"variable_declaration"
};


rule<shared_ptr<Variable>(Scope &, Typename)> var_usage {
	r_name() [
		_val = phoenix::bind(
			&Scope::get_var, _r1,
			VarDefinitionMode::DENY,
			_r2, _1
		)
	],
	"variable_reference"
};



static real_parser<double, strict_real_policies<double>> strict_double;

rule<Constant *()> numeric_constant {
	strict_double [
		_val = new_<Constant>(Number::static_name(), _1)
	]
	| int_ [
		_val = new_<Constant>(Number::static_name(), _1)
	],
	"numeric_constant"
};


rule<Constant *()> boolean_constant {
	lit("true") [
		_val = new_<Constant>(Bool::static_name(), true)
	]
	| lit("false") [
		_val = new_<Constant>(Bool::static_name(), false)
	],
	"boolean_constant"
};


rule<Constant *()> string_constant {
	qi::as_string [ qi::lexeme [
		lit('"') > *(char_ - '"') > lit('"')
	] ] [
		_val = new_<Constant>(String::static_name(), _1)
	],
	"string_constant"
};


rule<Constant *()> symbolic_constant {
	r_name() [
		_val = phoenix::bind(&Scope::get_symbol, phoenix::bind(&global_scope), _1),
		if_(_val == nullptr) [
			_pass = false
		]
	],
	"symbolic_constant_usage"
};


rule<Constant *()> symbolic_constant_def {
	r_name() [ _val = new_<Constant>(val(Symbol::static_name()), _1) ],
	"symbolic_constant_definition"
};


struct CompoundConstantParser : grammar<Constant *()> {
	CompoundConstantParser()
	: CompoundConstantParser::base_type(compound_constant_, "compound_constant")
	{
		compound_constant_ = (
			(any_type_specifier >> '{')
				> (
					r_name() > '=' > any_constant
				) % ',' > '}'
			) [
				_val = new_<Constant>(_1, _2)
			]
		;
		compound_constant_.name("compound_constant");

		any_constant_ =
			boolean_constant | numeric_constant
			| symbolic_constant | string_constant
			| compound_constant_
		;
		any_constant_.name("any_constant");

		GOLOGPP_DEBUG_NODES((compound_constant_)(any_constant_));
	}

	rule<Constant *()> compound_constant_;
	rule<Constant *()> any_constant_;
};


rule<Constant *()> compound_constant = CompoundConstantParser();



rule<Constant *()> any_constant {
	boolean_constant | numeric_constant
		| symbolic_constant | string_constant
		| compound_constant
	, "any_constant"
};



static std::unordered_map <
	Typename,
	std::reference_wrapper <
		rule<Constant *()>
	>
>
constant_parser_map {
	{ Bool::static_name(), boolean_constant },
	{ Number::static_name(), numeric_constant },
	{ String::static_name(), string_constant },
	{ Symbol::static_name(), symbolic_constant },
};

static rule<Constant *()> &get_constant_parser(Typename type);

rule<Constant *(Typename)> constant {
	lazy(phoenix::bind(&get_constant_parser, _r1))
};

static rule<Constant *()> &get_constant_parser(Typename type)
{
	auto it = constant_parser_map.find(type);
	if (it == constant_parser_map.end())
		return compound_constant;
	else
		return it->second;
}






}
}
