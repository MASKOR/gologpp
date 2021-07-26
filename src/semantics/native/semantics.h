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


class Escalate {
public:
	Escalate(const Instruction *static_cause, shared_ptr<const Instruction> dynamic_cause);

	void set_static_cause(const Instruction *cause);
	const Instruction *static_cause() const;
	void set_dynamic_cause(shared_ptr<const Instruction> cause);
	shared_ptr<const Instruction> dynamic_cause() const;

private:
	const Instruction *static_cause_;
	shared_ptr<const Instruction> dynamic_cause_;
};



template<class GologT>
class Semantics<GologT, enable_if<is_instruction<GologT>>>
: public GeneralSemantics<GologT>
{
public:
	using GeneralSemantics<GologT>::GeneralSemantics;
	virtual ~Semantics() override = default;

	virtual unique_ptr<Plan> trans(const BindingChain &b, History &h) override;

	virtual bool final(const BindingChain &, const History &) override
	{ throw Bug("This method should not be called"); }
};



template<class GologT>
class Semantics<GologT, enable_if<is_expression<GologT>>>
: public GeneralSemantics<GologT>
{
public:
	using GeneralSemantics<GologT>::GeneralSemantics;
	virtual ~Semantics() override = default;

	virtual Value evaluate(const BindingChain &b, const History &h) override;
};


template<class GologT>
class Semantics<GologT, enable_if <
	!is_expression<GologT>
	&& !is_instruction<GologT>
>>
: public GeneralSemantics<GologT>
{
public:
	using GeneralSemantics<GologT>::GeneralSemantics;
	virtual ~Semantics() override = default;
};


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
