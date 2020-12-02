#ifndef GOLOGPP_H_
#define GOLOGPP_H_

#include <memory>
#include <vector>
#include <string>
#include <list>
#include <optional>

#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/container/map.hpp>

namespace gologpp {

enum VarDefinitionMode {
	DENY, ALLOW, FORCE
};

typedef unsigned char arity_t;

template<class T>      using shared_ptr  = std::shared_ptr<T>;
template<class T>      using weak_ptr    = std::weak_ptr<T>;
template<class... Ts>  using tuple       = std::tuple<Ts...>;
template<class T>      using vector      = std::vector<T>;


template<class T>
class unique_ptr : public std::unique_ptr<T> {
public:
	using std::unique_ptr<T>::unique_ptr;
};


template<class T1, class T2>
#ifdef BOOST_FUSION_HAS_VARIADIC_VECTOR
using fusion_wtf_vector = boost::fusion::vector<T1, T2>;
#else
using fusion_wtf_vector = boost::fusion::vector2<T1, T2>;
#endif


template<class T>
using optional = std::optional<T>;
using std::nullopt;


using string = std::string;

class Logger;

class Expression;
class Instruction;

class Binding;
using BindingChain = vector<const Binding *>;

class ModelElement;
class Plan;
class PlanTransformation;
class DummyPlanTransformation;

class AbstractLanguageElement;
template<class, class> class LanguageElement;

class AbstractAction;
class Action;
class ExogAction;

class BackendMapping;

class Activity;
class Transition;

class Identifier;

class Global;

class Type;
class BoolType;
class NumberType;
class StringType;
class SymbolType;
class VoidType;
class UndefinedType;
class CompoundType;
class ListType;

using TypeList = std::list<std::reference_wrapper<const Type>>;


class AbstractEffectAxiom;
template<class> class EffectAxiom;

class Scope;
class ScopeOwner;
Scope &global_scope();

class Fluent;
class InitialValue;

class Variable;
class Value;
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
template<class> class Conditional;
class Concurrent;
class AbstractAssignment;
template<class> class Assignment;
class Pick;
class Call;
class Search;
class Solve;
class Test;
class While;
class DurativeCall;
class FieldAccess;
class ListAccess;
class ListLength;
class ListPop;
class ListPush;
class During;

class Function;
class Procedure;
class ExogFunction;

class AbstractReference;
template<class> class Reference;

class CompoundExpression;
class ListExpression;

class History;

template<class> class Semantics;
template<class> class GeneralSemantics;

class SemanticsFactory;

class AExecutionController;
class ExecutionController;

class PlatformBackend;

using ExogEvent = Reference<ExogAction>;



namespace platform {

class ActionSpec;
class StateSpec;

class Component;
class ComponentBackend;
class Transition;
class ExogTransition;
class Clock;
class State;

class SwitchStateAction;

class ClockFormula;
class ClockBound;
class BooleanClockOperation;
class ClockNegation;

class Constraint;
class StateAssertion;
class ActionHook;
class During;
template<class> class BooleanConstraintOperation;
template<class> class TemporalUnaryOperation;
template<class> class TemporalBinaryOperation;

template<class> class Reference;

class SemanticsFactory;

} // namespace platform


#define GOLOGPP_PREDEFINED_TYPES \
	(NumberType)(BoolType)(StringType)(SymbolType)(VoidType)(UndefinedType)

#define GOLOGPP_TYPES \
	GOLOGPP_PREDEFINED_TYPES(CompoundType)(ListType)

#define GOLOGPP_INSTRUCTIONS \
	(Assignment<Reference<Fluent>>) \
	(Assignment<FieldAccess>) \
	(Assignment<ListAccess>) \
	(Pick) \
	(Concurrent) \
	(Block) \
	(Choose) \
	(Search) \
	(Solve) \
	(Test) \
	(While) \
	(Conditional<Instruction>) \
	(During) \
	(Reference<Action>) \
	(Reference<ExogAction>) \
	(DurativeCall) \
	(Reference<Procedure>) \
	(ListPop) \
	(ListPush)

#define GOLOGPP_EXPRESSIONS \
	(Variable) \
	(Value) \
	(Reference<Fluent>) \
	(Reference<Variable>) \
	(Reference<Function>) \
	(Reference<ExogFunction>) \
	(FieldAccess) \
	(ListAccess) \
	(ListLength) \
	(ArithmeticOperation) \
	(Negation) \
	(BooleanOperation) \
	(Quantification) \
	(Conditional<Expression>) \
	(StringConcatenation) \
	(ToString) \
	(Comparison) \
	(CompoundExpression) \
	(ListExpression)

#define GOLOGPP_OTHER \
	(EffectAxiom<Reference<Fluent>>) \
	(EffectAxiom<FieldAccess>) \
	(EffectAxiom<ListAccess>) \
	(InitialValue)(Fluent) \
	(Binding) \
	(Domain) \
	(Function) \
	(ExogFunction) \
	(Procedure) \
	(Action) \
	(Activity) \
	(Transition) \
	(ExogAction) \
	(Scope) \
	(History)

#define GOLOGPP_PLATFORM_ELEMENTS \
	(platform::Component) \
	(platform::Transition) \
	(platform::ExogTransition) \
	(platform::Clock) \
	(platform::State) \
	(platform::ClockBound) \
	(platform::ClockNegation) \
	(platform::BooleanClockOperation) \
	(platform::Reference<platform::Clock>) \
	(platform::Reference<platform::Component>) \
	(platform::Reference<platform::State>) \
	(platform::Reference<Action>) \
	(platform::Constraint) \
	(platform::StateAssertion) \
	(platform::ActionHook) \
	(platform::During) \
	(platform::BooleanConstraintOperation<platform::ActionSpec>) \
	(platform::BooleanConstraintOperation<platform::StateSpec>) \
	(platform::TemporalUnaryOperation<platform::ActionSpec>) \
	(platform::TemporalUnaryOperation<platform::StateSpec>) \
	(platform::TemporalBinaryOperation<platform::ActionSpec>) \
	(platform::TemporalBinaryOperation<platform::StateSpec>) \

#define GOLOGPP_SEMANTIC_TYPES \
	GOLOGPP_INSTRUCTIONS \
	GOLOGPP_EXPRESSIONS \
	GOLOGPP_OTHER


/*
using elem_types = boost::mpl::map <
boost::mpl::pair<Assignment<Reference<Fluent>>, Instruction>,
boost::mpl::pair<Assignment<Reference<Variable>>, Instruction>,
boost::mpl::pair<Assignment<FieldAccess>, Instruction>,
boost::mpl::pair<Assignment<ListAccess>, Instruction>,
boost::mpl::pair<Pick, Instruction>,
boost::mpl::pair<Plan, Instruction>,
boost::mpl::pair<Concurrent, Instruction>,
boost::mpl::pair<Block, Instruction>,
boost::mpl::pair<Choose, Instruction>,
boost::mpl::pair<Search, Instruction>,
boost::mpl::pair<Solve, Instruction>,
boost::mpl::pair<Test, Instruction>,
boost::mpl::pair<While, Instruction>,
boost::mpl::pair<Conditional<Instruction>, Instruction>,
boost::mpl::pair<During, Instruction>,
boost::mpl::pair<Reference<Action>, Instruction>,
boost::mpl::pair<Reference<ExogAction>, Instruction>,
boost::mpl::pair<DurativeCall, Instruction>,
boost::mpl::pair<Reference<Procedure>, Instruction>,
boost::mpl::pair<ListPop, Instruction>,
boost::mpl::pair<ListPush, Instruction>

>;*/

} // namespace gologpp

#endif // GOLOGPP_H_
