#ifndef READYLOG_PROCEDURE_H_
#define READYLOG_PROCEDURE_H_

#include "semantics.h"
#include "scope.h"
#include "atoms.h"

#include <model/semantics.h>
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
class Semantics<AbstractFunction> : public ReadylogSemantics {
public:
	Semantics(const AbstractFunction &function)
	: function_(function)
	{}

	virtual EC_word plterm() override;
	virtual EC_word definition() = 0;

protected:
	const AbstractFunction &function_;
};


/*
 * 2. Semantics for all Function types that get translated into readylog procedures.
 *    This class is only used indirectly (see below)
 */
template<class ExprT>
class ReadylogProcedure : public Semantics<AbstractFunction> {
public:
	using Semantics<AbstractFunction>::Semantics;

	virtual EC_word definition() override
	{
		function_.scope().semantics().init_vars();
		return ::term(EC_functor("proc", 2),
			plterm(),
			function_.definition().semantics().plterm()
		);
	}
};


/*
 * 3. Map ReadylogProcedure class to to Function types that should use it.
 */
template<>
class Semantics<Function<Statement>> : public ReadylogProcedure<Statement> {
public:
	using ReadylogProcedure<Statement>::ReadylogProcedure;
};


template<>
class Semantics<Function<BooleanExpression>> : public ReadylogProcedure<BooleanExpression> {
public:
	using ReadylogProcedure<BooleanExpression>::ReadylogProcedure;
};



/*
 * 4. Implement all remaining types as readylog functions.
 */
template<class ExprT>
class Semantics<Function<ExprT>> : public Semantics<AbstractFunction> {
public:
	using Semantics<AbstractFunction>::Semantics;

	virtual EC_word definition() override
	{
		function_.scope().semantics().init_vars();
		return_var_ = ::newvar();

		return ::term(EC_functor("function", 3),
			plterm(),
			return_var_,
			function_.definition().semantics().plterm()
		);
	}


	EC_word return_var()
	{ return return_var_; }

protected:
	EC_word return_var_;
};



template<>
class Semantics<Block> : public ReadylogSemantics {
public:
	Semantics(const Block &);
	virtual EC_word plterm() override;
	EC_word current_program();
	void set_current_program(EC_word e);

private:
	const Block &block_;
	EC_word current_program_;
};


template<>
class Semantics<Choose> : public ReadylogSemantics {
public:
	Semantics(const Choose &);
	virtual EC_word plterm() override;

private:
	const Choose &choose_;
};


template<>
class Semantics<Conditional> : public ReadylogSemantics {
public:
	Semantics(const Conditional &);
	virtual EC_word plterm() override;

private:
	const Conditional &conditional_;
};



template<class LhsT>
class Semantics<Assignment<LhsT>> : public ReadylogSemantics {
public:
	Semantics(const Assignment<LhsT> &ass)
	: assignment_(ass)
	{}

	virtual EC_word plterm() override
	{
		return ::term(EC_functor("=", 2),
			assignment_.lhs().semantics().plterm(),
			assignment_.rhs().semantics().plterm()
		);
	}

private:
	const Assignment<LhsT> &assignment_;
};



template<class ExpressionT>
class Semantics<Assignment<Variable<ExpressionT>>> : public ReadylogSemantics {
public:
	Semantics(const Assignment<Variable<ExpressionT>> &ass)
	: assignment_(ass)
	{
		assignment_.lhs().target()->semantics().translate_as_golog_var(true);
	}

	virtual EC_word plterm() override
	{
		return ::term(EC_functor("=", 2),
			assignment_.lhs().semantics().plterm(),
			assignment_.rhs().semantics().plterm()
		);
	}

private:
	const Assignment<Variable<ExpressionT>> &assignment_;
};


template<class ExprT>
class Semantics<Pick<ExprT>> : public ReadylogSemantics {
public:
	Semantics(const Pick<ExprT> &pick)
	: pick_(pick)
	{
		if (pick_.domain().empty())
			throw std::runtime_error("ReadyLog requires a domain for pick()!");
	}

	virtual EC_word plterm() override
	{
		// Make sure the `pick'ed variable is a Golog variable
		// No init_vars() is needed in this case.
		{ GologVarMutator guard(pick_.variable().semantics());
			return ::term(EC_functor("pickBest", 3),
				pick_.variable().semantics().plterm(),
				to_ec_list(pick_.domain(), pick_.domain().begin()),
				pick_.statement().semantics().plterm()
			);
		}
	}


private:
	const Pick<ExprT> &pick_;
};


template<>
class Semantics<Search> : public ReadylogSemantics {
public:
	Semantics(const Search &);
	virtual EC_word plterm() override;

private:
	const Search &search_;
};


template<>
class Semantics<Solve> : public ReadylogSemantics {
public:
	Semantics(const Solve &);
	virtual EC_word plterm() override;

private:
	const Solve &solve_;
};


template<>
class Semantics<Test> : public ReadylogSemantics {
public:
	Semantics(const Test &);
	virtual EC_word plterm() override;

private:
	const Test &test_;
};


template<>
class Semantics<While> : public ReadylogSemantics {
public:
	Semantics(const While &);
	virtual EC_word plterm() override;

private:
	const While &while_;
};


template<class ExpressionT>
class Semantics<Return<ExpressionT>> : public ReadylogSemantics {
public:
	Semantics(const Return<ExpressionT> &r)
	: ret_(r)
	{}

	virtual EC_word plterm() override {
		const AbstractLanguageElement *root_parent = ret_.parent();
		const Expression *parent_expr = nullptr;
		while ((parent_expr = dynamic_cast<const Expression *>(root_parent))) {
			root_parent = parent_expr->parent();
		}

		try {
			const Function<ExpressionT> &function = dynamic_cast<const Function<ExpressionT> &>(*root_parent);
			return ::term(EC_functor("=", 2),
				function.semantics().return_var(),
				ret_.expression().semantics().plterm()
			);
		} catch (std::bad_cast &) {
			throw Bug(ret_.str() + ": Wrong type");
		}
	}

private:
	const Return<ExpressionT> &ret_;
};


template<>
EC_word Semantics<Return<BooleanExpression>>::plterm();



} // namespace gologpp


#endif // READYLOG_PROCEDURE_H_
