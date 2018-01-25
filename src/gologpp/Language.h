#ifndef GOLOGPP_LANGUAGE_H_
#define GOLOGPP_LANGUAGE_H_

#include "gologpp.h"
#include <memory>
#include "Implementation.h"

namespace gologpp {
namespace generic {

using namespace std;


class AbstractLanguageElement {
public:
	virtual ~AbstractLanguageElement();
	virtual AbstractImplementation &implementation() const = 0;
};


template<class GologT>
class LanguageElement : public virtual AbstractLanguageElement {
public:
	typedef GologT golog_t;

	LanguageElement(const GologT &obj)
	: impl_(new Implementation<GologT>(obj))
	{}

	virtual Implementation<GologT> &implementation() const override
	{
		/*if (!impl_)
			impl_ = make_unique<Implementation<GologT>>(dynamic_cast<GologT &>(*this));*/
		return *impl_;
	}

private:
	unique_ptr<Implementation<GologT>> impl_;
};



} // namespace generic
} // namespace gologpp

#endif // GOLOGPP_LANGUAGE_H_