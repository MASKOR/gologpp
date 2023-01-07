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

#include <memory>
#include <type_traits>

#include <boost/preprocessor/seq/for_each.hpp>

namespace gologpp {

/**
 * @brief Partially specialize this to true for certain templated GologTs (i.e. Reference<T>, EffectAxiom<T>...)
 * if you have generic definitions for Semantics<GologT, Cond>, but also want to supply certain
 * partial specializations.
 * Use it as a condition in the enable_if that gives Cond to prevent the ambiguous partial specialization
 * errors that would otherwise occur.
 */
template<class GologT>
static constexpr bool partially_specialized = false;

template<class GologT>
static constexpr bool is_instruction = std::is_base_of<Instruction, GologT>::value;

template<class GologT>
static constexpr bool is_expression = std::is_base_of<Expression, GologT>::value;


/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

template<class GologT>
class GeneralSemantics<GologT, enable_if<is_instruction<GologT>>>
: public virtual GeneralSemantics<Instruction>
{
public:
	GeneralSemantics(const GologT &elem, AExecutionController &context)
	: element_(&elem)
	, context_(context)
	{}

	virtual ~GeneralSemantics() = default;

	/// @return A reference to the concrete language element (code model element) covered by this
	/// semantics instance
	template<class = GologT>
	const GologT &element() const
	{ return *element_; }

	virtual AExecutionController &context() const override
	{ return context_; }

	void update_element(const GologT *new_element)
	{ element_ = new_element; }

	virtual const ModelElement &model_element() const override
	{ return *element_; }

	virtual const Instruction &instruction() const override
	{ return *element_; }

private:
	const GologT *element_;
	AExecutionController &context_;
};



template<class GologT>
class GeneralSemantics<GologT, enable_if<is_expression<GologT>>>
: public virtual GeneralSemantics<Expression>
{
public:
	GeneralSemantics(const GologT &elem, AExecutionController &context)
	: element_(&elem)
	, context_(context)
	{}

	virtual ~GeneralSemantics() = default;

	/// @return A reference to the concrete language element (code model element) covered by this
	/// semantics instance
	template<class = GologT>
	const GologT &element() const
	{ return *element_; }

	virtual AExecutionController &context() const override
	{ return context_; }

	void update_element(const GologT *new_element)
	{ element_ = new_element; }

	virtual const ModelElement &model_element() const override
	{ return *element_; }

	virtual const Expression &expression() const override
	{ return *element_; }

private:
	const GologT *element_;
	AExecutionController &context_;
};



template<class GologT>
class GeneralSemantics<GologT, enable_if <
	!is_expression<GologT>
	&& !is_instruction<GologT>
	&& !partially_specialized<GologT>
> >
: public virtual GeneralSemantics<ModelElement>
{
public:
	GeneralSemantics(const GologT &elem, AExecutionController &context)
	: element_(&elem)
	, context_(context)
	{}

	virtual ~GeneralSemantics() = default;

	/// @return A reference to the concrete language element (code model element) covered by this
	/// semantics instance
	template<class = GologT>
	const GologT &element() const
	{ return *element_; }

	virtual AExecutionController &context() const override
	{ return context_; }

	void update_element(const GologT *new_element)
	{ element_ = new_element; }

	virtual const ModelElement &model_element() const override
	{ return *element_; }

private:
	const GologT *element_;
	AExecutionController &context_;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

/// declare pure virtual make_semantics(...)
#define GOLOGPP_DECLARE_ABSTRACT_MAKE_SEMANTICS(r, data, T) \
	virtual unique_ptr<GeneralSemantics<ModelElement>> make_semantics(T &) = 0;

#define GOLOGPP_DECLARE_MAKE_SEMANTICS(_r, _data, T) \
	virtual unique_ptr<GeneralSemantics<ModelElement>> make_semantics(T &);

#define GOLOGPP_DECL_MAKE_SEMANTICS_OVERRIDE(_r, _data, GologT) \
	virtual unique_ptr<GeneralSemantics<ModelElement>> make_semantics(GologT &) override;

#define GOLOGPP_DEFINE_MAKE_SEMANTICS(_r, FactoryClass, GologT) \
	unique_ptr<GeneralSemantics<ModelElement>> FactoryClass::make_semantics(GologT &obj) \
	{ return unique_ptr<GeneralSemantics<ModelElement>>(new Semantics<GologT>(obj, context())); }

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

class SemanticsFactory {
public:
	SemanticsFactory(unique_ptr<platform::SemanticsFactory> &&psf);

	virtual ~SemanticsFactory() = default;

	virtual AExecutionController &context();
	void set_context(AExecutionController &);

	platform::SemanticsFactory &platform_semantics_factory();
	static SemanticsFactory &get();

	GOLOGPP_DECLARE_MAKE_SEMANTICS((), (), Reference<platform::SwitchStateAction>)

	BOOST_PP_SEQ_FOR_EACH(GOLOGPP_DECLARE_ABSTRACT_MAKE_SEMANTICS, (), GOLOGPP_SEMANTIC_TYPES)
	BOOST_PP_SEQ_FOR_EACH(GOLOGPP_DECLARE_MAKE_SEMANTICS, (), GOLOGPP_PLATFORM_ELEMENTS)

private:
	static unique_ptr<SemanticsFactory> construct();

	unique_ptr<platform::SemanticsFactory> platform_semantics_factory_;
	AExecutionController *context_;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

#define DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(...) \
	virtual void attach_semantics(::gologpp::SemanticsFactory &f) override { \
		if (!this->semantics_) { \
			this->semantics_ = f.make_semantics(*this); \
			boost::fusion::for_each(std::tie(__VA_ARGS__), [&] (auto &e) { \
				e.attach_semantics(f); \
			} ); \
		} \
	}

#define DEFINE_ATTACH_SEMANTICS \
	virtual void attach_semantics(::gologpp::SemanticsFactory &f) override { \
		if (!this->semantics_) \
			this->semantics_ = f.make_semantics(*this); \
	}



} // namespace gologpp

#endif // GOLOGPP_IMPLEMENTATION_H_
