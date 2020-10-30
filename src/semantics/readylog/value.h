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

#ifndef READYLOG_CONSTANT_H_
#define READYLOG_CONSTANT_H_

#include <model/semantics.h>
#include <model/variable.h>
#include <model/error.h>

#include "semantics.h"

#include <eclipseclass.h>


namespace gologpp {


template<>
class Semantics<Value>
: public Semantics<Expression>
, public GeneralSemantics<Value>
{
public:
	Semantics(const Value &value, ReadylogContext &context);

	using GeneralSemantics<Value>::evaluate;
	using Semantics<Expression>::expression;

	virtual ~Semantics() override = default;

	virtual const Expression &expression() const override;
	virtual Value evaluate(const BindingChain &b, const History &h) override;

	virtual EC_word plterm() override;
	virtual Semantics<Value> *copy(const Value &target_elem) const override;
};


Value pl_term_to_value(EC_word term, bool allow_ec_resume = true);


} // namespace gologpp

#endif // READYLOG_CONSTANT_H_
