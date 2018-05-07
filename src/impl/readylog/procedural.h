#ifndef READYLOG_PROCEDURE_H_
#define READYLOG_PROCEDURE_H_

#include "implementation.h"

#include <model/implementation.h>
#include <model/expressions.h>
#include <model/procedural.h>

#include <model/user_error.h>

#include <type_traits>

#include <eclipseclass.h>

namespace gologpp {

template<>
class Implementation<Procedure> : public ReadylogImplementation {
public:
	Implementation(const Procedure &proc);

	virtual EC_word term() override;
	EC_word definition();

private:
	const Procedure &procedure_;
};


template<>
class Implementation<AbstractFunction> : public ReadylogImplementation {
public:
	Implementation(const AbstractFunction &function);

	virtual EC_word term() override;
	EC_word definition();
	EC_word return_var();

private:
	const AbstractFunction &function_;
	EC_word return_var_;
};


template<class ExprT>
class Implementation<Function<ExprT>> : public Implementation<AbstractFunction> {
public:
	using Implementation<AbstractFunction>::Implementation;
};


template<>
class Implementation<Block> : public ReadylogImplementation {
public:
	Implementation(const Block &);
	virtual EC_word term() override;
	EC_ref current_program();
	void set_current_program(EC_ref e);

private:
	const Block &block_;
	EC_ref current_program_;
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


template<class ExpressionT>
class Implementation<Assignment<ExpressionT>> : public ReadylogImplementation {
public:
	Implementation(const Assignment<ExpressionT> &ass)
	: assignment_(ass)
	{}

	virtual EC_word term() override
	{
		return ::term(EC_functor("=", 2),
			assignment_.fluent().implementation().term(),
			assignment_.expression().implementation().term()
		);
	}

private:
	const Assignment<ExpressionT> &assignment_;
};


template<>
class Implementation<Pick> : public ReadylogImplementation {
public:
	Implementation(const Pick &);
	virtual EC_word term() override;

private:
	const Pick &pick_;
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
		shared_ptr<Expression> root_parent = ret_.parent_scope().owner();
		while (&root_parent->parent_scope() != &global_scope())
			root_parent = root_parent->parent_scope().owner();

		try {
			Function<ExpressionT> &function = dynamic_cast<Function<ExpressionT> &>(*root_parent);
			return ::term(EC_functor("=", 2),
				function.implementation().return_var(),
				ret_.implementation().term()
			);
		} catch (std::bad_cast &) {
			throw SemanticError(string(typeid(ret_).name()) + ": Return type mismatch");
		}
	}

private:
	const Return<ExpressionT> &ret_;
};


} // namespace gologpp


#endif // READYLOG_PROCEDURE_H_
