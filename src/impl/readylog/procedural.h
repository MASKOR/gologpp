#ifndef READYLOG_PROCEDURE_H_
#define READYLOG_PROCEDURE_H_

#include "implementation.h"
#include "scope.h"
#include "atoms.h"

#include <model/implementation.h>
#include <model/expressions.h>
#include <model/procedural.h>
#include <model/scope.h>

#include <model/user_error.h>

#include <type_traits>

#include <eclipseclass.h>

namespace gologpp {


/*
 * The implementations of the different function types that follow show how
 * different implementations can be used depending on the Expression type argument.
 */


/*
 * 1. Define the abstract superclass that is used by the ExecutionContext
 *    It can produce a term that references a function since that always looks the same.
 *    The definition is however pure virtual since that will look different in readylog
 *    functions and procedures.
 */
template<>
class Implementation<AbstractFunction> : public ReadylogImplementation {
public:
	Implementation(const AbstractFunction &function)
	: function_(function)
	{}

	virtual EC_word term() override;
	virtual EC_word definition() = 0;

protected:
	const AbstractFunction &function_;
};


/*
 * 2. Implementation for all Function types that get translated into readylog procedures.
 *    This class is only used indirectly (see below)
 */
template<class ExprT>
class ReadylogProcedure : public Implementation<AbstractFunction> {
public:
	using Implementation<AbstractFunction>::Implementation;

	virtual EC_word definition() override
	{
		function_.scope().implementation().init_vars();
		return ::term(EC_functor("proc", 2),
			term(),
			function_.definition().implementation().term()
		);
	}
};


/*
 * 3. Map ReadylogProcedure class to to Function types that should use it.
 */
template<>
class Implementation<Function<Statement>> : public ReadylogProcedure<Statement> {
public:
	using ReadylogProcedure<Statement>::ReadylogProcedure;
};


template<>
class Implementation<Function<BooleanExpression>> : public ReadylogProcedure<BooleanExpression> {
public:
	using ReadylogProcedure<BooleanExpression>::ReadylogProcedure;
};



/*
 * 4. Implement all remaining types as readylog functions.
 */
template<class ExprT>
class Implementation<Function<ExprT>> : public Implementation<AbstractFunction> {
public:
	using Implementation<AbstractFunction>::Implementation;

	virtual EC_word definition() override
	{
		function_.scope().implementation().init_vars();
		return_var_ = ::newvar();

		return ::term(EC_functor("function", 3),
			term(),
			return_var_,
			function_.definition().implementation().term()
		);
	}


	EC_word return_var()
	{ return return_var_; }

protected:
	EC_word return_var_;
};



template<>
class Implementation<Block> : public ReadylogImplementation {
public:
	Implementation(const Block &);
	virtual EC_word term() override;
	EC_word current_program();
	void set_current_program(EC_word e);

private:
	const Block &block_;
	EC_word current_program_;
};


template<>
class Implementation<Choose> : public ReadylogImplementation {
public:
	Implementation(const Choose &);
	virtual EC_word term() override;

private:
	const Choose &choose_;
};


template<>
class Implementation<Conditional> : public ReadylogImplementation {
public:
	Implementation(const Conditional &);
	virtual EC_word term() override;

private:
	const Conditional &conditional_;
};



template<class LhsT>
class Implementation<Assignment<LhsT>> : public ReadylogImplementation {
public:
	Implementation(const Assignment<LhsT> &ass)
	: assignment_(ass)
	{}

	virtual EC_word term() override
	{
		return ::term(EC_functor("=", 2),
			assignment_.lhs().implementation().term(),
			assignment_.rhs().implementation().term()
		);
	}

private:
	const Assignment<LhsT> &assignment_;
};



template<class ExpressionT>
class Implementation<Assignment<Variable<ExpressionT>>> : public ReadylogImplementation {
public:
	Implementation(const Assignment<Variable<ExpressionT>> &ass)
	: assignment_(ass)
	{
		assignment_.lhs().target()->implementation().translate_as_golog_var(true);
	}

	virtual EC_word term() override
	{
		return ::term(EC_functor("=", 2),
			assignment_.lhs().implementation().term(),
			assignment_.rhs().implementation().term()
		);
	}

private:
	const Assignment<Variable<ExpressionT>> &assignment_;
};


template<class ExprT>
class Implementation<Pick<ExprT>> : public ReadylogImplementation {
public:
	Implementation(const Pick<ExprT> &pick)
	: pick_(pick)
	{
		if (pick_.domain().empty())
			throw std::runtime_error("ReadyLog requires a domain for pick()!");
	}

	virtual EC_word term() override
	{
		// Make sure the `pick'ed variable is a Golog variable
		// No init_vars() is needed in this case.
		{ GologVarMutator guard(pick_.variable().implementation());
			return ::term(EC_functor("pickBest", 3),
				pick_.variable().implementation().term(),
				to_ec_list(pick_.domain(), pick_.domain().begin()),
				pick_.statement().implementation().term()
			);
		}
	}


private:
	const Pick<ExprT> &pick_;
};


template<>
class Implementation<Search> : public ReadylogImplementation {
public:
	Implementation(const Search &);
	virtual EC_word term() override;

private:
	const Search &search_;
};


template<>
class Implementation<Solve> : public ReadylogImplementation {
public:
	Implementation(const Solve &);
	virtual EC_word term() override;

private:
	const Solve &solve_;
};


template<>
class Implementation<Test> : public ReadylogImplementation {
public:
	Implementation(const Test &);
	virtual EC_word term() override;

private:
	const Test &test_;
};


template<>
class Implementation<While> : public ReadylogImplementation {
public:
	Implementation(const While &);
	virtual EC_word term() override;

private:
	const While &while_;
};


template<class ExpressionT>
class Implementation<Return<ExpressionT>> : public ReadylogImplementation {
public:
	Implementation(const Return<ExpressionT> &r)
	: ret_(r)
	{}

	virtual EC_word term() override {
		const AbstractLanguageElement *root_parent = ret_.parent();
		const Expression *parent_expr = nullptr;
		while ((parent_expr = dynamic_cast<const Expression *>(root_parent))) {
			root_parent = parent_expr->parent();
		}

		try {
			const Function<ExpressionT> &function = dynamic_cast<const Function<ExpressionT> &>(*root_parent);
			return ::term(EC_functor("=", 2),
				function.implementation().return_var(),
				ret_.expression().implementation().term()
			);
		} catch (std::bad_cast &) {
			throw Bug(ret_.str() + ": Wrong type");
		}
	}

private:
	const Return<ExpressionT> &ret_;
};


template<>
EC_word Implementation<Return<BooleanExpression>>::term();



} // namespace gologpp


#endif // READYLOG_PROCEDURE_H_
