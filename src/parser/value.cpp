/*************************************************************************
 * This file is part of golog++.
 *
 * golog++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * golog++ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with golog++.  If not, see <https://www.gnu.org/licenses/>.
**************************************************************************/

#include <string>

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
#include <boost/spirit/include/qi_optional.hpp>

#include <boost/phoenix/bind/bind_function.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/operator/comparison.hpp>
#include <boost/phoenix/operator/arithmetic.hpp>
#include <boost/phoenix/statement/if.hpp>
#include <boost/phoenix/fusion/at.hpp>
#include <boost/phoenix/operator/logical.hpp>
#include <boost/phoenix/object/dynamic_cast.hpp>
#include <boost/phoenix/core/value.hpp>

#include <model/scope.h>
#include <model/reference.h>
#include <model/variable.h>
#include <model/value.h>

#include <unordered_map>
#include <functional>

#include "value.h"

namespace gologpp {
namespace parser {


rule<Value *()> &undefined_value() {
	static rule<Value *()> rv {
		lit("null") [
			_val = new_<Value>()
		]
	};
	return rv;
}


rule<Value *()> &numeric_value() {
	static real_parser<double, strict_real_policies<double>> strict_double;
	static rule<Value *()> rv {
		undefined_value() [ _val = _1 ]
		| strict_double [
			_val = new_<Value>(NumberType::name(), _1)
		]
		| int_ [
			_val = new_<Value>(NumberType::name(), _1)
		],
		"numeric_value"
	};
//	GOLOGPP_DEBUG_NODE(rv)
	return rv;
}

rule<Value *()> &boolean_value() {
	static rule<Value *()> rv {
		undefined_value() [ _val = _1 ]
		| lit("true") [
			_val = new_<Value>(BoolType::name(), true)
		]
		| lit("false") [
			_val = new_<Value>(BoolType::name(), false)
		],
		"boolean_value"
	};
//	GOLOGPP_DEBUG_NODE(rv)
	return rv;
}


rule<Value *()> &string_value() {
	static rule<Value *()> rv {
		undefined_value() [ _val = _1 ]
		| raw_string_literal() [
			_val = new_<Value>(StringType::name(), _1)
		],
		"string_value"
	};
//	GOLOGPP_DEBUG_NODE(rv)
	return rv;
}


rule<Value *()> &symbolic_value() {
		static rule<Value *()> rv {
		undefined_value() [ _val = _1 ]
		| r_name() [
			_val = phoenix::bind(&Scope::get_symbol, phoenix::bind(&global_scope), _1),
			if_(_val == nullptr) [
				_pass = false
			]
		],
		"symbolic_value_usage"
	};
//	GOLOGPP_DEBUG_NODE(rv)
	return rv;
}


rule<Value *()> &symbolic_value_def() {
	static rule<Value *()> rv {
		r_name() [ _val = new_<Value>(val(SymbolType::name()), _1) ],
		"symbolic_value_definition"
	};
//	GOLOGPP_DEBUG_NODE(rv)
	return rv;
}



static rule<Value *()> list_value_;



struct CompoundValueParser : grammar<Value *()> {
	CompoundValueParser()
	: CompoundValueParser::base_type(compound_value_, "compound_value")
	{
		compound_value_ =
			undefined_value() [ _val = _1 ]
			| (
				(any_type_specifier()(phoenix::bind(&global_scope)) >> '{')
				>> (
					r_name() >> '=' >> any_value_
				) % ',' >> '}'
			) [
				_val = new_<Value>(_1, _2)
			]
		;
		compound_value_.name("compound_value");

		any_value_ =
			boolean_value() | numeric_value()
			| symbolic_value() | string_value()
			| list_value_
			| compound_value_
		;
		any_value_.name("any_value");

//		GOLOGPP_DEBUG_NODES((compound_constant_)(any_constant_))
	}

	rule<Value *()> compound_value_;
	rule<Value *()> any_value_;
};


rule<Value *()> &compound_value() {
	static CompoundValueParser ccp;
	static rule<Value *()> rv { ccp };
	return rv;
}


struct ListValueParser : grammar<Value *()> {
	ListValueParser()
	: ListValueParser::base_type(list_value_, "list_value")
	{
		list_value_ =
			undefined_value() [ _val = _1 ]
			| (
				any_type_specifier()(phoenix::bind(&global_scope))
				>> '['
				>> -(any_value_ % ',')
				>> ']'
			) [
			_val = new_<Value>(_1, _2)
		];
		list_value_.name("list_value");

		any_value_ =
			boolean_value() | numeric_value()
			| symbolic_value() | string_value()
			| compound_value()
			| list_value_
		;
		any_value_.name("any_value");

		GOLOGPP_DEBUG_NODES(
			//(list_value_)
		)
	}

	rule<Value *()> list_value_;
	rule<Value *()> any_value_;
};


rule<Value *()> &list_value() {
	static ListValueParser lvp;
	static rule<Value *()> rv { lvp };
	return rv;
}


void initialize_cyclic_values()
{
	list_value_ = list_value();
	list_value_.name("list_value");
	//GOLOGPP_DEBUG_NODE(list_value_)
}



rule<Value *()> &any_value() {
	static rule<Value *()> rv {
		boolean_value() | numeric_value()
			| symbolic_value() | string_value()
			| compound_value()
			| list_value()
		, "any_value"
	};
//	GOLOGPP_DEBUG_NODE(rv)
	return rv;
}



static rule<Value *()> &get_value_parser(Typename type, bool allow_symbol_def)
{
	static std::unordered_map <
		Typename,
		std::reference_wrapper <
			rule<Value *()>
		>
	> value_parser_map {
		{ BoolType::name(), boolean_value() },
		{ NumberType::name(), numeric_value() },
		{ StringType::name(), string_value() },
		{ SymbolType::name(), symbolic_value() },
	};

	if (type == SymbolType::name() && allow_symbol_def)
		return symbolic_value_def();

	auto it = value_parser_map.find(type);
	if (it != value_parser_map.end())
		return it->second;
	else {
		shared_ptr<const Type> tt = global_scope().lookup_type(type);
		if (tt->is<CompoundType>())
			return compound_value();
		else if (tt->is<ListType>())
			return list_value();
		else
			throw Bug("Unknown type " + type);
	}
}



rule<Value *(Typename, bool)> &value() {
	static rule<Value *(Typename, bool)> rv {
		lazy(phoenix::bind(&get_value_parser, _r1, _r2))
	};
//	GOLOGPP_DEBUG_NODE(rv)
	return rv;
}




}
}
