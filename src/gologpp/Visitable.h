#ifndef GOLOGPP_VISITABLE_H_
#define GOLOGPP_VISITABLE_H_

#include "Execution.h"

namespace gologpp {
namespace generic {


class Action;
class EffectAxiom;

class Fluent;

class Value;
class Variable;

class Negation;
class Comparison;
class Conjunction;
class Disjunction;
class Implication;
class ExistentialQuantification;
class UniversalQuantification;

class Block;
class Choose;
class Conditional;
class Assignment;
class Pick;
class Call;
class Search;
class Test;
class While;
class Procedure;

class Visitor {
public:
	virtual ~Visitor();

	virtual void visit(Action &) = 0;
	virtual void visit(EffectAxiom &) = 0;

	virtual void visit(Fluent &) = 0;

	virtual void visit(Value &) = 0;
	virtual void visit(Variable &) = 0;

	virtual void visit(Negation &) = 0;
	virtual void visit(Comparison &) = 0;
	virtual void visit(Conjunction &) = 0;
	virtual void visit(Disjunction &) = 0;
	virtual void visit(Implication &) = 0;
	virtual void visit(ExistentialQuantification &) = 0;
	virtual void visit(UniversalQuantification &) = 0;

	virtual void visit(Block &) = 0;
	virtual void visit(Choose &) = 0;
	virtual void visit(Conditional &) = 0;
	virtual void visit(Assignment &) = 0;
	virtual void visit(Pick &) = 0;
	virtual void visit(Call &) = 0;
	virtual void visit(Search &) = 0;
	virtual void visit(Test &) = 0;
	virtual void visit(While &) = 0;
	virtual void visit(Procedure &) = 0;


};


class Visitable {
public:
	virtual ~Visitable();

	virtual void accept(Visitor &visitor);
};


}
}


#endif // GOLOGPP_VISITABLE_H_