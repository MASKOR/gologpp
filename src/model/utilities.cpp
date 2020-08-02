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

#include "utilities.h"
#include "logger.h"
#include "value.h"

namespace gologpp {


Identifier::Identifier(const string &name, arity_t arity)
: Name(name)
, arity_(arity)
{}

Identifier::Identifier(Identifier &&other)
: Name(std::move(other))
, arity_(std::move(other.arity()))
{}

arity_t Identifier::arity() const
{ return arity_; }

void Identifier::set_arity(arity_t a)
{ arity_ = a; }

string Identifier::signature_str() const
{ return name() + "/" + std::to_string(arity()); }


vector<unique_ptr<Expression>> copy(const vector<unique_ptr<Expression>> &v)
{
	vector<unique_ptr<Expression>> rv;
	for (const unique_ptr<Expression> &e : v)
		rv.emplace_back(dynamic_cast<const Value &>(*e).copy());
	return rv;
}



#ifdef DEBUG_PARSER
const string indent("");
#else
const string indent("  ");
#endif


} // namespace gologpp
