#include "atoms.h"
#include "types.h"

#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
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

#include <boost/phoenix/bind/bind_function.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/operator/comparison.hpp>
#include <boost/phoenix/statement/if.hpp>
#include <boost/phoenix/fusion/at.hpp>
#include <boost/phoenix/operator/logical.hpp>

#include <model/scope.h>
#include <model/reference.h>



namespace gologpp {
namespace parser {


/******************
 * Variables
 ******************/


template<class ExpressionT, bool only_local, bool allow_def>
rule<shared_ptr<Variable<ExpressionT>>(Scope &)> &var() {
	static rule<shared_ptr<Variable<ExpressionT>>(Scope &)> variable;
	variable = {
		type_mark<ExpressionT>() >> r_name() [
			_val = phoenix::bind(
				&Scope::get_var<ExpressionT, only_local, allow_def>,
				_r1, _1
			),
			if_(!_val) [
				_pass = false
			]
		],
		type_descr<ExpressionT>() + "_variable"
	};

	//BOOST_SPIRIT_DEBUG_NODE(variable);
	return variable;
}

#define GOLOGPP_INSTANTIATE_TEMPLATE_VAR(_, seq) \
	template \
	rule < shared_ptr < Variable < BOOST_PP_SEQ_ELEM(0, seq) > > (Scope &) > & \
	var < BOOST_PP_SEQ_ELEM(0, seq), BOOST_PP_SEQ_ELEM(1, seq), BOOST_PP_SEQ_ELEM(2, seq) > ();

BOOST_PP_SEQ_FOR_EACH_PRODUCT(
	GOLOGPP_INSTANTIATE_TEMPLATE_VAR,
	(GOLOGPP_VALUE_TYPES) ((true)(false)) ((true)(false))
)


template<bool only_local>
rule<shared_ptr<AbstractVariable> (Scope &)> &abstract_var() {
	static rule<shared_ptr<AbstractVariable> (Scope &)> any_var {
		var<BooleanExpression, only_local>()(_r1) [ _val = phoenix::bind(
			&std::dynamic_pointer_cast<AbstractVariable, BooleanVariable>,
			_1
		) ]
		| var<NumericExpression, only_local>()(_r1) [ _val = phoenix::bind(
			&std::dynamic_pointer_cast<AbstractVariable, NumericVariable>,
			_1
		) ]
		| var<SymbolicExpression, only_local>()(_r1) [ _val = phoenix::bind(
			&std::dynamic_pointer_cast<AbstractVariable, SymbolicVariable>,
			_1
		) ]
		| var<StringExpression, only_local>()(_r1) [ _val = phoenix::bind(
			&std::dynamic_pointer_cast<AbstractVariable, StringVariable>,
			_1
		) ],
		"any_variable"
	};
	//BOOST_SPIRIT_DEBUG_NODE(any_var);
	return any_var;
}

template
rule<shared_ptr<AbstractVariable> (Scope &)> &abstract_var<true>();

template
rule<shared_ptr<AbstractVariable> (Scope &)> &abstract_var<false>();


/******************
* Constants
******************/

template<>
rule<Constant<BooleanExpression> *> &constant<BooleanExpression, false>() {
	static rule<Constant<BooleanExpression> *> bool_constant {
		lit("true") [
			_val = new_<Constant<BooleanExpression>>(true)
		]
		| lit("false") [
			_val = new_<Constant<BooleanExpression>>(false)
		],
		type_descr<BooleanExpression>() + "_constant"
	};
	//BOOST_SPIRIT_DEBUG_NODE(bool_constant);
	return bool_constant;
}

// Ignore allow_symbol_definition parameter
template<>
rule<Constant<BooleanExpression> *> &constant<BooleanExpression, true>()
{ return constant<BooleanExpression, false>(); }


template<>
rule<Constant<NumericExpression> *> &constant<NumericExpression, false>() {
	static rule<Constant<NumericExpression> *> num_constant {
		double_ [
			_val = new_<Constant<NumericExpression>>(_1)
		]
		| int_ [
			_val = new_<Constant<NumericExpression>>(_1)
		],
		type_descr<NumericExpression>() + "_constant"
	};
	//BOOST_SPIRIT_DEBUG_NODE(num_constant);
	return num_constant;
}

// Ignore allow_symbol_definition parameter
template<>
rule<Constant<NumericExpression> *> &constant<NumericExpression, true>()
{ return constant<NumericExpression, false>(); }


template<>
rule<Constant<SymbolicExpression> *> &constant<SymbolicExpression, false>() {
	static rule<Constant<SymbolicExpression> *> symbol_usage {
		r_name() [
			_val = phoenix::bind(&Scope::get_symbol, phoenix::bind(&global_scope), _1),
			if_(_val == nullptr) [
				_pass = false
			]
		],
		"symbolic_constant_usage"
	};
	//BOOST_SPIRIT_DEBUG_NODE(symbol_usage);
	return symbol_usage;
}


template<>
rule<Constant<SymbolicExpression> *> &constant<SymbolicExpression, true>() {
	static rule<Constant<SymbolicExpression> *> symbol_definition {
		r_name() [ _val = new_<Constant<SymbolicExpression>>(_1) ],
		"symbolic_constant_definition"
	};
	//BOOST_SPIRIT_DEBUG_NODE(symbol_definition);
	return symbol_definition;
}



template<>
rule<Constant<StringExpression> *> &constant<StringExpression, true>() {
	static rule<Constant<StringExpression> *> string_constant {
		qi::as_string [ qi::lexeme [
			lit('"') > *(char_ - '"') > lit('"')
		] ] [
			_val = new_<Constant<StringExpression>>(_1)
		],
		"string_constant"
	};
	return string_constant;
}

// Ignore allow_symbol_definition parameter
template<>
rule<Constant<StringExpression> *> &constant<StringExpression, false>()
{ return constant<StringExpression, true>(); }



template<bool allow_symbol_def>
rule<AbstractConstant *> &abstract_constant() {
	static rule<AbstractConstant *> any_constant {
		(
			constant<BooleanExpression>() // allow_symbol_def is ignored and defaults to false
			| constant<NumericExpression>() // allow_symbol_def is ignored and defaults to false
			| constant<SymbolicExpression, allow_symbol_def>()
			| constant<StringExpression>()
		),
		"any_constant"
	};
	BOOST_SPIRIT_DEBUG_NODE(any_constant);
	return any_constant;
};


template
rule<AbstractConstant *> &abstract_constant<false>();

template
rule<AbstractConstant *> &abstract_constant<true>();


/******************
* General atoms
******************/

rule<Expression *(Scope &)> &atom() {
	static rule<Expression *(Scope &)> any_atom {
		var<BooleanExpression>()(_r1) [ _val = new_<Reference<BooleanVariable>>(_1) ]
		| var<NumericExpression>()(_r1) [ _val = new_<Reference<NumericVariable>>(_1) ]
		| var<SymbolicExpression>()(_r1) [ _val = new_<Reference<SymbolicVariable>>(_1) ]
		| var<StringExpression>()(_r1) [ _val = new_<Reference<StringVariable>>(_1) ]
		| constant<BooleanExpression>() [ _val = _1 ]
		| constant<NumericExpression>() [ _val = _1 ]
		| constant<SymbolicExpression>() [ _val = _1 ]
		| constant<StringExpression>() [ _val = _1 ]
		, "any_atom"
	};
	BOOST_SPIRIT_DEBUG_NODE(any_atom);
	return any_atom;
}


}
}
