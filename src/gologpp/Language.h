#ifndef GOLOGPP_LANGUAGE_H_
#define GOLOGPP_LANGUAGE_H_

#include "gologpp.h"
#include <memory>
#include "Implementation.h"

#include <boost/fusion/include/std_tuple.hpp>
#include <boost/fusion/include/for_each.hpp>

namespace gologpp {


class AbstractLanguageElement {
public:
	AbstractLanguageElement() = default;

	// Only movable, not copyable
	AbstractLanguageElement(const AbstractLanguageElement &) = delete;
	AbstractLanguageElement(AbstractLanguageElement &&) = default;
	AbstractLanguageElement &operator = (const AbstractLanguageElement &) = delete;
	//AbstractLanguageElement &operator = (AbstractLanguageElement &&) = default;

	virtual ~AbstractLanguageElement() = default;

	template<class GologT = AbstractLanguageElement>
	Implementation<GologT> &implementation() const
	{ return dynamic_cast<Implementation<GologT> &>(*impl_); }

	void set_implementation(unique_ptr<AbstractImplementation> &&impl)
	{ impl_ = std::move(impl); }

	virtual void implement(Implementor &implementor) = 0;

protected:
	unique_ptr<AbstractImplementation> impl_;
};


#define DEFINE_IMPLEMENT_WITH_MEMBERS(...) \
	virtual void implement(Implementor &implementor) override { \
		if (!impl_) { \
			impl_ = implementor.make_impl(*this); \
			boost::fusion::for_each(std::tie(__VA_ARGS__), [&] (auto &e) { \
				e.implement(implementor); \
			} ); \
		} \
	}

#define DEFINE_IMPLEMENT \
	virtual void implement(Implementor &implementor) override { \
		if (!impl_) \
			impl_ = implementor.make_impl(*this); \
	}


class Expression;


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

	template<class = void>
	Implementation<GologT> &implementation() const
	{ return dynamic_cast<Implementation<GologT> &>(*impl_); }
};



} // namespace gologpp

#endif // GOLOGPP_LANGUAGE_H_
