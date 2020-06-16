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

#include "history.h"

namespace gologpp {



AbstractSemantics<History>::AbstractSemantics(History &h, ExecutionContext &context)
: history_(h)
, context_(context)
{}

AbstractSemantics<History>::~AbstractSemantics<History>()
{}


History::History(History &&other)
{
	semantics_ = std::move(other.semantics_);
}

Scope &History::parent_scope()
{ return global_scope(); }

const Scope &History::parent_scope() const
{ return global_scope(); }


string History::to_string(const string &pfx) const
{ return pfx + linesep + "history: no representation" + linesep; }

const History &gologpp::AbstractSemantics<History>::element() const
{ return history_; }

ExecutionContext &AbstractSemantics<History>::context() const
{ return context_; }

const ModelElement &AbstractSemantics<History>::model_element() const
{ return history_; }

void AbstractSemantics<History>::append(shared_ptr<Grounding<AbstractAction>> a)
{ append(*a); }



} // namespace gologpp
