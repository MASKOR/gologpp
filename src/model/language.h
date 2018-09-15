#ifndef GOLOGPP_LANGUAGE_H_
#define GOLOGPP_LANGUAGE_H_

#include "gologpp.h"
#include <memory>
#include "implementation.h"

#include <boost/fusion/include/std_tuple.hpp>
#include <boost/fusion/include/for_each.hpp>

namespace gologpp {


class AbstractLanguageElement {
public:
	AbstractLanguageElement() = default;

	// No language element is ever copyable, since it is part of a graph
	// that may even contain cycles.
	AbstractLanguageElement(const AbstractLanguageElement &) = delete;
	AbstractLanguageElement(AbstractLanguageElement &&) = default;
	AbstractLanguageElement &operator = (const AbstractLanguageElement &) = delete;
	//AbstractLanguageElement &operator = (AbstractLanguageElement &&) = default;

	virtual ~AbstractLanguageElement() = default;

	template<class GologT = AbstractLanguageElement>
	Implementation<GologT> &implementation() const
	{ return static_cast<Implementation<GologT> &>(*impl_); }

	void set_implementation(unique_ptr<AbstractImplementation> &&impl)
	{ impl_ = std::move(impl); }

	virtual void implement(Implementor &implementor) = 0;
	virtual string to_string(const string &pfx) const = 0;
	virtual Scope &scope() = 0;
	virtual const Scope &scope() const = 0;
	virtual Scope &parent_scope() = 0;
	virtual const Scope &parent_scope() const = 0;

	// Unambiguous alias name to simplify type resolution for phoenix::bind in the parser
	Scope &m_scope()
	{ return scope(); }

	string str() const
	{ return to_string(""); }

protected:
	unique_ptr<AbstractImplementation> impl_;
};


template<class GologT>
class LanguageElement : public virtual AbstractLanguageElement {
public:
	typedef GologT golog_t;

	LanguageElement() = default;
	virtual ~LanguageElement() = default;

	LanguageElement(LanguageElement &&other) = default;
	LanguageElement(const LanguageElement &) = delete;
	//LanguageElement &operator = (LanguageElement &&) = default;
	LanguageElement &operator = (const LanguageElement &) = delete;

	template<class = GologT>
	Implementation<GologT> &implementation() const
	{ return static_cast<Implementation<GologT> &>(*impl_); }
};



} // namespace gologpp

#endif // GOLOGPP_LANGUAGE_H_
