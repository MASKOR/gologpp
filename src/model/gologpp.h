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

enum VarDefinitionMode {
	DENY, ALLOW, FORCE
};

typedef unsigned char arity_t;

template<class T>      using shared_ptr  = std::shared_ptr<T>;
template<class T>      using weak_ptr    = std::weak_ptr<T>;
template<class... Ts>  using tuple       = std::tuple<Ts...>;
template<class T>      using vector      = std::vector<T>;


class Expression;
template<class> class TypedExpression;


template<class T>
class unique_ptr : public std::unique_ptr<T> {
public:
	using std::unique_ptr<T>::unique_ptr;
};


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

class Fluent;
class InitialValue;

class Variable;
class Constant;
class Domain;

class ArithmeticOperation;
class StringConcatenation;
class ToString;

class Negation;
class Comparison;
class BooleanOperation;
class Quantification;

class Block;
class Choose;
class Conditional;
class Concurrent;
template<class> class Assignment;
class Pick;
class Call;
class Search;
class Solve;
class Test;
class While;
class Return;
class DurativeCall;
class FieldAccess;

class Function;

template<class> class Reference;
template<class> class Grounding;

class History;

class AbstractSemantics;
template<class> class Semantics;
class SemanticsFactory;

class AExecutionContext;
class ExecutionContext;

class PlatformBackend;

using FluentAssignment = Assignment<Reference<Fluent>>;
using VariableAssignment = Assignment<Reference<Variable>>;
using FieldAssignment = Assignment<FieldAccess>;
using FluentReference = Reference<Fluent>;
using FunctionReference = Reference<Function>;
using VariableReference = Reference<Variable>;
using FluentEffectAxiom = EffectAxiom<Reference<Fluent>>;

#define GOLOGPP_SIMPLE_TYPES \
	(Number)(Bool)(String)(Symbol)(Void)

#define GOLOGPP_TYPES \
	GOLOGPP_SIMPLE_TYPES(CompoundType)

#define GOLOGPP_SEMANTIC_TYPES \
	(FluentEffectAxiom)(InitialValue)(Fluent)(Variable) \
	(Constant) \
	(FluentAssignment)(VariableAssignment) \
	(FluentReference)(VariableReference)(Pick)(Return) \
	(FieldAccess) \
	(FieldAssignment) \
	(Domain) \
	(Function)(FunctionReference) \
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
	(Comparison)


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
