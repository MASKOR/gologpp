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
#include <model/platform/semantics.h>

#include <eclipseclass.h>

#include <boost/preprocessor/seq/for_each.hpp>

#include "execution.h"
#include "utilities.h"

namespace gologpp {


class ReadylogContext;


class ReadylogSemanticsFactory : public SemanticsFactory {
public:
	ReadylogSemanticsFactory(unique_ptr<platform::SemanticsFactory> &&);
	virtual ~ReadylogSemanticsFactory() override = default;

	virtual ReadylogContext &context() override;

	BOOST_PP_SEQ_FOR_EACH(GOLOGPP_DECL_MAKE_SEMANTICS_OVERRIDE, (), GOLOGPP_SEMANTIC_TYPES)
};



class ReadylogSemantics
: public virtual GeneralSemantics<ModelElement>
{
public:
	using GeneralSemantics<ModelElement>::GeneralSemantics;

	ReadylogContext &rl_context() const;

	virtual EC_word plterm() = 0;
};


template<>
class Semantics<Expression>
: public virtual GeneralSemantics<Expression>
, public ReadylogSemantics
{
public:
	using GeneralSemantics<Expression>::GeneralSemantics;

	virtual ~Semantics<Expression>() override = default;
	virtual Value evaluate(const BindingChain &b, const History &h) override;
	virtual const Expression &expression() const override;
};



template<>
class Semantics<Instruction>
: public virtual GeneralSemantics<Instruction>
, public ReadylogSemantics
{
public:
	using GeneralSemantics<Instruction>::GeneralSemantics;

	virtual ~Semantics<Instruction>() override = default;
	virtual unique_ptr<Plan> trans(const BindingChain &b, History &h) override;
	virtual bool final(const BindingChain &b, const History &h) override;

	EC_word next_readylog_term();
	virtual const Instruction &instruction() const override;

protected:
	ManagedTerm next_readylog_term_;
};



template<>
class Semantics<Type>
: public virtual GeneralSemantics<ModelElement>
{
};



template<class GologT>
class Semantics
: public Semantics<typename GologT::ElementType>
, public GeneralSemantics<GologT>
{
public:
	using GeneralSemantics<GologT>::GeneralSemantics;
	virtual EC_word plterm() override;

	virtual const GologT &model_element() const override
	{ return GeneralSemantics<GologT>::element(); }
};




} // namespace gologpp



#endif
