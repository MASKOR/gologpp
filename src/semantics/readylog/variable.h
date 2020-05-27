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

#ifndef READYLOG_VARIABLE_H_
#define READYLOG_VARIABLE_H_

#include "semantics.h"

#include <model/semantics.h>
#include <model/variable.h>
#include <model/error.h>

#include <eclipseclass.h>


namespace gologpp {



template<>
class Semantics<Variable>
: public Semantics<Expression>
, public AbstractSemantics<Variable>
{
public:
	Semantics(const Variable &var, ReadylogContext &context);
	virtual ~Semantics() override;

	void init();
	virtual EC_word plterm() override;
	void translate_as_golog_var(bool as_gv);
	EC_word member_restriction();

private:
	EC_word ec_var_;
	EC_atom golog_var_;
	bool as_golog_var_;
};



/**
 * Scope guard that changes a variable implementation to render itself as a
 * Golog variable instead of a Prolog variable
 */
class GologVarMutator {
public:
	GologVarMutator(Semantics<Variable> &var_impl);
	GologVarMutator(const Reference<Variable> &var_ref);
	~GologVarMutator();

private:
	Semantics<Variable> &var_impl_;
};



} // namespace gologpp

#endif // READYLOG_VARIABLE_H_
