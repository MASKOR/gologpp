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
class AbstractTransition;
class Activity;
class Transition;

class Identifier;

class Global;
class Expression;
class BooleanExpression;
class NumericExpression;
class SymbolicExpression;
class StringExpression;
class VoidExpression;

typedef BooleanExpression Bool;
typedef NumericExpression Number;
typedef SymbolicExpression Symbol;
typedef StringExpression String;
typedef VoidExpression Void;
typedef VoidExpression Statement;


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

typedef Constant<Bool> BooleanConstant;
typedef Constant<Number> NumericConstant;
typedef Constant<Symbol> SymbolicConstant;
typedef Constant<String> StringConstant;

typedef Variable<Bool> BooleanVariable;
typedef Variable<Number> NumericVariable;
typedef Variable<Symbol> SymbolicVariable;
typedef Variable<String> StringVariable;

class ArithmeticOperation;
class StringConcatenation;

class Negation;
template<class> class Comparison;
class BooleanOperation;
class Quantification;

class Block;
class Choose;
class Conditional;
class Concurrent;
template<class> class Assignment;
template<class> class Pick;
class Call;
class Search;
class Solve;
class Test;
class While;
template<class> class Return;
class DurativeCall;

class AbstractFunction;
template<class> class Function;
using Procedure = Function<Void>;
using VoidFunction = Function<Void>;
using BooleanFunction = Function<Bool>;
using NumericFunction = Function<Number>;
using SymbolicFunction = Function<Symbol>;
using StringFunction = Function<String>;

class AbstractReference;
template<class> class Reference;

class History;

class AbstractSemantics;
template<class> class Semantics;
class SemanticsFactory;

class AExecutionContext;
class ExecutionContext;

class PlatformBackend;

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
	(NumericExpression)(SymbolicExpression)(StringExpression)

#define GOLOGPP_VALUE_TYPES \
	GOLOGPP_COMPARABLE_TYPES (BooleanExpression)

#define GOLOGPP_EXPRESSION_TYPES \
	GOLOGPP_VALUE_TYPES (VoidExpression)

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
	(Action)(ExogAction)(Activity)(Transition) \
	(Scope)(ArithmeticOperation) \
	(Negation)(BooleanOperation)(Quantification)(Concurrent) \
	(Block)(Choose)(Conditional)(Search)(Solve)(Test)(While) \
	(History)(Reference<Action>) \
	(StringConcatenation) \
	(DurativeCall) \
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
