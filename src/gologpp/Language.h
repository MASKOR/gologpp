#ifndef GOLOGPP_LANGUAGE_H_
#define GOLOGPP_LANGUAGE_H_

#include "gologpp.h"
#include <memory>
#include "Implementation.h"

namespace gologpp {
namespace generic {


class AbstractLanguageElement {
public:
	virtual ~AbstractLanguageElement();
	virtual AbstractImplementation &implementation() const = 0;
	virtual void init() = 0;
};


template<class GologT>
class LanguageElement : public virtual AbstractLanguageElement {
public:
	typedef GologT golog_t;

	LanguageElement(const GologT &)
	{}

	LanguageElement(LanguageElement &&) = default;
	LanguageElement &operator = (LanguageElement &&) = default;

	virtual Implementation<GologT> &implementation() const override
	{
		/*if (!impl_)
			impl_ = make_unique<Implementation<GologT>>(dynamic_cast<GologT &>(*this));*/
		return *impl_;
	}

	virtual void init() override
	{ impl_.reset(new Implementation<GologT>(dynamic_cast<GologT &>(*this))); }

private:
	unique_ptr<Implementation<GologT>> impl_;
};



} // namespace generic
} // namespace gologpp

#endif // GOLOGPP_LANGUAGE_H_
