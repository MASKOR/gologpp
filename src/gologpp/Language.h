#ifndef GOLOGPP_LANGUAGE_H_
#define GOLOGPP_LANGUAGE_H_

#include "gologpp.h"
#include <memory>
#include "Implementation.h"

#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/fusion/include/std_tuple.hpp>

namespace gologpp {
namespace generic {


class AbstractLanguageElement {
public:
	AbstractLanguageElement() = default;

	// Only movable, not copyable
	AbstractLanguageElement(const AbstractLanguageElement &) = delete;
	AbstractLanguageElement(AbstractLanguageElement &&) = default;
	AbstractLanguageElement &operator = (const AbstractLanguageElement &) = delete;
	//AbstractLanguageElement &operator = (AbstractLanguageElement &&) = default;

	virtual ~AbstractLanguageElement() = default;

	template<class GologT>
	Implementation<GologT> &impl_cast() const
	{ return dynamic_cast<Implementation<GologT> &>(implementation()); }

	virtual AbstractImplementation &implementation() const
	{ return *impl_; }

	void set_implementation(unique_ptr<AbstractImplementation> &&impl)
	{ impl_ = std::move(impl); }

	virtual void implement(Implementor &implementor) = 0;

protected:
	unique_ptr<AbstractImplementation> impl_;
};


#define DEFINE_IMPLEMENT_WITH_MEMBERS(...) \
	virtual void implement(Implementor &implementor) override { \
		impl_ = implementor.get_impl(*this); \
		boost::fusion::for_each(std::tie(__VA_ARGS__), [&] (auto &e) { \
			e.implement(implementor); \
		} ); \
	}

#define DEFINE_IMPLEMENT \
	virtual void implement(Implementor &implementor) override { \
		impl_ = implementor.get_impl(*this); \
	}


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
};



} // namespace generic
} // namespace gologpp

#endif // GOLOGPP_LANGUAGE_H_
