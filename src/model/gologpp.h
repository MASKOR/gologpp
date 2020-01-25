#ifndef GOLOGPP_H_
#define GOLOGPP_H_

#include <memory>
#include <vector>
#include <string>
#include <list>

#include <boost/fusion/include/vector.hpp>

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


using string = std::string;


class Expression;
class Instruction;
class Binding;

class AbstractLanguageElement;
template<class, class> class LanguageElement;

class AbstractAction;
class Action;
class ExogAction;
class ExogEvent;
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
class ListAccess;
class ListLength;
class ListPop;
class ListPush;
class During;

class Function;
class Procedure;

class AbstractReference;
template<class> class Reference;
class AbstractGrounding;
template<class> class Grounding;

class CompoundExpression;
class ListExpression;

class History;

template<class> class Semantics;
template<class> class AbstractSemantics;

class SemanticsFactory;

class AExecutionContext;
class ExecutionContext;

class PlatformBackend;

#define GOLOGPP_PREDEFINED_TYPES \
	(NumberType)(BoolType)(StringType)(SymbolType)(VoidType)(UndefinedType)

#define GOLOGPP_TYPES \
	GOLOGPP_PREDEFINED_TYPES(CompoundType)(ListType)

#define GOLOGPP_SEMANTIC_TYPES \
	(EffectAxiom<Reference<Fluent>>) \
	(EffectAxiom<FieldAccess>) \
	(EffectAxiom<ListAccess>) \
	(InitialValue)(Fluent)(Variable) \
	(Value) \
	(Assignment<Reference<Fluent>>) \
	(Assignment<Reference<Variable>>) \
	(Assignment<FieldAccess>) \
	(Assignment<ListAccess>) \
	(Reference<Fluent>) \
	(Reference<Variable>) \
	(Reference<Function>) \
	(Reference<Procedure>) \
	(Pick)(Return) \
	(FieldAccess) \
	(ListAccess)(ListLength) \
	(ListPop)(ListPush) \
	(Domain) \
	(Function) \
	(Procedure) \
	(Action)(Activity)(Transition) \
	(ExogAction)(ExogEvent) \
	(Scope)(ArithmeticOperation) \
	(Negation)(BooleanOperation)(Quantification)(Concurrent) \
	(Block)(Choose)(Search)(Solve)(Test)(While) \
	(Conditional<Instruction>) \
	(Conditional<Expression>) \
	(During) \
	(History)(Reference<Action>) \
	(Reference<ExogAction>) \
	(StringConcatenation) \
	(ToString) \
	(DurativeCall) \
	(Comparison) \
	(CompoundExpression) (ListExpression)


} // namespace gologpp

#endif // GOLOGPP_H_
