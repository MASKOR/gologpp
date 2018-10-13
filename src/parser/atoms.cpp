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
	return rv;
}

template
rule<shared_ptr<Variable<BooleanExpression>>(Scope &)> &var<BooleanExpression>();

template
rule<shared_ptr<Variable<NumericExpression>>(Scope &)> &var<NumericExpression>();





rule<shared_ptr<AbstractVariable> (Scope &)> &abstract_var() {
	static rule<shared_ptr<AbstractVariable> (Scope &)> rv {
		var<BooleanExpression>()(_r1) [ _val = phoenix::bind(
			&std::dynamic_pointer_cast<AbstractVariable, BooleanVariable>,
			_1
		) ]
		| var<NumericExpression>()(_r1) [ _val = phoenix::bind(
			&std::dynamic_pointer_cast<AbstractVariable, NumericVariable>,
			_1
		) ],
		"any_variable"
	};

	return rv;
}


/******************
* Constants
******************/

template<>
rule<Constant<BooleanExpression> *> &constant<BooleanExpression>() {
	static rule<Constant<BooleanExpression> *> rv {
		lit("true") [
			_val = new_<Constant<BooleanExpression>>(true)
		]
		| lit("false") [
			_val = new_<Constant<BooleanExpression>>(false)
		],
		type_descr<BooleanExpression>() + "_constant"
	};
	return rv;
}


template<>
rule<Constant<NumericExpression> *> &constant<NumericExpression>() {
	static rule<Constant<NumericExpression> *> rv {
		double_ [
			_val = new_<Constant<NumericExpression>>(_1)
		]
		| int_ [
			_val = new_<Constant<NumericExpression>>(_1)
		],
		type_descr<NumericExpression>() + "_constant"
	};
	return rv;
}



rule<AbstractConstant *> &abstract_constant() {
	static rule<AbstractConstant *> rv {
		(constant<BooleanExpression>() | constant<NumericExpression>()),
		"any_constant"
	};
	return rv;
};


extern template
rule<Constant<BooleanExpression> *> &constant<BooleanExpression>();

extern template
rule<Constant<NumericExpression> *> &constant<NumericExpression>();



/******************
* General atoms
******************/

rule<Expression *(Scope &)> &atom() {
	static rule<Expression *(Scope &)> rv {
		var<BooleanExpression>()(_r1) [ _val = new_<Reference<BooleanVariable>>(_1) ]
		| var<NumericExpression>()(_r1) [ _val = new_<Reference<NumericVariable>>(_1) ]
		| constant<BooleanExpression>() [ _val = _1 ]
		| constant<NumericExpression>() [ _val = _1 ]
		, "any_atom"
	};
	return rv;
}


}
}
