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

#ifndef GOLOGPP_IMPLEMENTATION_H_
#define GOLOGPP_IMPLEMENTATION_H_

#include "gologpp.h"
#include <memory>

#include <boost/preprocessor/seq/for_each.hpp>

namespace gologpp {


template<>
class AbstractSemantics<AbstractLanguageElement> {
public:
	virtual ~AbstractSemantics<AbstractLanguageElement>() = default;

};


template<class GologT>
class AbstractSemantics : public virtual AbstractSemantics<AbstractLanguageElement> {
public:
	virtual ~AbstractSemantics<GologT>() = default;
};


template<>
class AbstractSemantics<Expression> : public virtual AbstractSemantics<AbstractLanguageElement> {
public:
	virtual Value evaluate(const AbstractGrounding &context, const History &h) = 0;
};


#define GOLOGPP_DECLARE_ABSTRACT_MAKE_SEMANTICS(r, data, T) \
	virtual unique_ptr<AbstractSemantics<AbstractLanguageElement>> make_semantics(T &) = 0;

class SemanticsFactory {
public:
	virtual ~SemanticsFactory() = default;

	BOOST_PP_SEQ_FOR_EACH(GOLOGPP_DECLARE_ABSTRACT_MAKE_SEMANTICS, (), GOLOGPP_SEMANTIC_TYPES)
};



}

#endif // GOLOGPP_IMPLEMENTATION_H_
