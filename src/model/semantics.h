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

#include "model_element.h"
#include "gologpp.h"
#include "expressions.h"
#include "plan.h"
#include <memory>

#include <boost/preprocessor/seq/for_each.hpp>

namespace gologpp {



template<class GologT>
class GeneralSemantics
: public virtual GeneralSemantics<ModelElement>
{
public:
	GeneralSemantics(const GologT &elem, ExecutionContext &context)
	: element_(&elem)
	, context_(context)
	{}

	virtual ~GeneralSemantics<GologT>() = default;


	template<class = GologT>
	const GologT &element() const
	{ return *element_; }

	virtual ExecutionContext &context() const override
	{ return context_; }

	void update_element(const GologT *new_element)
	{ element_ = new_element; }

	virtual const ModelElement &model_element() const override
	{ return *element_; }

private:
	const GologT *element_;
	ExecutionContext &context_;
};



#define GOLOGPP_DECLARE_ABSTRACT_MAKE_SEMANTICS(r, data, T) \
	virtual unique_ptr<GeneralSemantics<ModelElement>> make_semantics(T &) = 0;

class SemanticsFactory {
public:
	virtual ~SemanticsFactory() = default;

	BOOST_PP_SEQ_FOR_EACH(GOLOGPP_DECLARE_ABSTRACT_MAKE_SEMANTICS, (), GOLOGPP_SEMANTIC_TYPES)
};



}

#endif // GOLOGPP_IMPLEMENTATION_H_
