#ifndef READYLOG_PROCEDURE_H_
#define READYLOG_PROCEDURE_H_

#include "semantics.h"
#include "scope.h"
#include "variable.h"
#include "utilities.h"
#include "fluent.h"
#include "reference.h"

#include <model/semantics.h>
#include <model/expressions.h>
#include <model/procedural.h>
#include <model/scope.h>

#include <model/user_error.h>

#include <type_traits>

#include <eclipseclass.h>

namespace gologpp {



template<>
class Semantics<Function> : public ReadylogSemantics {
public:
	Semantics(const Function &function)
	: function_(function)
	{}

	virtual EC_word plterm() override;
	virtual EC_word definition();
	EC_word return_var();

private:
	const Function &function_;
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
	ManagedTerm current_program_;
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



template<>
class Semantics<Concurrent> : public ReadylogSemantics {
public:
	Semantics(const Concurrent &);
	virtual EC_word plterm() override;

private:
	const Concurrent &concurrent_;
};



template<class LhsT>
class Semantics<Assignment<LhsT>> : public ReadylogSemantics {
public:
	Semantics(const Assignment<LhsT> &ass)
	: assignment_(ass)
	{
		throw std::runtime_error(string("Assignment to ") + typeid(LhsT).name() + " is not implemented");
	}

	virtual EC_word plterm() override
	{ return EC_atom("fail"); }

private:
	const Assignment<LhsT> &assignment_;
};



template<>
class Semantics<Assignment<Reference<Fluent>>> : public ReadylogSemantics {
public:
	Semantics(const Assignment<Reference<Fluent>> &ass);

	virtual EC_word plterm() override;

private:
	const Assignment<Reference<Fluent>> &assignment_;
};



template<>
class Semantics<Assignment<FieldAccess>> : public ReadylogSemantics {
public:
	Semantics(const Assignment<FieldAccess> &ass);

	virtual EC_word plterm() override;

private:
	const Assignment<FieldAccess> &assignment_;
	const FieldAccess &field_access_;
};



template<>
class Semantics<Pick> : public ReadylogSemantics {
public:
	Semantics(const Pick &pick);
	virtual EC_word plterm() override;

private:
	const Pick &pick_;
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



template<>
class Semantics<Return> : public ReadylogSemantics {
public:
	Semantics(const Return &r);
	virtual EC_word plterm() override;

private:
	const Return &ret_;
};



template<>
class Semantics<DurativeCall> : public ReadylogSemantics {
public:
	Semantics(const DurativeCall &call);
	virtual EC_word plterm() override;

private:
	const DurativeCall &call_;
};



template<>
class Semantics<FieldAccess> : public ReadylogSemantics {
public:
	Semantics(const FieldAccess &field_access);

	virtual EC_word plterm() override;
	EC_word field_assign(const Expression &value);
	void set_lvalue(bool lvalue);

private:
	const FieldAccess &field_access_;
	bool is_lvalue_;
};



} // namespace gologpp


#endif // READYLOG_PROCEDURE_H_
