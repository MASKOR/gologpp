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
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_difference.hpp>
#include <boost/spirit/include/qi_kleene.hpp>
#include <boost/spirit/include/qi_no_skip.hpp>
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


rule<shared_ptr<Variable>(Scope &)> &var_decl() {
	static rule<shared_ptr<Variable>(Scope &)> rv {
		(any_type_specifier() >> r_name()) [
			_val = phoenix::bind(
				&Scope::get_var, _r1,
				VarDefinitionMode::FORCE,
				_1, _2
			)
		],
		"variable_declaration"
	};
//	GOLOGPP_DEBUG_NODE(rv)
	return rv;
}


rule<shared_ptr<Variable>(Scope &, Typename)> &var_usage() {
	static rule<shared_ptr<Variable>(Scope &, Typename)> rv {
		r_name() [
			_val = phoenix::bind(
				&Scope::get_var, _r1,
				VarDefinitionMode::DENY,
				_r2, _1
			),
			_pass = !!_val // force conversion to bool
		],
		"typed_variable_reference"
	};
//	GOLOGPP_DEBUG_NODE(rv)
	return rv;
}


rule<shared_ptr<Variable>(Scope &)> &any_var_usage() {
	static rule<shared_ptr<Variable>(Scope &)> rv {
		r_name() [
			_val = phoenix::bind(
				&Scope::lookup_var, _r1,
				_1
			),
			_pass = !!_val // force conversion to bool
		],
		"variable_reference"
	};
//	GOLOGPP_DEBUG_NODE(rv)
	return rv;
}




rule<Value *()> &numeric_constant() {
	static real_parser<double, strict_real_policies<double>> strict_double;
	static rule<Value *()> rv {
		strict_double [
			_val = new_<Value>(NumberType::name(), _1)
		]
		| int_ [
			_val = new_<Value>(NumberType::name(), _1)
		],
		"numeric_constant"
	};
//	GOLOGPP_DEBUG_NODE(rv)
	return rv;
}

rule<Value *()> &boolean_constant() {
	static rule<Value *()> rv {
		lit("true") [
			_val = new_<Value>(BoolType::name(), true)
		]
		| lit("false") [
			_val = new_<Value>(BoolType::name(), false)
		],
		"boolean_constant"
	};
//	GOLOGPP_DEBUG_NODE(rv)
	return rv;
}


rule<Value *()> &string_constant() {
	static rule<Value *()> rv {
		raw_string_literal() [
			_val = new_<Value>(StringType::name(), _1)
		],
		"string_constant"
	};
//	GOLOGPP_DEBUG_NODE(rv)
	return rv;
}


rule<Value *()> &symbolic_constant() {
		static rule<Value *()> rv {
		r_name() [
			_val = phoenix::bind(&Scope::get_symbol, phoenix::bind(&global_scope), _1),
			if_(_val == nullptr) [
				_pass = false
			]
		],
		"symbolic_constant_usage"
	};
//	GOLOGPP_DEBUG_NODE(rv)
	return rv;
}


rule<Value *()> &symbolic_constant_def() {
	static rule<Value *()> rv {
		r_name() [ _val = new_<Value>(val(SymbolType::name()), _1) ],
		"symbolic_constant_definition"
	};
//	GOLOGPP_DEBUG_NODE(rv)
	return rv;
}

struct CompoundConstantParser : grammar<Value *()> {
	CompoundConstantParser()
	: CompoundConstantParser::base_type(compound_constant_, "compound_constant")
	{
		compound_constant_ = (
			(any_type_specifier() >> '{')
				> (
					r_name() > '=' > any_constant_
				) % ',' > '}'
			) [
				_val = new_<Value>(_1, _2)
			]
		;
		compound_constant_.name("compound_constant");

		any_constant_ =
			boolean_constant() | numeric_constant()
			| symbolic_constant() | string_constant()
			| compound_constant_
		;
		any_constant_.name("any_constant");

//		GOLOGPP_DEBUG_NODES((compound_constant_)(any_constant_))
	}

	rule<Value *()> compound_constant_;
	rule<Value *()> any_constant_;
};


rule<Value *()> &compound_constant() {
	static CompoundConstantParser ccp;
	static rule<Value *()> rv { ccp };
	return rv;
}



rule<Value *()> &any_constant() {
	static rule<Value *()> rv {
		boolean_constant() | numeric_constant()
			| symbolic_constant() | string_constant()
			| compound_constant()
		, "any_constant"
	};
//	GOLOGPP_DEBUG_NODE(rv)
	return rv;
}



static rule<Value *()> &get_constant_parser(Typename type, bool allow_symbol_def)
{
	static std::unordered_map <
		Typename,
		std::reference_wrapper <
			rule<Value *()>
		>
	>
	constant_parser_map {
		{ BoolType::name(), boolean_constant() },
		{ NumberType::name(), numeric_constant() },
		{ StringType::name(), string_constant() },
		{ SymbolType::name(), symbolic_constant() },
	};

	if (type == SymbolType::name() && allow_symbol_def)
		return symbolic_constant_def();

	auto it = constant_parser_map.find(type);
	if (it == constant_parser_map.end())
		return compound_constant();
	else
		return it->second;
}



rule<Value *(Typename, bool)> &constant() {
	static rule<Value *(Typename, bool)> rv {
		lazy(phoenix::bind(&get_constant_parser, _r1, _r2))
	};
//	GOLOGPP_DEBUG_NODE(rv)
	return rv;
}





}
}
