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

#ifndef GOLOGPP_LANGUAGE_H_
#define GOLOGPP_LANGUAGE_H_

#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>

#include "gologpp.h"
#include <memory>
#include <type_traits>
#include "types.h"

namespace gologpp {


class AbstractLanguageElement {
private:
	// Used by the constructor of the first Scope (i.e. the global scope) because at that time
	// the type registry isn't initialized, yet. So we have to pass the type instead of looking it up.
	AbstractLanguageElement(shared_ptr<const UndefinedType>);
	friend class Scope;

public:
	AbstractLanguageElement();

	// By default, a language element is not copyable, since it is part of a graph
	// that may even contain cycles.
	AbstractLanguageElement(const AbstractLanguageElement &) = delete;
	AbstractLanguageElement(AbstractLanguageElement &&) = delete;
	AbstractLanguageElement &operator = (const AbstractLanguageElement &) = delete;
	AbstractLanguageElement &operator = (AbstractLanguageElement &&) = delete;

	virtual ~AbstractLanguageElement() = default;

	virtual AbstractSemantics<AbstractLanguageElement> &abstract_semantics() const
	{ return *semantics_; }

	template<class GologT = AbstractLanguageElement>
	Semantics<GologT> &semantics() const
	{ return dynamic_cast<Semantics<GologT> &>(*semantics_); }

	void set_semantics(unique_ptr<AbstractSemantics<AbstractLanguageElement>> &&impl);
	virtual void attach_semantics(SemanticsFactory &) = 0;

	virtual string to_string(const string &pfx) const = 0;
	string str() const;

	virtual Scope &scope() = 0;
	virtual const Scope &scope() const = 0;
	virtual Scope &parent_scope() = 0;
	virtual const Scope &parent_scope() const = 0;

	void set_type_by_name(const string &name);
	void set_type(const Type &t);
	virtual const Type &type() const;
	shared_ptr<const Type> type_ptr() const;

	// Unambiguous alias name to simplify type resolution for phoenix::bind in the parser
	Scope &m_scope();

	bool is_ref() const;

	template<class T>
	bool is_a() const
	{ return dynamic_cast<const T *>(this); }

	template<class T>
	void ensure_type();

	void ensure_type(const Type &t);


protected:
	void set_type_unchecked(const Type &t);

	template<class TypeT>
	void t_set_type_unchecked();

	unique_ptr<AbstractSemantics<AbstractLanguageElement>> semantics_;

private:
	shared_ptr<const Type> type_;
};




template<class GologT, class TypeT = UndefinedType>
class LanguageElement : public virtual AbstractLanguageElement {
public:
	typedef GologT golog_t;

	LanguageElement()
	{
		if (typeid(TypeT) != typeid(UndefinedType))
			t_set_type_unchecked<TypeT>();
	}

	virtual ~LanguageElement() = default;

	virtual AbstractSemantics<GologT> &abstract_semantics() const override {
		return dynamic_cast<AbstractSemantics<GologT> &>(*semantics_);
	}

	template<class = GologT>
	Semantics<GologT> &semantics() const
	{ return dynamic_cast<Semantics<GologT> &>(*semantics_); }
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

#endif // GOLOGPP_LANGUAGE_H_
