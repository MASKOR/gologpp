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

	AbstractSemantics<ModelElement> &abstract_semantics() const
	{ return *semantics_; }

	template<class GologT = ModelElement>
	Semantics<GologT> &semantics() const
	{ return dynamic_cast<Semantics<GologT> &>(*semantics_); }

	void set_semantics(std::unique_ptr<AbstractSemantics<ModelElement>> &&impl);
	virtual void attach_semantics(SemanticsFactory &) = 0;

	virtual string to_string(const string &pfx) const = 0;
	string str() const;

protected:
	std::unique_ptr<AbstractSemantics<ModelElement>> semantics_;
};



#define DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(...) \
	virtual void attach_semantics(SemanticsFactory &f) override { \
		if (!semantics_) { \
			semantics_ = f.make_semantics(*this); \
			boost::fusion::for_each(std::tie(__VA_ARGS__), [&] (auto &e) { \
				e.attach_semantics(f); \
			} ); \
		} \
	}

#define DEFINE_ATTACH_SEMANTICS \
	virtual void attach_semantics(SemanticsFactory &f) override { \
		if (!semantics_) \
			semantics_ = f.make_semantics(*this); \
	}





} // namespace gologpp
