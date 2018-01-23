#ifndef GOLOGPP_LANGUAGE_H_
#define GOLOGPP_LANGUAGE_H_

#include "gologpp.h"
#include <memory>

namespace gologpp {
namespace generic {

using namespace std;


class AbstractLanguageElement {
public:
	virtual ~AbstractLanguageElement();
	virtual shared_ptr<AbstractImplementation> implementation() = 0;
};


template<class ImplT>
class LanguageElement : public virtual AbstractLanguageElement {
public:
	LanguageElement()
	: implementation_(new Implementation<ImplT>())
	{}

	virtual shared_ptr<AbstractImplementation> implementation() override
	{ return implementation_; }

private:
	shared_ptr<Implementation<ImplT>> implementation_;
};



} // namespace generic
} // namespace gologpp

#endif // GOLOGPP_LANGUAGE_H_