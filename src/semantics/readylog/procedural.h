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
: public Semantics<ModelElement>
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
: public Semantics<ModelElement>
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



template<class LhsT>
class Semantics<Assignment<LhsT>>
: public Semantics<ModelElement>
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
class Semantics<Pick>
: public Semantics<ModelElement>
, public AbstractSemantics<Pick>
{
public:
	Semantics(const Pick &);
	virtual EC_word plterm() override;
};



template<>
class Semantics<Test>
: public Semantics<Instruction>
, public AbstractSemantics<Test>
{
public:
	using AbstractSemantics<Test>::AbstractSemantics;
	virtual EC_word plterm() override;

	void make_plan_marker(EC_word cond);

private:
	bool is_plan_marker_ = false;
	ManagedTerm marker_cond_;
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



} // namespace gologpp


#endif // READYLOG_PROCEDURE_H_
