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

#ifndef READYLOG_FORMULA_H_
#define READYLOG_FORMULA_H_

#include "semantics.h"
#include <model/gologpp.h>
#include <model/formula.h>

#include <eclipseclass.h>

namespace gologpp {


template<>
class Semantics<Negation> : public Semantics<Expression> {
public:
	Semantics(const Negation &);
	virtual ~Semantics() override = default;

	virtual EC_word plterm() override;

private:
	const Negation &negation_;
};


template<>
class Semantics<Comparison> : public Semantics<Expression> {
public:
	Semantics(const Comparison &cmp);

	virtual ~Semantics() override = default;
	virtual EC_word plterm() override;

private:
	const Comparison &comparison_;
	const char *functor_;
};


template<>
class Semantics<BooleanOperation> : public Semantics<Expression> {
public:
	Semantics(const BooleanOperation &);
	virtual EC_word plterm() override;

private:
	const BooleanOperation &conjunction_;
	const char *functor_;
};


template<>
class Semantics<Quantification> : public Semantics<Expression> {
public:
	Semantics(const Quantification &);
	virtual EC_word plterm() override;

private:
	const Quantification &quantification_;
};



} // namespace gologpp


#endif
