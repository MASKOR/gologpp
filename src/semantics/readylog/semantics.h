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


class ReadylogContext;


template<>
class Semantics<ModelElement>
: public virtual AbstractSemantics<ModelElement> {
public:
	virtual ~Semantics<ModelElement>() override = default;

	virtual EC_word plterm() = 0;
};



template<>
class Semantics<Expression>
: public AbstractSemantics<Expression>
, public Semantics<ModelElement>
{
public:
	virtual ~Semantics<Expression>() override = default;
	virtual Value evaluate(const Binding &b, const History &h) override;
};


template<>
class Semantics<Instruction>
: public AbstractSemantics<Instruction>
, public Semantics<ModelElement>
{
public:
	virtual ~Semantics<Instruction>() override = default;
	virtual Plan trans(const Binding &b, History &h) override;
};


template<>
class Semantics<Type>
: public virtual AbstractSemantics<ModelElement>
, public Semantics<ModelElement>
{
};


template<class GologT>
class Semantics
: public Semantics<typename GologT::ElementType>
, public AbstractSemantics<GologT>
{
public:
	using AbstractSemantics<GologT>::AbstractSemantics;
	virtual EC_word plterm() override;
};


#define GOLOGPP_DECL_MAKE_SEMANTICS_OVERRIDE(_r, _data, GologT) \
	virtual unique_ptr<AbstractSemantics<ModelElement>> make_semantics(GologT &) override;


class ReadylogSemanticsFactory : public SemanticsFactory {
public:
	ReadylogSemanticsFactory(ReadylogContext &context);
	virtual ~ReadylogSemanticsFactory() override = default;

	BOOST_PP_SEQ_FOR_EACH(GOLOGPP_DECL_MAKE_SEMANTICS_OVERRIDE, (), GOLOGPP_SEMANTIC_TYPES)

private:
	ReadylogContext &context_;
};



} // namespace gologpp



#endif
