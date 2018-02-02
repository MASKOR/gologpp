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
	AbstractLanguageElement(const AbstractLanguageElement &) = delete;
	AbstractLanguageElement(AbstractLanguageElement &&) = default;

	virtual ~AbstractLanguageElement() = default;
	virtual AbstractImplementation &implementation() const = 0;

	template<class rv_t = void>
	rv_t init() {
		init_this<rv_t>();
		init_members();
	}

protected:
	template<class rv_t = void>
	rv_t init_this() {}

	virtual void init_members() = 0;
};


template<class... member_ts>
class CompositeLanguageElement : public virtual AbstractLanguageElement {
public:
	typedef std::tuple<member_ts &...> members_t;
	
	virtual members_t members() = 0;

protected:
	virtual void init_members() override
	{ boost::fusion::for_each(members(), [] (AbstractLanguageElement &e) { e.init<void>(); }); }
};


template<class GologT, class... member_ts>
class LanguageElement : public CompositeLanguageElement<member_ts...> {
public:
	typedef GologT golog_t;

	LanguageElement()
	{}

	LanguageElement(LanguageElement &&) = default;
	LanguageElement &operator = (LanguageElement &&) = default;

	virtual AbstractImplementation &implementation() const override
	{
		/*if (!impl_)
			impl_ = make_unique<Implementation<GologT>>(dynamic_cast<GologT &>(*this));*/
		return *impl_;
	}

	template<class rv_t = void>
	rv_t init_this()
	{ impl_.reset(new Implementation<GologT>(dynamic_cast<GologT &>(*this))); }

private:
	unique_ptr<AbstractImplementation> impl_;
};



} // namespace generic
} // namespace gologpp

#endif // GOLOGPP_LANGUAGE_H_
