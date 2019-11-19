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

#ifndef READYLOG_REFERENCE_H_
#define READYLOG_REFERENCE_H_

#include <model/reference.h>

// Include headers for everything we might have a reference to (see plterm() method)
#include <model/action.h>
#include <model/fluent.h>
#include <model/procedural.h>

#include "semantics.h"
#include "utilities.h"

#include <eclipseclass.h>


namespace gologpp {

template<class GologT> class Reference;



template<>
class Semantics<Reference<Variable>>
: public Semantics<Expression>
, public AbstractSemantics<Reference<Variable>>
{
public:
	Semantics(const Reference<Variable> &ref);

	virtual EC_word plterm() override;
};



template<class GologT, class ExprT>
EC_word reference_term(const ReferenceBase<GologT, ExprT> &ref)
{
	if (ref.arity() > 0)
		return ::term(EC_functor(ref.name().c_str(), ref.arity()),
			to_ec_words(ref.args()).data()
		);
	else
		return EC_atom(ref.name().c_str());
}



template<class TargetT>
class Semantics<Reference<TargetT>>
: public Semantics<Expression>
, public AbstractSemantics<Reference<TargetT>>
{
public:
	using AbstractSemantics<Reference<TargetT>>::AbstractSemantics;

	virtual EC_word plterm() override
	{
		return reference_term(
			AbstractSemantics<Reference<TargetT>>::template element()
		);
	}
};


template<>
EC_word Semantics<Reference<Action>>::plterm();




} // namespace gologpp



#endif // READYLOG_REFERENCE_H_
