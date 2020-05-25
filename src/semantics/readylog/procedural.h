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

#include <model/error.h>

#include <type_traits>

#include <eclipseclass.h>

namespace gologpp {



template<>
class Semantics<Function>
: public Semantics<AbstractLanguageElement>
, public AbstractSemantics<Function>
{
public:
	using AbstractSemantics<Function>::AbstractSemantics;
	virtual EC_word plterm() override;
	virtual EC_word definition();
	EC_word return_var();

private:
	EC_word return_var_;
};



template<>
class Semantics<Procedure>
: public Semantics<AbstractLanguageElement>
, public AbstractSemantics<Procedure>
{
public:
	using AbstractSemantics<Procedure>::AbstractSemantics;
	virtual EC_word plterm() override;
	virtual EC_word definition();
};



template<>
class Semantics<Block>
: public Semantics<Instruction>
, public AbstractSemantics<Block>
{
public:
	using AbstractSemantics<Block>::AbstractSemantics;

	virtual EC_word plterm() override;
	virtual Plan trans(const Binding &b, History &h) override;

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
	using AbstractSemantics<Choose>::AbstractSemantics;
	virtual EC_word plterm() override;
};



template<class SignT>
class Semantics<Conditional<SignT>>
: public Semantics<AbstractLanguageElement>
, public AbstractSemantics<Conditional<SignT>>
{
public:
	using AbstractSemantics<Conditional<SignT>>::AbstractSemantics;
	virtual EC_word plterm() override;
};



template<>
class Semantics<Concurrent>
: public Semantics<AbstractLanguageElement>
, public AbstractSemantics<Concurrent>
{
public:
	using AbstractSemantics<Concurrent>::AbstractSemantics;
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
	using AbstractSemantics<Assignment<Reference<Fluent>>>::AbstractSemantics;
	virtual EC_word plterm() override;
};



template<>
class Semantics<Assignment<FieldAccess>>
: public Semantics<AbstractLanguageElement>
, public AbstractSemantics<Assignment<FieldAccess>>
{
public:
	using AbstractSemantics<Assignment<FieldAccess>>::AbstractSemantics;
	virtual EC_word plterm() override;
};



template<>
class Semantics<Assignment<ListAccess>>
: public Semantics<AbstractLanguageElement>
, public AbstractSemantics<Assignment<ListAccess>>
{
public:
	using AbstractSemantics<Assignment<ListAccess>>::AbstractSemantics;
	virtual EC_word plterm() override;
};



template<>
class Semantics<Pick>
: public Semantics<AbstractLanguageElement>
, public AbstractSemantics<Pick>
{
public:
	Semantics(const Pick &);
	virtual EC_word plterm() override;
};



template<>
class Semantics<Search>
: public Semantics<AbstractLanguageElement>
, public AbstractSemantics<Search>
{
public:
	using AbstractSemantics<Search>::AbstractSemantics;
	virtual EC_word plterm() override;
};



template<>
class Semantics<Solve>
: public Semantics<AbstractLanguageElement>
, public AbstractSemantics<Solve>
{
public:
	using AbstractSemantics<Solve>::AbstractSemantics;
	virtual EC_word plterm() override;
};



template<>
class Semantics<Test>
: public Semantics<AbstractLanguageElement>
, public AbstractSemantics<Test>
{
public:
	using AbstractSemantics<Test>::AbstractSemantics;
	virtual EC_word plterm() override;
};



template<>
class Semantics<While>
: public Semantics<AbstractLanguageElement>
, public AbstractSemantics<While>
{
public:
	using AbstractSemantics<While>::AbstractSemantics;
	virtual EC_word plterm() override;
};



template<>
class Semantics<DurativeCall>
: public Semantics<AbstractLanguageElement>
, public AbstractSemantics<DurativeCall>
{
public:
	using AbstractSemantics<DurativeCall>::AbstractSemantics;
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
	using AbstractSemantics<FieldAccess>::AbstractSemantics;

	virtual EC_word plterm() override;
	EC_word field_assign(const Expression &value);
	EC_atom pl_field_name();
};



template<>
class Semantics<ListAccess>
: public Semantics<Expression>
, public AbstractSemantics<ListAccess>
{
public:
	using AbstractSemantics<ListAccess>::AbstractSemantics;

	virtual EC_word plterm() override;
	EC_word pl_index();
};



template<>
class Semantics<ListPop>
: public Semantics<AbstractLanguageElement>
, public AbstractSemantics<ListPop>
{
public:
	using AbstractSemantics<ListPop>::AbstractSemantics;
	virtual EC_word plterm() override;
};



template<>
class Semantics<ListPush>
: public Semantics<AbstractLanguageElement>
, public AbstractSemantics<ListPush>
{
public:
	using AbstractSemantics<ListPush>::AbstractSemantics;
	virtual EC_word plterm() override;
};



template<>
class Semantics<ListLength>
: public Semantics<Expression>
, public AbstractSemantics<ListLength>
{
public:
	using AbstractSemantics<ListLength>::AbstractSemantics;
	virtual EC_word plterm() override;
};



template<>
class Semantics<During>
: public Semantics<AbstractLanguageElement>
, public AbstractSemantics<During>
{
public:
	using AbstractSemantics<During>::AbstractSemantics;
	virtual EC_word plterm() override;
};


} // namespace gologpp


#endif // READYLOG_PROCEDURE_H_
