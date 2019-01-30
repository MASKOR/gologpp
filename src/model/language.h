#ifndef GOLOGPP_LANGUAGE_H_
#define GOLOGPP_LANGUAGE_H_

#include "gologpp.h"
#include <memory>
#include "semantics.h"

namespace gologpp {


class AbstractLanguageElement {
public:
	AbstractLanguageElement() = default;

	// By default, a language element is not copyable, since it is part of a graph
	// that may even contain cycles.
	AbstractLanguageElement(const AbstractLanguageElement &) = delete;
	AbstractLanguageElement(AbstractLanguageElement &&) = default;
	AbstractLanguageElement &operator = (const AbstractLanguageElement &) = delete;
	//AbstractLanguageElement &operator = (AbstractLanguageElement &&) = default;

	virtual ~AbstractLanguageElement() = default;

	template<class GologT = AbstractLanguageElement>
	Semantics<GologT> &semantics() const
	{ return static_cast<Semantics<GologT> &>(*semantics_); }

	void set_implementation(unique_ptr<AbstractSemantics> &&impl);
	virtual void attach_semantics(SemanticsFactory &implementor) = 0;

	virtual string to_string(const string &pfx) const = 0;
	string str() const;

	virtual Scope &scope() = 0;
	virtual const Scope &scope() const = 0;
	virtual Scope &parent_scope() = 0;
	virtual const Scope &parent_scope() const = 0;

	// Unambiguous alias name to simplify type resolution for phoenix::bind in the parser
	Scope &m_scope();

	bool is_ref() const;

protected:
	unique_ptr<AbstractSemantics> semantics_;
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
	Semantics<GologT> &semantics() const
	{ return static_cast<Semantics<GologT> &>(*semantics_); }
};



} // namespace gologpp

#endif // GOLOGPP_LANGUAGE_H_
