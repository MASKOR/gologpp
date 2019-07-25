#ifndef GOLOGPP_H_
#define GOLOGPP_H_

#include <memory>
#include <vector>
#include <string>

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


class Expression;


using string = std::string;

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
class CompoundType;

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

#define GOLOGPP_PREDEFINED_TYPES \
	(NumberType)(BoolType)(StringType)(SymbolType)(VoidType)

#define GOLOGPP_TYPES \
	GOLOGPP_PREDEFINED_TYPES(CompoundType)

#define GOLOGPP_SEMANTIC_TYPES \
	(EffectAxiom<Reference<Fluent>>) \
	(EffectAxiom<FieldAccess>) \
	(InitialValue)(Fluent)(Variable) \
	(Value) \
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
	(Grounding<ExogAction>) \
	(Reference<ExogAction>) \
	(StringConcatenation) \
	(ToString) \
	(DurativeCall) \
	(Comparison)


} // namespace gologpp

#endif // GOLOGPP_H_
