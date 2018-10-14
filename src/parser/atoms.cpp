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

#include <boost/phoenix/bind/bind_function.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/phoenix/object/new.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/operator/comparison.hpp>
#include <boost/phoenix/statement/if.hpp>

#include <model/scope.h>
#include <model/reference.h>



namespace gologpp {
namespace parser {


/******************
 * Variables
 ******************/


template<class ExpressionT>
rule<shared_ptr<Variable<ExpressionT>>(Scope &)> &var() {
	static rule<shared_ptr<Variable<ExpressionT>>(Scope &)> rv {
		type_mark<ExpressionT>() >> r_name() [
			_val = phoenix::bind(
				&Scope::get_var<ExpressionT>, _r1, _1
			)
		],
		type_descr<ExpressionT>() + "_variable"
	};
	BOOST_SPIRIT_DEBUG_NODE(rv);
	return rv;
}

template
rule<shared_ptr<Variable<BooleanExpression>>(Scope &)> &var<BooleanExpression>();

template
rule<shared_ptr<Variable<NumericExpression>>(Scope &)> &var<NumericExpression>();

template
rule<shared_ptr<Variable<SymbolicExpression>>(Scope &)> &var<SymbolicExpression>();





rule<shared_ptr<AbstractVariable> (Scope &)> &abstract_var() {
	static rule<shared_ptr<AbstractVariable> (Scope &)> any_var {
		var<BooleanExpression>()(_r1) [ _val = phoenix::bind(
			&std::dynamic_pointer_cast<AbstractVariable, BooleanVariable>,
			_1
		) ]
		| var<NumericExpression>()(_r1) [ _val = phoenix::bind(
			&std::dynamic_pointer_cast<AbstractVariable, NumericVariable>,
			_1
		) ]
		| var<SymbolicExpression>()(_r1) [ _val = phoenix::bind(
			&std::dynamic_pointer_cast<AbstractVariable, SymbolicVariable>,
			_1
		) ],
		"any_variable"
	};
	BOOST_SPIRIT_DEBUG_NODE(any_var);
	return any_var;
}



/******************
* Constants
******************/

template<>
rule<Constant<BooleanExpression> *> &constant<BooleanExpression>(bool) {
	static rule<Constant<BooleanExpression> *> bool_constant {
		lit("true") [
			_val = new_<Constant<BooleanExpression>>(true)
		]
		| lit("false") [
			_val = new_<Constant<BooleanExpression>>(false)
		],
		type_descr<BooleanExpression>() + "_constant"
	};
	BOOST_SPIRIT_DEBUG_NODE(bool_constant);
	return bool_constant;
}


template<>
rule<Constant<NumericExpression> *> &constant<NumericExpression>(bool) {
	static rule<Constant<NumericExpression> *> num_constant {
		double_ [
			_val = new_<Constant<NumericExpression>>(_1)
		]
		| int_ [
			_val = new_<Constant<NumericExpression>>(_1)
		],
		type_descr<NumericExpression>() + "_constant"
	};
	BOOST_SPIRIT_DEBUG_NODE(num_constant);
	return num_constant;
}


template<>
rule<Constant<SymbolicExpression> *> &constant<SymbolicExpression>(bool allow_symbol_def) {
	static rule<Constant<SymbolicExpression> *> rv_no_sym_def {
		r_name() [
			_val = phoenix::bind(&Scope::get_symbol, phoenix::bind(&global_scope), _1),
			if_(_val == nullptr) [
				_pass = false
			]
		],
		"symbolic_constant_usage"
	};
	static rule<Constant<SymbolicExpression> *> rv_allow_sym_def {
		r_name() [ _val = new_<Constant<SymbolicExpression>>(_1) ],
		"symbolic_constant_definition"
	};
	BOOST_SPIRIT_DEBUG_NODES((rv_no_sym_def)(rv_allow_sym_def));
	return allow_symbol_def ? rv_allow_sym_def : rv_no_sym_def;
}



rule<AbstractConstant *> &abstract_constant(bool allow_symbol_def) {
	static rule<AbstractConstant *> any_constant {
		(
			constant<BooleanExpression>()
			| constant<NumericExpression>()
			| constant<SymbolicExpression>(allow_symbol_def)
		),
		"any_constant"
	};
	BOOST_SPIRIT_DEBUG_NODE(any_constant);
	return any_constant;
};



/******************
* General atoms
******************/

rule<Expression *(Scope &)> &atom(bool allow_symbol_def) {
	static rule<Expression *(Scope &)> any_atom {
		var<BooleanExpression>()(_r1) [ _val = new_<Reference<BooleanVariable>>(_1) ]
		| var<NumericExpression>()(_r1) [ _val = new_<Reference<NumericVariable>>(_1) ]
		| var<SymbolicExpression>()(_r1) [ _val = new_<Reference<SymbolicVariable>>(_1) ]
		| constant<BooleanExpression>() [ _val = _1 ]
		| constant<NumericExpression>() [ _val = _1 ]
		| constant<SymbolicExpression>(allow_symbol_def) [_val = _1 ]
		, "any_atom"
	};
	BOOST_SPIRIT_DEBUG_NODE(any_atom);
	return any_atom;
}


}
}
