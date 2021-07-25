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

#include "wrap_eclipseclass.h"

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

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

template<>
class Semantics<ModelElement>
: public virtual GeneralSemantics<ModelElement>
{
public:
	virtual ~Semantics() override = default;
	virtual EC_word plterm() = 0;

	ReadylogContext &rl_context() const;
};


template<>
class Semantics<Expression>
: public virtual GeneralSemantics<Expression>
, public Semantics<ModelElement>
{
public:
	using GeneralSemantics<Expression>::GeneralSemantics;

	virtual ~Semantics() override = default;
	virtual Value evaluate(const BindingChain &b, const History &h) override;
};



template<>
class Semantics<Instruction>
: public virtual GeneralSemantics<Instruction>
, public Semantics<ModelElement>
{
public:
	using GeneralSemantics<Instruction>::GeneralSemantics;

	virtual ~Semantics() override = default;
	virtual unique_ptr<Plan> trans(const BindingChain &b, History &h) override;
	virtual bool final(const BindingChain &b, const History &h) override;

	EC_word next_readylog_term();

protected:
	ManagedTerm next_readylog_term_;
};


template<class GologT>
class Semantics<GologT, enable_if <
	!partially_specialized<GologT>
	&& is_expression<GologT>
> >
: public GeneralSemantics<GologT>
, public Semantics<Expression>
{
public:
	using GeneralSemantics<GologT>::GeneralSemantics;
	virtual ~Semantics() override = default;

	virtual EC_word plterm() override;

	ReadylogContext &rl_context() const
	{ return dynamic_cast<ReadylogContext &>(GeneralSemantics<GologT>::context()); }
};


template<class GologT>
class Semantics<GologT, enable_if <
	!partially_specialized<GologT>
	&& is_instruction<GologT>
> >
: public GeneralSemantics<GologT>
, public Semantics<Instruction>
{
public:
	using GeneralSemantics<GologT>::GeneralSemantics;
	virtual ~Semantics() override = default;

	virtual EC_word plterm() override;

	ReadylogContext &rl_context() const
	{ return dynamic_cast<ReadylogContext &>(GeneralSemantics<GologT>::context()); }
};



template<class GologT>
class Semantics<GologT, enable_if <
	!partially_specialized<GologT>
	&& !is_expression<GologT>
	&& !is_instruction<GologT>
> >
: public GeneralSemantics<GologT>
, public Semantics<ModelElement>
{
public:
	using GeneralSemantics<GologT>::GeneralSemantics;
	virtual ~Semantics() override = default;

	virtual EC_word plterm() override;

	ReadylogContext &rl_context() const
	{ return dynamic_cast<ReadylogContext &>(GeneralSemantics<GologT>::context()); }
};




} // namespace gologpp



#endif
