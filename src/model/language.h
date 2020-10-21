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

#include "model_element.h"
#include <memory>
#include "types.h"

namespace gologpp {


class AbstractLanguageElement : public virtual ModelElement {
private:
	// Used by the constructor of the first Scope (i.e. the global scope) because at that time
	// the type registry isn't initialized, yet. So we have to pass the type instead of looking it up.
	AbstractLanguageElement(shared_ptr<const UndefinedType>);
	friend class Scope;

public:
	AbstractLanguageElement();

	virtual Scope &scope() = 0;
	virtual const Scope &scope() const = 0;
	virtual Scope &parent_scope() = 0;
	virtual const Scope &parent_scope() const = 0;

	// Unambiguous alias name to simplify type resolution for phoenix::bind in the parser
	Scope &m_scope();

	bool is_ref() const;

	template<class T>
	bool is_a() const
	{ return dynamic_cast<const T *>(this); }

	template<class T>
	void ensure_type();

	void ensure_type(const Type &t);

	void set_type_by_name(const string &name);
	void set_type(const Type &t);
	virtual const Type &type() const;
	shared_ptr<const Type> type_ptr() const;

protected:
	void set_type_unchecked(const Type &t);

	template<class TypeT>
	void t_set_type_unchecked();

private:
	shared_ptr<const Type> type_;
};



class ChildElement
: public virtual AbstractLanguageElement
{
public:
	ChildElement();
	ChildElement(ChildElement &&) = default;

	AbstractLanguageElement *parent();
	const AbstractLanguageElement *parent() const;

	void set_parent(AbstractLanguageElement *parent);

	virtual Scope &parent_scope() override;
	virtual const Scope &parent_scope() const override;

private:
	AbstractLanguageElement *parent_;
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

	Semantics<GologT> &special_semantics() const
	{ return dynamic_cast<Semantics<GologT> &>(*semantics_); }

};



} // namespace gologpp

#endif // GOLOGPP_LANGUAGE_H_
