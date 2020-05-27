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
#include <model/model_element.h>

#include <boost/preprocessor/seq/for_each.hpp>

namespace gologpp {


template<class TSub, class TBas, class TArg>
using enable_if_subclass = std::enable_if <
	std::is_base_of<TBas, TSub>::value,
	TArg
>;


template<>
class Semantics<ModelElement>
: public virtual GeneralSemantics<ModelElement>
{
public:
	virtual ~Semantics<ModelElement>() override = default;
};


template<>
class Semantics<Expression>
: public GeneralSemantics<Expression>
, public Semantics<ModelElement>
{
public:
	virtual Value evaluate(const Binding &context, const History &h) override;
	virtual const Expression &expression() const override;
};


template<>
class Semantics<Instruction>
: public GeneralSemantics<Instruction>
, public Semantics<ModelElement>
{
public:
	virtual unique_ptr<Plan> plan(const Binding &b, History &h);
	virtual bool final(const Binding &b, const History &h) override;
	virtual const Instruction &instruction() const override;
};


template<class GologT>
class Semantics
: public Semantics<typename GologT::ElementType>
{
public:
	typename  std::enable_if < std::is_base_of<GologT, Expression>::value,
		Value
	>::type evaluate(const Binding &b, const History &h) override;

	typename enable_if_subclass< GologT, Instruction,
		unique_ptr<Plan>
	>::type trans(const Binding &b, History &h) override;

	const GologT &element() const;
};



using EvaluationSemantics = Semantics<Expression>;
using TransitionSemantics = Semantics<Instruction>;

#define GOLOGPP_DECL_MAKE_SEMANTICS_OVERRIDE(_r, _data, GologT) \
	virtual unique_ptr<GeneralSemantics<ModelElement>> make_semantics(GologT &) override;


class NativeSemanticsFactory : public SemanticsFactory {
public:
	NativeSemanticsFactory(unique_ptr<platform::SemanticsFactory> &&psf);
	virtual ~NativeSemanticsFactory() override = default;

	BOOST_PP_SEQ_FOR_EACH(GOLOGPP_DECL_MAKE_SEMANTICS_OVERRIDE, (), GOLOGPP_SEMANTIC_TYPES)
};



} // namespace gologpp



#endif
