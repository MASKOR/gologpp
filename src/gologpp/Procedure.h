#ifndef PROCEDURE_H
#define PROCEDURE_H

#include <vector>
#include <memory>

#include "Reference.h"
#include "Language.h"


namespace gologpp {
namespace generic {

class Fluent;
class Statement;
class Action;


class Statement : public Expression {
public:
	using Expression::Expression;
	Statement(Statement &&) = default;

	virtual ~Statement();
};


class Block : public Statement, public LanguageElement<Block> {
public:
	Block(const vector<shared_ptr<Statement>> &elements, Scope &parent_scope);
	
private:
	vector<shared_ptr<Statement>> elements_;
};


class Choose : public Statement, public LanguageElement<Choose> {
public:
	Choose(const vector<shared_ptr<Block>> &alternatives, Scope &parent_scope);

private:
	vector<shared_ptr<Block>> alternatives_;
};


class Conditional : public Statement, public LanguageElement<Conditional> {
public:
	Conditional(const shared_ptr<BooleanExpression> &condition,
	            const shared_ptr<Block> &block_true,
	            const shared_ptr<Block> &block_false,
	            Scope &parent_expr);
	
protected:
	shared_ptr<BooleanExpression> condition_;
	shared_ptr<Block> block_true_;
	shared_ptr<Block> block_false_;
};


class Assignment : public Statement, public LanguageElement<Assignment> {
public:
	Assignment(Reference<Fluent> &&fluent, const shared_ptr<Expression> &expression, Scope &parent_scope);

private:
    Reference<Fluent> fluent_;
    shared_ptr<Expression> expression_;
};


class Pick : public Statement, public LanguageElement<Pick> {
public:
	Pick(const shared_ptr<Variable> &variable, const shared_ptr<Block> &block, Scope &parent_scope);
    
private:
	shared_ptr<Variable> variable_;
	shared_ptr<Block> block_;
};


class Call : public Statement, public LanguageElement<Call> {
public:
	Call(const shared_ptr<Action> &action, const vector<shared_ptr<Expression>> &args, Scope &parent_scope);

private:
	shared_ptr<Action> action_;
	vector<shared_ptr<Expression>> args_;
};


class Search : public Statement, public LanguageElement<Search> {
public:
	Search(unique_ptr<Block> &&block, Scope &parent_scope);

private:
	unique_ptr<Block> block_;
};


class Test : public Statement, public LanguageElement<Test> {
public:
    Test(const shared_ptr<BooleanExpression> &expression, Scope &parent_scope);

protected:
	shared_ptr<BooleanExpression> expression_;
};


class While : public Statement, public LanguageElement<While> {
public:
	While(const shared_ptr<BooleanExpression> &expression, unique_ptr<Block> &&block, Scope &parent_scope);

protected:
	shared_ptr<BooleanExpression> expression_;
	unique_ptr<Block> block_;
};


class Procedure : public Statement, public NameWithArity, public LanguageElement<Procedure> {
public:
    Procedure(const string &name, const vector<string> &arg_names, unique_ptr<Block> &&block);
    Procedure(Procedure &&) = default;

protected:
    Scope scope_;
    unique_ptr<Block> block_;
};


} // namespace generic
} // namespace gologpp


#endif // PROCEDURE_H
