#ifndef GOLOGPP_H_
#define GOLOGPP_H_

#include <memory>
#include <vector>

#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/adapted/std_tuple.hpp>

#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/for_each_product.hpp>

namespace gologpp {

typedef unsigned char arity_t;

template<class T>      using shared_ptr  = std::shared_ptr<T>;
template<class T>      using unique_ptr  = std::unique_ptr<T>;
template<class T>      using weak_ptr    = std::weak_ptr<T>;
template<class... Ts>  using tuple       = std::tuple<Ts...>;
template<class T>      using vector      = std::vector<T>;

using string = std::string;

class AbstractLanguageElement;
template<class> class LanguageElement;

class AbstractAction;
class Action;
class ExogAction;
class Transition;
class ExogTransition;

class Identifier;

class Global;
class Expression;
class BooleanExpression;
class NumericExpression;
class SymbolicExpression;
class Statement;

typedef BooleanExpression Bool;
typedef NumericExpression Number;


class AbstractEffectAxiom;
template<class> class EffectAxiom;

class Scope;
class ScopeOwner;
Scope &global_scope();

class AbstractFluent;
template<class> class Fluent;
template<class> class InitialValue;

typedef Fluent<BooleanExpression> BooleanFluent;
typedef Fluent<NumericExpression> NumericFluent;

class AbstractVariable;
template<class> class Variable;

class AbstractConstant;
template<class> class Constant;

class AbstractDomain;
template<class> class Domain;

typedef Constant<BooleanExpression> BooleanConstant;
typedef Constant<NumericExpression> NumericConstant;
typedef Constant<SymbolicExpression> SymbolicConstant;

typedef Variable<BooleanExpression> BooleanVariable;
typedef Variable<NumericExpression> NumericVariable;
typedef Variable<SymbolicExpression> SymbolicVariable;

class ArithmeticOperation;

class Negation;
template<class> class Comparison;
class BooleanOperation;
class Quantification;

class Block;
class Choose;
class Conditional;
template<class> class Assignment;
template<class> class Pick;
class Call;
class Search;
class Solve;
class Test;
class While;
template<class> class Return;

class AbstractFunction;
template<class> class Function;
using Procedure = Function<Statement>;
using BooleanFunction = Function<BooleanExpression>;
using NumericFunction = Function<NumericExpression>;

class AbstractReference;
template<class> class Reference;

class History;

class AbstractSemantics;
template<class> class Semantics;
class SemanticsFactory;

class AExecutionContext;
class ExecutionContext;

template<class ExprT>
using FluentAssignment = Assignment<Fluent<ExprT>>;

template<class ExprT>
using VariableAssignment = Assignment<Variable<ExprT>>;

template<class ExprT>
using FluentReference = Reference<Fluent<ExprT>>;

template<class ExprT>
using FunctionReference = Reference<Function<ExprT>>;

template<class ExprT>
using VariableReference = Reference<Variable<ExprT>>;


#define GOLOGPP_COMPARABLE_TYPES \
	(NumericExpression)(SymbolicExpression)

#define GOLOGPP_VALUE_TYPES \
	GOLOGPP_COMPARABLE_TYPES (BooleanExpression)

#define GOLOGPP_EXPRESSION_TYPES \
	GOLOGPP_VALUE_TYPES (Statement)

#define GOLOGPP_VALUE_TYPE_TEMPLATES \
	(EffectAxiom)(InitialValue)(Fluent)(Variable)(Constant) \
	(FluentAssignment)(VariableAssignment) \
	(FluentReference)(VariableReference)(Pick)(Return) \
	(Domain)

#define GOLOGPP_EXPRESSION_TYPE_TEMPLATES \
	(Function)(FunctionReference)

#define GOLOGPP_TEMPLATE_CLASS(_, seq) \
	(BOOST_PP_SEQ_ELEM(0, seq)<BOOST_PP_SEQ_ELEM(1, seq)>)

#define GOLOGPP_SEMANTIC_TYPES \
	(Action)(ExogAction)(Transition)(ExogTransition) \
	(Scope)(ArithmeticOperation) \
	(Negation)(BooleanOperation)(Quantification) \
	(Block)(Choose)(Conditional)(Search)(Solve)(Test)(While) \
	(History)(Reference<Action>) \
	BOOST_PP_SEQ_FOR_EACH_PRODUCT(GOLOGPP_TEMPLATE_CLASS, \
		(GOLOGPP_VALUE_TYPE_TEMPLATES)(GOLOGPP_VALUE_TYPES) \
	) \
	BOOST_PP_SEQ_FOR_EACH_PRODUCT(GOLOGPP_TEMPLATE_CLASS, \
		(GOLOGPP_EXPRESSION_TYPE_TEMPLATES)(GOLOGPP_EXPRESSION_TYPES) \
	) \
	BOOST_PP_SEQ_FOR_EACH_PRODUCT(GOLOGPP_TEMPLATE_CLASS, \
		((Comparison))(GOLOGPP_COMPARABLE_TYPES) \
	)


#define DEFINE_IMPLEMENT_WITH_MEMBERS(...) \
	virtual void attach_semantics(SemanticsFactory &f) override { \
		if (!semantics_) { \
			semantics_ = f.make_semantics(*this); \
			boost::fusion::for_each(std::tie(__VA_ARGS__), [&] (auto &e) { \
				e.attach_semantics(f); \
			} ); \
		} \
	}

#define DEFINE_IMPLEMENT \
	virtual void attach_semantics(SemanticsFactory &f) override { \
		if (!semantics_) \
			semantics_ = f.make_semantics(*this); \
	}


} // namespace gologpp

#endif // GOLOGPP_H_
