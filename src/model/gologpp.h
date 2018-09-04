#ifndef GOLOGPP_H_
#define GOLOGPP_H_

#include <memory>
#include <vector>

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

typedef Constant<BooleanExpression> BooleanConstant;
typedef Constant<NumericExpression> NumericConstant;

typedef Variable<BooleanExpression> BooleanVariable;
typedef Variable<NumericExpression> NumericVariable;

class ArithmeticOperation;

class Negation;
class Comparison;
class BooleanOperation;
class Quantification;

class Block;
class Choose;
class Conditional;
template<class> class Assignment;
class Pick;
class Call;
class Search;
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

class AbstractImplementation;
template<class> class Implementation;
class Implementor;

class AExecutionContext;
class ExecutionContext;


#define DEFINE_IMPLEMENT_WITH_MEMBERS(...) \
	virtual void implement(Implementor &implementor) override { \
		if (!impl_) { \
			impl_ = implementor.make_impl(*this); \
			boost::fusion::for_each(std::tie(__VA_ARGS__), [&] (auto &e) { \
				e.implement(implementor); \
			} ); \
		} \
	}

#define DEFINE_IMPLEMENT \
	virtual void implement(Implementor &implementor) override { \
		if (!impl_) \
			impl_ = implementor.make_impl(*this); \
	}


} // namespace gologpp

#endif // GOLOGPP_H_
