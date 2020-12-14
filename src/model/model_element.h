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

#pragma once

#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>


#include "gologpp.h"

namespace gologpp {


class ModelElement {
public:
	using SignifierT = ModelElement;
	ModelElement() = default;

	// By default, a language element is not copyable, since it is part of a graph
	// that may even contain cycles.
	ModelElement(const ModelElement &) = delete;
	ModelElement(ModelElement &&) = delete;
	ModelElement &operator = (const ModelElement &) = delete;
	ModelElement &operator = (ModelElement &&) = delete;

	virtual ~ModelElement() = default;

	/// @return The general (i.e. implementation-agnostic) semantics of this model element
	template<class GologT = ModelElement>
	GeneralSemantics<GologT> &general_semantics() const
	{ return dynamic_cast<GeneralSemantics<GologT> &>(*semantics_); }

	/// @return The implementation-specific semantics of this model element.
	/// This method cannot be called (or even instantiated) from the code model context. It
	/// can only be instantiated and called from the semantics implementation.
	template<class GologT>
	Semantics<GologT> &semantics() const
	{ return dynamic_cast<Semantics<GologT> &>(*semantics_); }

	void set_semantics(std::unique_ptr<GeneralSemantics<ModelElement>> &&impl);
	virtual void attach_semantics(SemanticsFactory &) = 0;

	virtual string to_string(const string &pfx) const = 0;
	string str() const;

	template<class T>
	T &cast()
	{ return dynamic_cast<typename std::remove_reference<T>::type &>(*this); }

	template<class T>
	const T &cast() const
	{ return dynamic_cast<const T &>(*this); }

protected:
	std::unique_ptr<GeneralSemantics<ModelElement>> semantics_;
};



template<>
class GeneralSemantics<ModelElement> {
public:
	GeneralSemantics() = default;

	virtual ~GeneralSemantics();

	virtual AExecutionController &context() const = 0;

	/// @return A reference to the abstract model element covered by this semantics
	virtual const ModelElement &model_element() const = 0;

private:
	// Not moveable by default because of cross-referencing with language element
	GeneralSemantics(GeneralSemantics &&) = delete;
	GeneralSemantics & operator = (GeneralSemantics &&) = delete;
};




} // namespace gologpp
