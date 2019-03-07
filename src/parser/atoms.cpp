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


namespace gologpp {
namespace parser {


/******************
 * Variables
 ******************/


template<class ExpressionT, bool only_local, bool allow_def>
rule<shared_ptr<Variable<ExpressionT>>(Scope &), locals<string>> &
var() {
	static rule<shared_ptr<Variable<ExpressionT>>(Scope &), locals<string>> variable;
	variable = {
		type_mark<ExpressionT>() >> r_name() [
			_val = phoenix::bind(
				&Scope::get_var<ExpressionT, only_local, allow_def>,
				_r1, _1
			)
		] > type_specifier<ExpressionT>() [
			_a = _1, // can't use _1 directly, have to assign it to a local first (wtf?)
			if_(!_val || !phoenix::bind(&Variable<ExpressionT>::set_type_by_name, *_val, _a)) [
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
	rule < shared_ptr < Variable < BOOST_PP_SEQ_ELEM(0, seq) > > (Scope &), locals<string> > & \
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
		) ]
		| var<CompoundExpression, only_local>()(_r1) [ _val = phoenix::bind(
			&std::dynamic_pointer_cast<AbstractVariable, CompoundVariable>,
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
void ConstantParser<BooleanExpression, false>::init(rule<Constant<BooleanExpression> *()> &constant)
{
	constant =
		lit("true") [
			_val = new_<Constant<BooleanExpression>>(true)
		]
		| lit("false") [
			_val = new_<Constant<BooleanExpression>>(false)
		]
	;
	constant.name(type_descr<BooleanExpression>() + "_constant");
	//BOOST_SPIRIT_DEBUG_NODE(bool_constant);
}

template<>
void ConstantParser<BooleanExpression, true>::init(rule<Constant<BooleanExpression> *()> &constant)
{ ConstantParser<BooleanExpression, false>::init(constant); }



template<>
void ConstantParser<NumericExpression, false>::init(rule<Constant<NumericExpression> *()> &constant)
{
	constant =
		double_ [
			_val = new_<Constant<NumericExpression>>(_1)
		]
		| int_ [
			_val = new_<Constant<NumericExpression>>(_1)
		]
	;
	constant.name(type_descr<NumericExpression>() + "_constant");
	//BOOST_SPIRIT_DEBUG_NODE(num_constant);
}

template<>
void ConstantParser<NumericExpression, true>::init(rule<Constant<NumericExpression> *()> &constant)
{ ConstantParser<NumericExpression, false>::init(constant); }



template<>
void ConstantParser<SymbolicExpression, true>::init(rule<Constant<SymbolicExpression> *()> &constant)
{
	constant = r_name() [ _val = new_<Constant<SymbolicExpression>>(_1) ];
	constant.name(type_descr<SymbolicExpression>() + "_constant_definition");
}

template<>
void ConstantParser<SymbolicExpression, false>::init(rule<Constant<SymbolicExpression> *()> &constant)
{
	constant = r_name() [
		_val = phoenix::bind(&Scope::get_symbol, phoenix::bind(&global_scope), _1),
		if_(_val == nullptr) [
			_pass = false
		]
	];
	constant.name(type_descr<SymbolicExpression>() + "_constant_usage");
	//BOOST_SPIRIT_DEBUG_NODE(symbol_usage);
}



template<>
void ConstantParser<StringExpression, false>::init(rule<Constant<StringExpression> *()> &constant)
{
	constant =
		qi::as_string [ qi::lexeme [
			lit('"') > *(char_ - '"') > lit('"')
		] ] [
			_val = new_<Constant<StringExpression>>(_1)
		]
	;
	constant.name(type_descr<StringExpression>() + "_constant");
}

template<>
void ConstantParser<StringExpression, true>::init(rule<Constant<StringExpression> *()> &constant)
{ ConstantParser<StringExpression, false>::init(constant); }



template<bool allow_symbol_def>
ConstantParser<CompoundExpression, allow_symbol_def>::ConstantParser()
: ConstantParser<CompoundExpression, allow_symbol_def>::base_type(constant, type_descr<CompoundExpression>() + "_constant")
{
	constant =
		( qi::lit('{') > *(
			r_name() > '=' > abstract_constant
		) > qi::lit('}') ) [
			_val = new_<Constant<CompoundExpression>>(_1)
		]
	;
	constant.name(type_descr<CompoundExpression>() + "_constant");

	abstract_constant = boolean_constant | numeric_constant | symbolic_constant | string_constant | constant;
	abstract_constant.name("any_constant");

	BOOST_SPIRIT_DEBUG_NODES((constant)(abstract_constant));
}

template
ConstantParser<CompoundExpression, false>::ConstantParser();

template
ConstantParser<CompoundExpression, true>::ConstantParser();


ConstantParser<NumericExpression> numeric_constant;
ConstantParser<BooleanExpression> boolean_constant;
ConstantParser<StringExpression> string_constant;
ConstantParser<SymbolicExpression> symbolic_constant;
ConstantParser<SymbolicExpression, true> symbolic_constant_def;
ConstantParser<CompoundExpression> compound_constant;



template<>
rule<AbstractConstant *()> &abstract_constant<true>() {
	static rule<AbstractConstant *()> any_constant {
		boolean_constant | numeric_constant
		| symbolic_constant_def
		| string_constant | compound_constant,
		"any_constant"
	};
	BOOST_SPIRIT_DEBUG_NODE(any_constant);
	return any_constant;
};



template<>
rule<AbstractConstant *()> &abstract_constant<false>() {
	static rule<AbstractConstant *()> any_constant {
		boolean_constant | numeric_constant
		| symbolic_constant
		| string_constant | compound_constant,
		"any_constant"
	};
	BOOST_SPIRIT_DEBUG_NODE(any_constant);
	return any_constant;
};



/******************
* General atoms
******************/

rule<Expression *(Scope &)> &atom() {
	static rule<Expression *(Scope &)> any_atom {
		var<BooleanExpression>()(_r1) [ _val = new_<Reference<BooleanVariable>>(_1) ]
		| var<NumericExpression>()(_r1) [ _val = new_<Reference<NumericVariable>>(_1) ]
		| var<SymbolicExpression>()(_r1) [ _val = new_<Reference<SymbolicVariable>>(_1) ]
		| var<StringExpression>()(_r1) [ _val = new_<Reference<StringVariable>>(_1) ]
		| var<CompoundExpression>()(_r1) [ _val = new_<Reference<CompoundVariable>>(_1) ]
		| abstract_constant<false>() [ _val = _1 ]
		, "any_atom"
	};
	BOOST_SPIRIT_DEBUG_NODE(any_atom);
	return any_atom;
}


}
}
