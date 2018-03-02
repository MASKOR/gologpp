#ifndef PROCEDURE_H
#define PROCEDURE_H

#include <vector>
#include <memory>

#include "Reference.h"
#include "Language.h"
#include "Scope.h"


namespace gologpp {

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
	Block(vector<unique_ptr<Statement>> &&elements, Scope &parent_scope);
	void implement(Implementor &);

private:
	vector<unique_ptr<Statement>> elements_;
};


class Choose : public Statement, public LanguageElement<Choose> {
public:
	Choose(vector<unique_ptr<Block>> &&alternatives, Scope &parent_scope);
	void implement(Implementor &);

private:
	vector<unique_ptr<Block>> alternatives_;
};


class Conditional : public Statement, public LanguageElement<Conditional> {
public:
	Conditional(unique_ptr<BooleanExpression> &&condition,
	            unique_ptr<Block> &&block_true,
	            unique_ptr<Block> &&block_false,
	            Scope &parent_expr);
	DEFINE_IMPLEMENT_WITH_MEMBERS(*condition_, *block_true_, *block_false_)

protected:
	unique_ptr<BooleanExpression> condition_;
	unique_ptr<Block> block_true_;
	unique_ptr<Block> block_false_;
};


class Assignment : public Statement, public LanguageElement<Assignment> {
public:
	Assignment(Reference<Fluent> &&fluent, unique_ptr<Expression> &&expression, Scope &parent_scope);
	DEFINE_IMPLEMENT_WITH_MEMBERS(fluent_, *expression_)

private:
    Reference<Fluent> fluent_;
    unique_ptr<Expression> expression_;
};


class Pick : public Statement, public LanguageElement<Pick> {
public:
	Pick(const shared_ptr<Variable> &variable, unique_ptr<Block> &&block, Scope &parent_scope);
	DEFINE_IMPLEMENT_WITH_MEMBERS(*variable_, *block_)

private:
	shared_ptr<Variable> variable_;
	unique_ptr<Block> block_;
};


class Search : public Statement, public LanguageElement<Search> {
public:
	Search(unique_ptr<Block> &&block, Scope &parent_scope);
	DEFINE_IMPLEMENT_WITH_MEMBERS(*block_)

private:
	unique_ptr<Block> block_;
};


class Test : public Statement, public LanguageElement<Test> {
public:
    Test(unique_ptr<BooleanExpression> &&expression, Scope &parent_scope);
	DEFINE_IMPLEMENT_WITH_MEMBERS(*expression_)

protected:
	unique_ptr<BooleanExpression> expression_;
};


class While : public Statement, public LanguageElement<While> {
public:
	While(unique_ptr<BooleanExpression> &&expression, unique_ptr<Block> &&block, Scope &parent_scope);
	DEFINE_IMPLEMENT_WITH_MEMBERS(*expression_, *block_)

protected:
	unique_ptr<BooleanExpression> expression_;
	unique_ptr<Block> block_;
};


class Procedure : public Statement, public Identifier, public LanguageElement<Procedure> {
public:
    Procedure(const string &name, const vector<string> &arg_names, unique_ptr<Block> &&block);
    Procedure(Procedure &&) = default;
	DEFINE_IMPLEMENT_WITH_MEMBERS(scope_, *block_)

	const Scope &scope() const;
	const Block &block() const;
	const vector<string> &args() const;

protected:
    Scope scope_;
    unique_ptr<Block> block_;
    vector<string> args_;
};


} // namespace gologpp


#endif // PROCEDURE_H
