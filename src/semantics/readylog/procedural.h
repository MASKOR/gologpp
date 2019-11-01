/*************************************************************************
 * This file is part of golog++.
 *
 * golog++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * golog++ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with golog++.  If not, see <https://www.gnu.org/licenses/>.
**************************************************************************/

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
class Semantics<Function>
: public Semantics<AbstractLanguageElement>
, public AbstractSemantics<Function>
{
public:
	Semantics(const Function &function);
	virtual EC_word plterm() override;
	virtual EC_word definition();
	EC_word return_var();

private:
	EC_word return_var_;
};



template<>
class Semantics<Block>
: public Semantics<AbstractLanguageElement>
, public AbstractSemantics<Block>
{
public:
	Semantics(const Block &);
	virtual EC_word plterm() override;
	EC_word current_program();
	void set_current_program(EC_word e);

private:
	ManagedTerm current_program_;
};



template<>
class Semantics<Choose>
: public Semantics<AbstractLanguageElement>
, public AbstractSemantics<Choose>
{
public:
	Semantics(const Choose &);
	virtual EC_word plterm() override;
};



template<>
class Semantics<Conditional>
: public Semantics<AbstractLanguageElement>
, public AbstractSemantics<Conditional>
{
public:
	Semantics(const Conditional &);
	virtual EC_word plterm() override;
};



template<>
class Semantics<Concurrent>
: public Semantics<AbstractLanguageElement>
, public AbstractSemantics<Concurrent>
{
public:
	Semantics(const Concurrent &);
	virtual EC_word plterm() override;
};



template<class LhsT>
class Semantics<Assignment<LhsT>>
: public Semantics<AbstractLanguageElement>
, public AbstractSemantics<Assignment<LhsT>>
{
public:
	Semantics(const Assignment<LhsT> &ass)
	: AbstractSemantics<Assignment<LhsT>>(ass)
	{
		throw std::runtime_error(string("Assignment to ") + typeid(LhsT).name() + " is not implemented");
	}

	virtual EC_word plterm() override
	{ throw std::runtime_error(string("Assignment to ") + typeid(LhsT).name() + " is not implemented"); }
};



template<>
class Semantics<Assignment<Reference<Fluent>>>
: public Semantics<AbstractLanguageElement>
, public AbstractSemantics<Assignment<Reference<Fluent>>>
{
public:
	Semantics(const Assignment<Reference<Fluent>> &ass);

	virtual EC_word plterm() override;
};



template<>
class Semantics<Assignment<FieldAccess>>
: public Semantics<AbstractLanguageElement>
, public AbstractSemantics<Assignment<FieldAccess>>
{
public:
	Semantics(const Assignment<FieldAccess> &ass);

	virtual EC_word plterm() override;

private:
	const FieldAccess &field_access_;
};



template<>
class Semantics<Assignment<ListAccess>>
: public Semantics<AbstractLanguageElement>
, public AbstractSemantics<Assignment<ListAccess>>
{
public:
	Semantics(const Assignment<ListAccess> &ass);

	virtual EC_word plterm() override;

private:
	const ListAccess &field_access_;
};



template<>
class Semantics<Pick>
: public Semantics<AbstractLanguageElement>
, public AbstractSemantics<Pick>
{
public:
	Semantics(const Pick &pick);
	virtual EC_word plterm() override;
};



template<>
class Semantics<Search>
: public Semantics<AbstractLanguageElement>
, public AbstractSemantics<Search>
{
public:
	Semantics(const Search &);
	virtual EC_word plterm() override;
};



template<>
class Semantics<Solve>
: public Semantics<AbstractLanguageElement>
, public AbstractSemantics<Solve>
{
public:
	Semantics(const Solve &);
	virtual EC_word plterm() override;
};



template<>
class Semantics<Test>
: public Semantics<AbstractLanguageElement>
, public AbstractSemantics<Test>
{
public:
	Semantics(const Test &);
	virtual EC_word plterm() override;
};



template<>
class Semantics<While>
: public Semantics<AbstractLanguageElement>
, public AbstractSemantics<While>
{
public:
	Semantics(const While &);
	virtual EC_word plterm() override;
};



template<>
class Semantics<Return>
: public Semantics<AbstractLanguageElement>
, public AbstractSemantics<Return>
{
public:
	Semantics(const Return &r);
	virtual EC_word plterm() override;
};



template<>
class Semantics<DurativeCall>
: public Semantics<AbstractLanguageElement>
, public AbstractSemantics<DurativeCall>
{
public:
	Semantics(const DurativeCall &call);
	virtual EC_word plterm() override;
};


std::pair<const Reference<Fluent> *, EC_word>
traverse_mixed_field_access(const FieldAccess *fa, const ListAccess *la);


template<>
class Semantics<FieldAccess>
: public Semantics<Expression>
, public AbstractSemantics<FieldAccess>
{
public:
	Semantics(const FieldAccess &field_access);

	virtual EC_word plterm() override;
	EC_word field_assign(const Expression &value);
	EC_atom pl_field_name();
	void set_lvalue(bool lvalue);

private:
	bool is_lvalue_;
};



template<>
class Semantics<ListAccess>
: public Semantics<Expression>
, public AbstractSemantics<ListAccess>
{
public:
	Semantics(const ListAccess &list_access);

	virtual EC_word plterm() override;
	EC_word pl_index();
};



template<>
class Semantics<ListPop>
: public Semantics<AbstractLanguageElement>
, public AbstractSemantics<ListPop>
{
public:
	Semantics(const ListPop &list_access);

	virtual EC_word plterm() override;
};



template<>
class Semantics<ListPush>
: public Semantics<AbstractLanguageElement>
, public AbstractSemantics<ListPush>
{
public:
	Semantics(const ListPush &list_access);

	virtual EC_word plterm() override;
};



template<>
class Semantics<ListLength>
: public Semantics<Expression>
, public AbstractSemantics<ListLength>
{
public:
	Semantics(const ListLength &list_access);

	virtual EC_word plterm() override;
};



} // namespace gologpp


#endif // READYLOG_PROCEDURE_H_
