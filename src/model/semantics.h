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


template<>
class AbstractSemantics<AbstractLanguageElement> {
public:
	AbstractSemantics();

	virtual ~AbstractSemantics<AbstractLanguageElement>();

	/// @return a reference to the model element that this semantics refers to
	//virtual	const AbstractLanguageElement &element() const = 0;

private:
	// Not trivially moveable because cross-referencing with language element
	AbstractSemantics(AbstractSemantics &&) = delete;
	AbstractSemantics & operator = (AbstractSemantics &&) = delete;
};



template<>
class AbstractSemantics<Expression>
: public virtual AbstractSemantics<AbstractLanguageElement>
{
public:
	virtual Value evaluate(const Binding &b, const History &h) = 0;
};



template<>
class AbstractSemantics<Instruction>
: public virtual AbstractSemantics<AbstractLanguageElement>
{
public:
	virtual Plan trans(const Binding &b, History &h) = 0;
};



template<class GologT>
class AbstractSemantics
: public virtual AbstractSemantics<AbstractLanguageElement>
{
public:
	AbstractSemantics(const GologT &elem)
	: element_(&elem)
	{}

	virtual ~AbstractSemantics<GologT>() = default;

	const GologT &element() const
	{ return *element_; }

	void update_element(const GologT *new_element)
	{ element_ = new_element; }

private:
	const GologT *element_;
};



/*template<>
class AbstractSemantics<Value>
: public AbstractSemantics<Expression>
{
public:
	friend Value;

	AbstractSemantics(const Value &elem)
	: element_(&elem)
	{}

	virtual ~AbstractSemantics<Value>();

	const Value &element() const;

private:
	const Value *element_;
};//*/



#define GOLOGPP_DECLARE_ABSTRACT_MAKE_SEMANTICS(r, data, T) \
	virtual unique_ptr<AbstractSemantics<AbstractLanguageElement>> make_semantics(T &) = 0;

class SemanticsFactory {
public:
	virtual ~SemanticsFactory() = default;

	BOOST_PP_SEQ_FOR_EACH(GOLOGPP_DECLARE_ABSTRACT_MAKE_SEMANTICS, (), GOLOGPP_SEMANTIC_TYPES)
};



}

#endif // GOLOGPP_IMPLEMENTATION_H_
