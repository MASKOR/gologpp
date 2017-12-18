#ifndef PROCEDURE_H
#define PROCEDURE_H

#include <vector>
#include <memory>

#include "Formula.h"
#include "Fluent.h"
#include "Action.h"
#include "Situation.h"
#include "Execution.h"


namespace gologpp {

using namespace std;


class Statement : public Expression {
public:
	virtual ~Statement();
};


class Block : public Statement {
public:
	Block(const vector<shared_ptr<Statement>> &elements = {});
	
private:
	const vector<shared_ptr<Statement>> elements_;
};


class Choose : public Statement {
public:
	Choose(const vector<shared_ptr<Block>> &alternatives = {});

private:
	const vector<shared_ptr<Block>> alternatives_;
};


class Conditional : public Statement {
public:
	Conditional(const shared_ptr<Expression> &condition,
	            const shared_ptr<Block> &block_true,
	            const shared_ptr<Block> &block_false);
	
protected:
	const shared_ptr<Expression> condition_;
	const shared_ptr<Block> block_true_;
	const shared_ptr<Block> block_false_;
};


class Assignment : public Statement {
public:
	Assignment(const shared_ptr<Fluent> &fluent, const shared_ptr<Expression> &expression);

private:
    const shared_ptr<Fluent> fluent_;
    const shared_ptr<Expression> expression_;
};


class Pick : public Statement {
public:
	Pick(const shared_ptr<Variable> &variable, const shared_ptr<Block> &block);
    
private:
	const shared_ptr<Variable> variable_;
	const shared_ptr<Block> block_;
};


class Call : public Statement {
public:
	Call(const shared_ptr<Action> &action, const vector<shared_ptr<Expression>> &args = {});

private:
	const shared_ptr<Action> action_;
	const vector<shared_ptr<Expression>> args_;
};


class Search : public Statement {
public:
	Search(unique_ptr<Block> &&block);

private:
	const unique_ptr<Block> block_;
};


class Test : public Statement {
public:
    Test(const shared_ptr<Expression> &expression);

protected:
	const shared_ptr<Expression> expression_;
};


class While : public Statement {
public:
	While(const shared_ptr<Expression> &expression, unique_ptr<Block> &&block);

protected:
	const shared_ptr<Expression> expression_;
	const unique_ptr<Block> block_;
};


class Procedure : public Statement, public NameWithArity<Procedure> {
public:
    Procedure(const string &name, arity_t arity, unique_ptr<Block> &&block);

protected:
    const unique_ptr<Block> block_;
};


} // namespace gologpp


#endif // PROCEDURE_H
