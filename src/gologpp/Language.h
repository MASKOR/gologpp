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

	template<class GologT>
	Implementation<GologT> &impl_cast() const
	{ return dynamic_cast<Implementation<GologT> &>(implementation()); }

	void init(unique_ptr<AbstractImplementation> &&impl)
	{ impl_ = std::move(impl); }

	virtual AbstractImplementation &implementation() const
	{ return *impl_; }

protected:
	unique_ptr<AbstractImplementation> impl_;
};


template<class... member_ts>
class CompositeLanguageElement : public virtual AbstractLanguageElement {
public:
	typedef std::tuple<member_ts &...> members_t;
	
	virtual members_t members() = 0;

	template<class DummyT = void>
	void init_members()
	{ boost::fusion::for_each(members(), [] (auto &e) { e.template init<decltype(e)>(); }); }
};


template<class GologT>
class LanguageElement : public virtual AbstractLanguageElement {
public:
	typedef GologT golog_t;

	LanguageElement()
	{}

	LanguageElement(LanguageElement &&) = default;
	LanguageElement &operator = (LanguageElement &&) = default;

	template<class... ArgTs>
	static GologT construct(ArgTs... args)
	{
		GologT rv(std::forward<ArgTs>(args)...);
		rv.init(std::make_unique<Implementation<GologT>>(rv));
		return rv;
	}
};



} // namespace generic
} // namespace gologpp

#endif // GOLOGPP_LANGUAGE_H_
