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

#ifndef READYLOG_IMPLEMENTATION_H_
#define READYLOG_IMPLEMENTATION_H_

#include <model/semantics.h>

#include <eclipseclass.h>

#include <boost/preprocessor/seq/for_each.hpp>

namespace gologpp {


template<>
class Semantics<AbstractLanguageElement> : public AbstractSemantics {
public:
	virtual ~Semantics() override = default;
	virtual EC_word plterm() = 0;
};


using ReadylogSemantics = Semantics<AbstractLanguageElement>;


#define GOLOGPP_DECL_MAKE_SEMANTICS_OVERRIDE(_r, _data, GologT) \
	virtual unique_ptr<AbstractSemantics> make_semantics(GologT &) override;


class ReadylogSemanticsFactory : public SemanticsFactory {
public:
	ReadylogSemanticsFactory() = default;
	virtual ~ReadylogSemanticsFactory() override = default;

	BOOST_PP_SEQ_FOR_EACH(GOLOGPP_DECL_MAKE_SEMANTICS_OVERRIDE, (), GOLOGPP_SEMANTIC_TYPES)
};


}

#endif
