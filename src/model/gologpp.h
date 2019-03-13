#ifndef GOLOGPP_H_
#define GOLOGPP_H_

#include <memory>
#include <vector>
#include <string>

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
class ExogEvent;
class Activity;
class Transition;

class Identifier;

class Global;


class Type;
class Bool;
class Number;
class String;
class Symbol;
class Void;
class CompoundType;

class Expression;

template<class> class TypedExpression;

using BooleanExpression = TypedExpression<Bool>;
using NumericExpression = TypedExpression<Number>;
using SymbolicExpression = TypedExpression<Symbol>;
using StringExpression = TypedExpression<String>;
using VoidExpression = TypedExpression<Void>;
using CompoundExpression = TypedExpression<CompoundType>;


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
typedef Fluent<SymbolicExpression> SymbolicFluent;
typedef Fluent<StringExpression> StringFluent;
typedef Fluent<CompoundExpression> CompoundFluent;

class AbstractVariable;
template<class> class Variable;

class AbstractConstant;
template<class> class Constant;

class AbstractDomain;
template<class> class Domain;

typedef Constant<BooleanExpression> BooleanConstant;
typedef Constant<NumericExpression> NumericConstant;
typedef Constant<SymbolicExpression> SymbolicConstant;
typedef Constant<StringExpression> StringConstant;
typedef Constant<CompoundExpression> CompoundConstant;

typedef Variable<BooleanExpression> BooleanVariable;
typedef Variable<NumericExpression> NumericVariable;
typedef Variable<SymbolicExpression> SymbolicVariable;
typedef Variable<StringExpression> StringVariable;
typedef Variable<CompoundExpression> CompoundVariable;

class ArithmeticOperation;
class StringConcatenation;
class ToString;

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
template<class> class FieldAccess;

class AbstractFunction;
template<class> class Function;
using Procedure = Function<VoidExpression>;
using VoidFunction = Function<VoidExpression>;
using BooleanFunction = Function<BooleanExpression>;
using NumericFunction = Function<NumericExpression>;
using SymbolicFunction = Function<SymbolicExpression>;
using StringFunction = Function<StringExpression>;

class AbstractReference;
template<class> class Reference;
template<class> class Grounding;

class History;

class AbstractSemantics;
template<class> class Semantics;
class SemanticsFactory;

class AExecutionContext;
class ExecutionContext;

class PlatformBackend;

template<class ExprT>
using FluentAssignment = Assignment<Reference<Fluent<ExprT>>>;

template<class ExprT>
using VariableAssignment = Assignment<Reference<Variable<ExprT>>>;

template<class ExprT>
using FluentReference = Reference<Fluent<ExprT>>;

template<class ExprT>
using FunctionReference = Reference<Function<ExprT>>;

template<class ExprT>
using VariableReference = Reference<Variable<ExprT>>;

template<class ExprT>
using FluentEffectAxiom = EffectAxiom<Reference<Fluent<ExprT>>>;


#define GOLOGPP_COMPARABLE_TYPES \
	(NumericExpression)(SymbolicExpression)(StringExpression)

#define GOLOGPP_VALUE_TYPES \
	GOLOGPP_COMPARABLE_TYPES \
	(CompoundExpression) \
	(BooleanExpression)

#define GOLOGPP_EXPRESSION_TYPES \
	GOLOGPP_VALUE_TYPES (VoidExpression)

#define GOLOGPP_SIMPLE_TYPES \
	(Number)(Bool)(String)(Symbol)(Void)

#define GOLOGPP_VALUE_TYPE_TEMPLATES \
	(FluentEffectAxiom)(InitialValue)(Fluent)(Variable)(Constant) \
	(FluentAssignment)(VariableAssignment) \
	(FluentReference)(VariableReference)(Pick)(Return) \
	(FieldAccess) \
	(Domain)

#define GOLOGPP_EXPRESSION_TYPE_TEMPLATES \
	(Function)(FunctionReference)

#define GOLOGPP_TEMPLATE_CLASS(_, seq) \
	(BOOST_PP_SEQ_ELEM(0, seq)<BOOST_PP_SEQ_ELEM(1, seq)>)

#define GOLOGPP_SEMANTIC_TYPES \
	(Action)(Activity)(Transition) \
	(ExogAction)(ExogEvent) \
	(Scope)(ArithmeticOperation) \
	(Negation)(BooleanOperation)(Quantification)(Concurrent) \
	(Block)(Choose)(Conditional)(Search)(Solve)(Test)(While) \
	(History)(Reference<Action>)(Grounding<Action>) \
	(Reference<ExogAction>) \
	(StringConcatenation) \
	(ToString) \
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
